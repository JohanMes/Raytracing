#include "resource.h"
#include "Scene.h"
#include "Camera.h"
#include "Models.h"
#include "Materials.h"
#include "Textures.h"
#include "Renderer.h"

// Centrale materiaaldumps
Models* models = NULL;
Materials* materials = NULL;
Textures* textures = NULL;
Renderer* renderer = NULL; // de centrale class
Scene* scene = NULL;
Camera* camera = NULL;
char exepath[MAX_PATH] = ""; // Current directory

// UI elements
HWND ThreadsLbl = NULL;
HWND ThreadsEdit = NULL;
HWND BlockSizeLbl = NULL;
HWND BlockSizeEdit = NULL;
HWND RenderSizeLbl = NULL;
HWND RenderSizeTimesLbl = NULL;
HWND RenderSizeXEdit = NULL;
HWND RenderSizeYEdit = NULL;
HWND BouncesLbl = NULL;
HWND BouncesEdit = NULL;
HWND SamplesLbl = NULL;
HWND SamplesEdit = NULL;
HWND StartBtn = NULL;
HWND LogLbl = NULL;

// Math stuff
float DegToRad(float degin) {
	return degin/180.0f*pi;
}
float RadToDeg(float radin) {
	return radin*180.0f/pi;
}
float3 GramSchmidt2(float3 v1,float3 x2) {

	// v2 = x2 - dot(v1 * x2)/dot(v1,v1) * v1
	float factor1 = v1.Dot(x2)/v1.Dot(v1);

	return x2 - factor1 * v1;
}
float3 GramSchmidt3(float3 v1,float3 v2,float3 x3) {

	// v3 = x3 - dot(v1,x3)/dot(v1,v1) * v1 - dot(v2,x3)/dot(v2,v2) * v2
	float factor1 = v1.Dot(x3)/v1.Dot(v1);
	float factor2 = v2.Dot(x3)/v2.Dot(v2);

	return x3 - factor1 * v1 - factor2 * v2;
}
bool fequals(float x1,float x2) {
	return x1 + 0.000001f >= x2 && x1 - 0.000001f <= x2;
}
float RandomRange(float min,float max) {
	// Scale the range to [0,1]
	float baserange = (float)rand()/(float)RAND_MAX;
	
	// Then scale to [0,(max-min)]
	float scalerange = baserange*(max - min);
	
	// And move by min to [min,max]
	float finalrange = scalerange + min;
	return finalrange;
}
float GetReflectance(float3 input,float3 normal,float n1,float n2) {
	float refractindex = n1/n2;
	float cosinput = -normal.Dot(input);
	float sintranssquared = refractindex * refractindex * (1 - cosinput * cosinput);
	
	// Do not transmit rays in the case of TIR
	if(sintranssquared > 1.0f) {
		return 1.0f;
	}
	
	float costrans = sqrt(1 - sintranssquared);
	float n1cosinput = n1 * cosinput;
	float n2cosinput = n2 * cosinput;
	float n1costrans = n1 * costrans;
	float n2costrans = n2 * costrans;
	
	// Average reflectance for both polarizations
	float Rperpendicular = pow((n1cosinput - n2costrans) / (n1cosinput + n2costrans),2);
	float Rparallel      = pow((n2cosinput - n1costrans) / (n2cosinput + n1costrans),2);
	return (Rperpendicular + Rparallel) / 2.0f;
}

// Path stuff
void GetFullPath(const char* file,const char* folder,char* fullpath) {
	if(file[1] != ':') { // prepend exe dir
		sprintf(fullpath,"%s\\%s\\%s",exepath,folder,file);
	} else {
		strcpy(fullpath,file);
	}
}
char* ExtractFileName(const char* text,char* result) {
	char* lastslash = strrchr(text,'\\');
	if(lastslash == NULL) { // geen pad gegeven, dus text is alleen de naam
		sprintf(result,"%s",text);
	} else { // vanaf laatste slash kopieren
		strcpy(result,++lastslash);
	}

	return result;
}
char* ExtractFilePath(const char* text,char* result) {
	char* lastslash = strrchr(text,'\\');
	if(lastslash == NULL) { // geen mappad gegeven,niks returnen
		result[0] = 0;
	} else { // wel mappad gegeven, kopieren t/m dat ding
		lastslash++; // neem slash mee
		strncpy(result,text,lastslash-text);
		result[lastslash-text] = '\0'; // En zet er een 0 achter
	}

	return result;
}
char* ExtractFileExt(const char* text,char* result) {
	char* period = strrchr(text,'.');
	if(period == NULL) { // geen extensie gegeven, niks returnen
		result[0] = 0;
	} else { // wel extensie gegeven, kopieren vanaf dat ding
		strcpy(result,++period);
	}

	return result;
}
char* ChangeFileExt(const char* text,const char* newext,char* result) {

	// Sla alles op TOT punt...
	char* period = strrchr(text,'.');
	if(period == NULL) { // geen extensie gegeven, mooi
		result[0] = 0;
	} else { // wel extensie gegeven, kopieren tot dat ding
		strncpy(result,text,period-text);
		result[period-text] = '\0'; // En zet er een 0 achter
	}

	// gooi de nieuwe ext erachter
	strcat(result,newext);

	return result;
}

// String stuff
int CountChar(const char* text,char token) {
	int len = strlen(text);
	int result = 0;
	for(int i = 0;i < len;i++) {
		if(text[i] == token) {
			result++;
		}
	}
	return result;
}
char* TrimLeft(char* text) {
	while(isspace(*text)) {
		*text = 0; // zero fill
		text++;
	}
	return text;
}
char* TrimRight(char* text) {
	char* end = text + strlen(text) - 1;
	while(isspace(*end)) {
		*end = 0; // zero fill
		end--;
	}
	return text;
}
char* Trim(char* text) {
	return TrimLeft(TrimRight(text));
}

// Responsiveness
void ProcessMessages(HWND hwnd) {
	MSG Msg;
	while(PeekMessage(&Msg,0,0,0,PM_REMOVE)) {
		if(!IsDialogMessage(hwnd,&Msg)) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
}
