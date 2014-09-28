#include "Renderer.h"

Renderer::Renderer() {
	blocksize = 48;
	targetxsize = 0;
	targetysize = 0;
	bounces = 1;
	samples = 4;
	numthreads = std::thread::hardware_concurrency();
	
	// Set SSAA sampling points
	sampleoffsets.push_back(float2( 0.40, 0.15));
	sampleoffsets.push_back(float2(-0.40,-0.15));
	sampleoffsets.push_back(float2(-0.15, 0.40));
	sampleoffsets.push_back(float2( 0.15,-0.40));
}

Renderer::~Renderer() {
	
}

int Renderer::GetBounces() {
	return bounces;
}

int Renderer::GetNumThreads() {
	return numthreads;
}

int Renderer::GetBlockSize() {
	return blocksize;
}

int Renderer::GetResolutionX() {
	return targetxsize;
}

int Renderer::GetResolutionY() {
	return targetysize;
}

int Renderer::GetSamples() {
	return samples;
}

float3* Renderer::GetPixelValue(int x,int y) {
	if(x >= 0 and x < targetxsize) {
		if(y >= 0 and y < targetysize) {
			return &target[x][y];
		}
	}
	return NULL;
}

void Renderer::SetScene(Scene* scene) {
	this->scene = scene;
}

void Renderer::SetCamera(Camera* camera) {
	this->camera = camera;
}

void Renderer::SetTargetWindow(HWND hwnd) {
	this->hwnd = hwnd;
}

void Renderer::SetResolution(int x,int y) {
	target.clear();
	
	// Resize
	target.resize(x);
	for(int i = 0;i < x;i++) {
		target[i].resize(y);
	}
	targetxsize = x;
	targetysize = y;
	
	if(camera) {
		camera->SetRatio((float)x/(float)y);
	}
}

void Renderer::SetNumThreads(int numthreads) {
	this->numthreads = numthreads;
}

void Renderer::SetBlockSize(int blocksize) {
	this->blocksize = blocksize;
}

void Renderer::SetBounces(int bounces) {
	this->bounces = bounces;
}

void Renderer::SetSamples(int samples) {
	this->samples = samples;
}

bool Renderer::GetNextWorkBlock(RECT* block) {
	if(block->right + blocksize < targetxsize) { // whole column available
		block->left += blocksize;
		block->right += blocksize;
		return true;
	} else if(block->left + blocksize < targetxsize) { // column slice available
		block->left += blocksize;
		block->right = targetxsize;
		return true;
	} else if(block->bottom + blocksize < targetysize) { // whole row available
		block->left = 0; // next row
		block->right = blocksize;
		block->top += blocksize; // next row
		block->bottom += blocksize;
		return true;
	} else if(block->top + blocksize < targetysize) { // row slice available
		block->left = 0; // next row
		block->right = blocksize;
		block->top += blocksize;
		block->bottom = targetysize;
		return true;
	} else {
		return false;
	}
}

void Renderer::ThreadWork(Renderer* renderer) {

	// Fetch work from array
	while(renderer->workblockindex < (int)renderer->workblocks.size()) {
		
		// Start processing this one
		RECT* block = &renderer->workblocks[renderer->workblockindex];
		
		// Mark it as done
		renderer->workblockindex++;
		
		// Paint in-progress shape
		HDC hdc = GetDC(renderer->hwnd);
		renderer->DrawProgressShape(block,hdc);
		ReleaseDC(renderer->hwnd,hdc);
		
		// Cast numsamples rays for each pixel
		for(int x = block->left;x < block->right;x++) {
			for(int y = block->top;y < block->bottom;y++) {
				
				// Reuse ray in heap
				Ray* ray = new Ray();
				ray->SetEyePoint(renderer->camera->GetPos());
				ray->SetScene(renderer->scene);
				
				// Add samples, and divide afterwards
				renderer->target[x][y] = {0,0,0};
				for(int i = 0;i < renderer->samples;i++) {
					ray->ClearOriginFace();
					ray->SetBounces(renderer->bounces);
					ray->SetOrigin(renderer->camera->GetPos());
					ray->SetDirection(renderer->GetPixelWorldRay(float2(x,y) + renderer->sampleoffsets[i]));
					ray->SetRefractiveIndex(1.0f); // air
					renderer->target[x][y] += ray->Gather();
				}
				renderer->target[x][y] /= renderer->samples;
				
				// EOL
				delete ray;
			}
		}
		
		// Paint result square
		hdc = GetDC(renderer->hwnd);
		renderer->DrawResultColor(block,hdc);
		ReleaseDC(renderer->hwnd,hdc);
	}
}

void Renderer::Render() {
	Clock* clock = new Clock(false);
	float fromtime = clock->GetTimeSec();
	
	// Prepare an array of work
	workblocks.clear();
	RECT block = {
		0,
		0,
		std::min(blocksize,targetxsize),
		std::min(blocksize,targetysize)
	};
	do {
		workblocks.emplace_back(block);
	} while(GetNextWorkBlock(&block));
	
	// Start processing the array of work here
	workblockindex = 0;
	
	// Spawn n threads
	vector<thread> threadlist;
	for(int i = 0;i < numthreads;i++) {
		threadlist.push_back(thread(ThreadWork,this));
	}
	
	// Wait for them to finish
	for(int i = 0;i < numthreads;i++) {
		threadlist[i].join();
	}	

	float totime = clock->GetTimeSec();
	float difftime = totime - fromtime;
	delete clock;

	// Print result time in window caption
	char result[256];
	snprintf(result,256,"Done in %.3g seconds (%.3g FPS)!",difftime,1.0f/difftime);
	SendMessage(hwnd,WM_SETTEXT,0,(LPARAM)result);
}

void Renderer::DrawProgressShape(RECT* boundingrect,HDC hdc) {
	SetBkColor(hdc,RGB(255,255,255));
	
	// Don't redraw everything
	int xfrom = boundingrect->left;
	int xto = boundingrect->right;
	int yfrom = boundingrect->top;
	int yto = boundingrect->bottom;
	
	// Top left
	RECT rect = {xfrom,yfrom,xfrom+1,yfrom+1};
	ExtTextOut(hdc,0,0,ETO_OPAQUE,&rect,"",0,NULL);
	rect = {xfrom+1,yfrom,xfrom+2,yfrom+1};
	ExtTextOut(hdc,0,0,ETO_OPAQUE,&rect,"",0,NULL);
	rect = {xfrom,yfrom+1,xfrom+1,yfrom+2};
	ExtTextOut(hdc,0,0,ETO_OPAQUE,&rect,"",0,NULL);
	
	// Top right
	rect = {xto-1,yfrom,xto,yfrom+1};
	ExtTextOut(hdc,0,0,ETO_OPAQUE,&rect,"",0,NULL);
	rect = {xto-2,yfrom,xto-1,yfrom+1};
	ExtTextOut(hdc,0,0,ETO_OPAQUE,&rect,"",0,NULL);
	rect = {xto-1,yfrom+1,xto,yfrom+2};
	ExtTextOut(hdc,0,0,ETO_OPAQUE,&rect,"",0,NULL);
	
	// Bottom left
	rect = {xfrom,yto-1,xfrom+1,yto};
	ExtTextOut(hdc,0,0,ETO_OPAQUE,&rect,"",0,NULL);
	rect = {xfrom+1,yto-1,xfrom+2,yto};
	ExtTextOut(hdc,0,0,ETO_OPAQUE,&rect,"",0,NULL);
	rect = {xfrom,yto-2,xfrom+1,yto-1};
	ExtTextOut(hdc,0,0,ETO_OPAQUE,&rect,"",0,NULL);
	
	// Bottom right
	rect = {xto-1,yto-1,xto,yto};
	ExtTextOut(hdc,0,0,ETO_OPAQUE,&rect,"",0,NULL);
	rect = {xto-2,yto-1,xto-1,yto};
	ExtTextOut(hdc,0,0,ETO_OPAQUE,&rect,"",0,NULL);
	rect = {xto-1,yto-2,xto,yto-1};
	ExtTextOut(hdc,0,0,ETO_OPAQUE,&rect,"",0,NULL);
}

void Renderer::DrawResultColor(RECT* boundingrect,HDC hdc) {
	// Don't redraw everything
	int xfrom = boundingrect->left;
	int xto = boundingrect->right;
	int yfrom = boundingrect->top;
	int yto = boundingrect->bottom;

	for(int x = xfrom;x < xto;x++) {
		for(int y = yfrom;y < yto;y++) {
			RECT rect = {x,y,x+1,y+1};
			
			// Perform HDR to LDR conversion here
			float3 hdrcolor = target[x][y];
			
			// Reinhard + gamma correction
		//	float3 ldrcolor = hdrcolor.Div(hdrcolor + float3(1));
		//	float3 ldrcolorcorrected = ldrcolor.Pow(float3(1/2.2f));
	
			// John Hable + gamma correction
			float A = 0.22;
			float B = 0.30;
			float C = 0.10;
			float D = 0.20;
			float E = 0.01;
			float F = 0.30;
			float3 ldrcolorupper = hdrcolor.Mul(A*hdrcolor + float3(C*B)) + float3(D*E);
			float3 ldrcolorlower = hdrcolor.Mul(A*hdrcolor + float3(B))   + float3(D*F);
			float3 ldrcolor = ldrcolorupper.Div(ldrcolorlower) - float3(E/F);
			float3 ldrcolorcorrected = ldrcolor.Pow(float3(1/2.2f));
				
			SetBkColor(hdc,RGB(
				ldrcolorcorrected.x * 255,
				ldrcolorcorrected.y * 255,
				ldrcolorcorrected.z * 255
			));

			// Deze is veel sneller dan FillRect of Rectangle
			ExtTextOut(hdc,0,0,ETO_OPAQUE,&rect,"",0,NULL);
		}
	}
}

void Renderer::Repaint(HWND hwnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd,&ps);
	
	// Don't redraw everything
	RECT boundingrect;
	boundingrect.left = ps.rcPaint.left;
	boundingrect.right = std::min(targetxsize,(int)ps.rcPaint.right);
	boundingrect.top = ps.rcPaint.top;
	boundingrect.bottom = std::min(targetysize,(int)ps.rcPaint.bottom);
	DrawResultColor(&boundingrect,hdc);

	EndPaint(hwnd,&ps);
}

float3 Renderer::GetPixelWorldRay(float2 pixelpos) {
	float2 projectionpos = PixelsToProjection(pixelpos);

	// Muis in view coords
	float3 mouseview(
		projectionpos.x / camera->GetProjMatrix()._11,
		projectionpos.y / camera->GetProjMatrix()._22,
		1.0f);

	// Muis in world space (z = z)
	return mouseview.TransformNormal(camera->GetViewInverseMatrix());
}
float2 Renderer::PixelsToProjection(float2 pixelpos) {
	float2 result;
	result.x =  2.0f * pixelpos.x / (float)targetxsize - 1.0f;
	result.y = -2.0f * pixelpos.y / (float)targetysize + 1.0f;
	return result; // [-1,1] voor x en y, resultaat van P(V(W(x)))
}
float2 Renderer::ProjectionToPixels(float2 projectionpos) {
	float2 result;
	result.x =  (projectionpos.x * targetxsize + targetxsize)  / 2.0f;
	result.y = (-projectionpos.y * targetysize + targetysize) / 2.0f;
	return result;
}
