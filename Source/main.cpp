#include "Scene.h"
#include "Models.h"
#include "Materials.h"
#include "Camera.h"
#include "Renderer.h"
#include "resource.h"

#define UI_HEIGHT 28

void InitRenderer(HWND hwnd) {
	// Begin altijd met andere random waardes
	srand(GetTickCount());

	// Sla de current directory op
	char fullexepath[MAX_PATH];
	GetModuleFileName(NULL,fullexepath,MAX_PATH);
	ExtractFilePath(fullexepath,exepath);
	
	// Maak dumps voor gegevens
	models = new Models();
	materials = new Materials();
	textures = new Textures();
	
	// Maak main class
	renderer = new Renderer();
	
	// Maak camera en scene, en plak aan renderer vast
	scene = new Scene();
	camera = new Camera();
	camera->SetPos(float3(-6,2,-6));
	camera->SetLookAt(float3(0,2.3,0));
	
	// Bekijk hoe groot ons venster is
	RECT clientrect;
	GetClientRect(hwnd,&clientrect);
	int width = clientrect.right - clientrect.left;
	int height = clientrect.bottom - clientrect.top - UI_HEIGHT;
	
	// Plak vast
	renderer->SetCamera(camera);
	renderer->SetScene(scene);
	renderer->SetTargetWindow(hwnd);
	renderer->SetResolution(width/2,height/2);
	renderer->SetBounces(0);
	renderer->SetSamples(1); // heel sloom
}

void LoadMaterials() {
	Material* dummy;
	
	dummy = materials->Add("glass");
	dummy->SetColor(float3(0.01,0.01,0.01));
	dummy->SetSpecularValue(10);
	dummy->SetShininess(50); // sharpness of reflection
	dummy->SetRefractiveIndex(1.035);
	dummy->SetOpacity(0.5);
	
	dummy = materials->Add("sadglass");
	dummy->SetColor(float3(0.01,0.01,0.01));
	dummy->SetSpecularValue(10);
	dummy->SetShininess(50); // sharpness of reflection
	dummy->SetRefractiveIndex(2);
	dummy->SetOpacity(0.5);
	
	dummy = materials->Add("checker");
	dummy->SetColor(float3(0,0,0));
	dummy->SetSpecularValue(0);
	dummy->SetShininess(0); // sharpness of reflection
	dummy->SetRefractiveIndex(1);
	dummy->SetTexture("CheckerBmp","Checker.bmp");
	
	dummy = materials->Add("redmatte");
	dummy->SetColor(float3(1,0,0));
	dummy->SetSpecularValue(0);
	dummy->SetShininess(0); // sharpness of reflection
	dummy->SetRefractiveIndex(1);
	
	dummy = materials->Add("greenshiny");
	dummy->SetColor(float3(0,1,0));
	dummy->SetSpecularValue(20);
	dummy->SetShininess(15); // sharpness of reflection
	dummy->SetRefractiveIndex(1);

	dummy = materials->Add("ground");
	dummy->SetColor(float3(0.2,0.2,0.2));
	dummy->SetSpecularValue(0);
	dummy->SetShininess(0); // sharpness of reflection
	dummy->SetRefractiveIndex(1);

//	dummy = materials->Add("skybox");
//	dummy->SetSpecularValue(0);
//	dummy->SetShininess(0); // sharpness of reflection
//	dummy->SetRefractiveIndex(1);
//	dummy->SetTexture("skybox2","Skybox_2.bmp");
}
void LoadObjects() {
	Object* dummy;
	
	dummy = new Object();
	dummy->SetModel("Sphere2.obj");
	dummy->SetTranslation(float3(-2,2,-2));
	dummy->SetMaterial("glass");
	scene->AddObject(dummy);
	
	dummy = new Object();
	dummy->SetModel("Sphere.obj");
	dummy->SetTranslation(float3(0,1,-5));
	dummy->SetMaterial("redmatte");
	scene->AddObject(dummy);
	
	dummy = new Object();
	dummy->SetModel("Sphere.obj");
	dummy->SetTranslation(float3(-5,1,0));
	dummy->SetMaterial("sadglass");
	scene->AddObject(dummy);
	
	dummy = new Object();
	dummy->SetModel("Wall.obj");
	dummy->SetTranslation(float3(2,0,-3));
	dummy->SetRotation(float3(0,0,0));
	dummy->SetMaterial("checker");
	scene->AddObject(dummy);
	
	dummy = new Object();
	dummy->SetModel("Wall.obj");
	dummy->SetTranslation(float3(-3,0,2));
	dummy->SetRotation(float3(0,90,0));
	dummy->SetMaterial("checker");
	scene->AddObject(dummy);
	
//	dummy = new Object();
//	dummy->SetModel("Cube.obj");
//	dummy->SetTranslation(float3(-5,0,-5));
//	dummy->SetMaterial("redmatte");
//	scene->AddObject(dummy);
	
	dummy = new Object();
	dummy->SetModel("Plane.obj");
	dummy->SetTranslation(float3(0,0,0));
	dummy->SetMaterial("ground");
	scene->AddObject(dummy);
}

void LoadLights() {
	Light* dummy;

	dummy = new Light(ltDirection);
	dummy->SetDirection(-float3(0.33,-1,1));
	dummy->SetColor(10*float3(1.4,1.4,1.2));
	scene->AddLight(dummy);

//	dummy = new Light(ltPoint);
//	dummy->SetPosition(float3(-5,2,-2));
//	dummy->SetColor(10*float3(1.4,1.4,1.2));
//	scene->AddLight(dummy);
}

void LoadScene() {
	LoadMaterials();
	LoadObjects();
	LoadLights();
}

void InitUI(HWND hwnd) {
	char text[128];
	
	// Aantal threads
	ThreadsLbl = CreateWindow("STATIC","Threads:",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)ID_THREADSLBL,GetModuleHandle(NULL),NULL);
	ThreadsEdit = CreateWindow("EDIT","",WS_BORDER|ES_NUMBER|WS_CHILD|WS_VISIBLE|WS_TABSTOP,0,0,0,0,hwnd,(HMENU)ID_THREADSEDIT,GetModuleHandle(NULL),NULL);
	
	// Fill out box
	snprintf(text,128,"%d",renderer->GetNumThreads());
	SetWindowText(ThreadsEdit,text);

	// Blokgrootte
	BlockSizeLbl = CreateWindow("STATIC","Block size:",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)ID_BLOCKSIZELBL,GetModuleHandle(NULL),NULL);
	BlockSizeEdit = CreateWindow("EDIT","",WS_BORDER|ES_NUMBER|WS_CHILD|WS_VISIBLE|WS_TABSTOP,0,0,0,0,hwnd,(HMENU)ID_BLOCKSIZEEDIT,GetModuleHandle(NULL),NULL);
	
	// Fill out box
	snprintf(text,128,"%d",renderer->GetBlockSize());
	SetWindowText(BlockSizeEdit,text);

	// Resolutie
	RenderSizeLbl = CreateWindow("STATIC","Resolution:",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)ID_RENDERSIZELBL,GetModuleHandle(NULL),NULL);
	RenderSizeXEdit = CreateWindow("EDIT","",WS_BORDER|ES_NUMBER|WS_CHILD|WS_VISIBLE|WS_TABSTOP,0,0,0,0,hwnd,(HMENU)ID_RENDERSIZEXEDIT,GetModuleHandle(NULL),NULL);
	RenderSizeTimesLbl = CreateWindow("STATIC","x",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)ID_RENDERSIZETIMESLBL,GetModuleHandle(NULL),NULL);
	RenderSizeYEdit = CreateWindow("EDIT","",WS_BORDER|ES_NUMBER|WS_CHILD|WS_VISIBLE|WS_TABSTOP,0,0,0,0,hwnd,(HMENU)ID_RENDERSIZEYEDIT,GetModuleHandle(NULL),NULL);
	
	// Fill out boxex
	snprintf(text,128,"%d",renderer->GetResolutionX());
	SetWindowText(RenderSizeXEdit,text);
	snprintf(text,128,"%d",renderer->GetResolutionY());
	SetWindowText(RenderSizeYEdit,text);

	// Aantal bounces
	BouncesLbl = CreateWindow("STATIC","Bounces:",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)ID_BOUNCESLBL,GetModuleHandle(NULL),NULL);
	BouncesEdit = CreateWindow("EDIT","",WS_BORDER|ES_NUMBER|WS_CHILD|WS_VISIBLE|WS_TABSTOP,0,0,0,0,hwnd,(HMENU)ID_BOUNCESEDIT,GetModuleHandle(NULL),NULL);
	
	// Fill out box
	snprintf(text,128,"%d",renderer->GetBounces());
	SetWindowText(BouncesEdit,text);

	// Aantal samples per pixel
	SamplesLbl = CreateWindow("STATIC","Samples:",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)ID_SAMPLESLBL,GetModuleHandle(NULL),NULL);
	SamplesEdit = CreateWindow("EDIT","",WS_BORDER|ES_NUMBER|WS_CHILD|WS_VISIBLE|WS_TABSTOP,0,0,0,0,hwnd,(HMENU)ID_SAMPLESEDIT,GetModuleHandle(NULL),NULL);
		
	// Fill out box
	snprintf(text,128,"%d",renderer->GetSamples());
	SetWindowText(SamplesEdit,text);
	
	// Debugtekst
	LogLbl = CreateWindow("STATIC","",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)ID_BOUNCESLBL,GetModuleHandle(NULL),NULL);
		
	// Start button
	StartBtn = CreateWindow("BUTTON","Render",WS_CHILD|WS_VISIBLE,0,0,0,0,hwnd,(HMENU)ID_STARTBTN,GetModuleHandle(NULL),NULL);
	
	// Maak mooi font
	HDC dc = GetDC(hwnd);
	HFONT font = CreateFont(-MulDiv(9,GetDeviceCaps(dc,LOGPIXELSY),72),0,0,0,0,0,0,0,0,0,0,0,0,"Segoe UI");
	SendMessage(ThreadsLbl,WM_SETFONT,(WPARAM)font,0);
	SendMessage(ThreadsEdit,WM_SETFONT,(WPARAM)font,0);
	
	SendMessage(BlockSizeLbl,WM_SETFONT,(WPARAM)font,0);
	SendMessage(BlockSizeEdit,WM_SETFONT,(WPARAM)font,0);
	
	SendMessage(RenderSizeLbl,WM_SETFONT,(WPARAM)font,0);
	SendMessage(RenderSizeXEdit,WM_SETFONT,(WPARAM)font,0);
	SendMessage(RenderSizeTimesLbl,WM_SETFONT,(WPARAM)font,0);
	SendMessage(RenderSizeYEdit,WM_SETFONT,(WPARAM)font,0);
	
	SendMessage(BouncesLbl,WM_SETFONT,(WPARAM)font,0);
	SendMessage(BouncesEdit,WM_SETFONT,(WPARAM)font,0);

	SendMessage(SamplesLbl,WM_SETFONT,(WPARAM)font,0);
	SendMessage(SamplesEdit,WM_SETFONT,(WPARAM)font,0);
	
	SendMessage(LogLbl,WM_SETFONT,(WPARAM)font,0);
			
	SendMessage(StartBtn,WM_SETFONT,(WPARAM)font,0);
	ReleaseDC(hwnd,dc);
}

void ResizeUI(HWND hwnd) {
	RECT rect;
	GetClientRect(hwnd,&rect);

	// Threads
	SetWindowPos(ThreadsLbl,NULL,
		5,
		rect.bottom-UI_HEIGHT+5,
		60,
		UI_HEIGHT-4,
		SWP_NOZORDER);
	SetWindowPos(ThreadsEdit,NULL,
		65,
		rect.bottom-UI_HEIGHT+2,
		40,
		UI_HEIGHT-4,
		SWP_NOZORDER);
		
	// Block size
	SetWindowPos(BlockSizeLbl,NULL,
		130,
		rect.bottom-UI_HEIGHT+5,
		100,
		UI_HEIGHT-4,
		SWP_NOZORDER);
	SetWindowPos(BlockSizeEdit,NULL,
		195,
		rect.bottom-UI_HEIGHT+2,
		40,
		UI_HEIGHT-4,
		SWP_NOZORDER);

	// Resolutie
	SetWindowPos(RenderSizeLbl,NULL,
		255,
		rect.bottom-UI_HEIGHT+5,
		100,
		UI_HEIGHT-4,
		SWP_NOZORDER);
	SetWindowPos(RenderSizeXEdit,NULL,
		325,
		rect.bottom-UI_HEIGHT+2,
		40,
		UI_HEIGHT-4,
		SWP_NOZORDER);
	SetWindowPos(RenderSizeTimesLbl,NULL,
		370,
		rect.bottom-UI_HEIGHT+5,
		10,
		UI_HEIGHT-4,
		SWP_NOZORDER);
	SetWindowPos(RenderSizeYEdit,NULL,
		385,
		rect.bottom-UI_HEIGHT+2,
		40,
		UI_HEIGHT-4,
		SWP_NOZORDER);
	
	// Bounces
	SetWindowPos(BouncesLbl,NULL,
		440,
		rect.bottom-UI_HEIGHT+5,
		100,
		UI_HEIGHT-4,
		SWP_NOZORDER);
	SetWindowPos(BouncesEdit,NULL,
		500,
		rect.bottom-UI_HEIGHT+2,
		40,
		UI_HEIGHT-4,
		SWP_NOZORDER);

	// Samples
	SetWindowPos(SamplesLbl,NULL,
		560,
		rect.bottom-UI_HEIGHT+5,
		100,
		UI_HEIGHT-4,
		SWP_NOZORDER);
	SetWindowPos(SamplesEdit,NULL,
		620,
		rect.bottom-UI_HEIGHT+2,
		40,
		UI_HEIGHT-4,
		SWP_NOZORDER);
		
	// Log
	SetWindowPos(LogLbl,NULL,
		680,
		rect.bottom-UI_HEIGHT+6,
		rect.right - 65 - 680,
		UI_HEIGHT-4,
		SWP_NOZORDER);
	
	// Startknop
	SetWindowPos(StartBtn,NULL,
		rect.right - 65,
		rect.bottom-UI_HEIGHT+2,
		60,
		UI_HEIGHT-4,
		SWP_NOZORDER);
}

void ApplyUI(HWND hwnd) {
	char text[128];
	int value1 = 0,value2 = 0;

	// Read box and send info
	GetWindowText(ThreadsEdit,text,128);
	if(sscanf(text,"%d",&value1) == 1) {
		renderer->SetNumThreads(value1);
	}
	
	// Idem
	GetWindowText(BlockSizeEdit,text,128);
	if(sscanf(text,"%d",&value1) == 1) {
		renderer->SetBlockSize(value1);
	}
	
	// Idem
	GetWindowText(BouncesEdit,text,128);
	if(sscanf(text,"%d",&value1) == 1) {
		renderer->SetBounces(value1);
	}
	
	// Idem
	GetWindowText(SamplesEdit,text,128);
	if(sscanf(text,"%d",&value1) == 1) {
		renderer->SetSamples(value1);
	}
		
	// Idem2
	GetWindowText(RenderSizeXEdit,text,128);
	if(sscanf(text,"%d",&value1) == 1) {
		GetWindowText(RenderSizeYEdit,text,128);
		if(sscanf(text,"%d",&value2) == 1) {
			renderer->SetResolution(value1,value2);
		}
	}
}

void DeleteRenderer() {
	delete renderer;
	delete materials;
	delete textures;
	delete models;
	delete scene;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {
		case WM_COMMAND: {
			switch(wParam) {
				case ID_STARTBTN: {
					ApplyUI(hwnd);
					renderer->Render();					
					break;
				}
			}
			break;
		}
		case WM_KEYDOWN: {
			switch(wParam) {
				case VK_F5: {
					ApplyUI(hwnd);
					renderer->Render();						
					break;
				}
			}
			break;
		}
		case WM_CREATE: {
			// Init renderer
			InitRenderer(hwnd);
			
			// Load some dummy objects
			LoadScene();
			
			// Add some user control
			InitUI(hwnd);
			break;
		}
		case WM_SIZE: {
//			if(renderer) {
//				renderer->Repaint(hwnd);
//			}

			ResizeUI(hwnd);
			break;
		}
		case WM_PAINT: {
			renderer->Repaint(hwnd);
			break;
		}
		case WM_MOUSEMOVE: {
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			
			// Get pixel in current work area
			float3* pixel = renderer->GetPixelValue(x,y);

			// If the pixel exists, update caption
			if(pixel) {
				char buffer[256];
				snprintf(buffer,256,"R: %4f, G: %4f, B: %4f",
					pixel->x,
					pixel->y,
					pixel->z);
					
				// Update log text
				SendMessage(LogLbl,WM_SETTEXT,0,(LPARAM)&buffer);
			}
			break;
		}
		case WM_DESTROY: {
			DeleteRenderer();
			PostQuitMessage(0);
			break;
		}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;
	
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc;
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, "A"); /* use "A" as icon name when you want to use the project icon */
	wc.hIconSm		 = LoadIcon(NULL, "A"); /* as above */

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	// Set up a window with x usable pixels
	RECT result = {0,0,1280,720 + UI_HEIGHT}; // 30 for UI elements
	AdjustWindowRect(&result,WS_VISIBLE|WS_OVERLAPPEDWINDOW,false);
	hwnd = CreateWindow("WindowClass","Raytracing",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, // x
		CW_USEDEFAULT, // y
		result.right - result.left, // width
		result.bottom - result.top, // height
		NULL,NULL,hInstance,NULL);
	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	while(GetMessage(&Msg, NULL, 0, 0) > 0) {
		if(!IsDialogMessage(hwnd,&Msg)) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
	
	return Msg.wParam;
}
