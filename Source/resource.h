#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include "float2.h"
#include "float3.h"

#define pi 3.141592654

extern char exepath[MAX_PATH]; // Current directory

// UI elements
extern HWND ThreadsLbl;
extern HWND ThreadsEdit;
extern HWND BlockSizeLbl;
extern HWND BlockSizeEdit;
extern HWND RenderSizeLbl;
extern HWND	RenderSizeTimesLbl;
extern HWND RenderSizeXEdit;
extern HWND RenderSizeYEdit;
extern HWND BouncesLbl;
extern HWND BouncesEdit;
extern HWND SamplesLbl;
extern HWND SamplesEdit;
extern HWND StartBtn;
extern HWND LogLbl;

// UI elements, ids
#define ID_THREADSLBL 1001
#define ID_THREADSEDIT 1002
#define ID_BLOCKSIZELBL 1003
#define ID_BLOCKSIZEEDIT 1004
#define ID_RENDERSIZELBL 1005
#define ID_RENDERSIZETIMESLBL 1006
#define ID_RENDERSIZEXEDIT 1007
#define ID_RENDERSIZEYEDIT 1008
#define ID_BOUNCESEDIT 1009
#define ID_BOUNCESLBL 1010
#define ID_SAMPLESLBL 1011
#define ID_SAMPLESEDIT 1012
#define ID_STARTBTN 109

// Math stuff
float DegToRad(float degin);
float RadToDeg(float radin);
float3 GramSchmidt2(float3 v1,float3 x2);
float3 GramSchmidt3(float3 v1,float3 v2,float3 x3);
bool fequals(float x1,float x2);
float RandomRange(float min,float max);
float GetReflectance(float3 input,float3 normal,float n1,float n2); // fresnel

// Path stuff
void GetFullPath(const char* file,const char* folder,char* fullpath);
char* ExtractFileName(const char* text,char* result);
char* ExtractFilePath(const char* text,char* result);
char* ExtractFileExt(const char* text,char* result);
char* ChangeFileExt(const char* text,const char* newext,char* result);

// String stuff
int CountChar(const char* text,char token);
char* TrimLeft(char* text);
char* TrimRight(char* text);
char* Trim(char* text);

// Responsiveness
void ProcessMessages(HWND hwnd);

#endif
