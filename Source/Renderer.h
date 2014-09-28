#ifndef RENDERER_H
#define RENDERER_H

#include "Camera.h"
#include "Scene.h"
#include "Ray.h"
#include "Clock.h"
#include <vector>
using std::vector;
#include <thread>
using std::thread;
#include <atomic>
using std::atomic;

class Renderer {
	Camera* camera;
	Scene* scene;
	HWND hwnd;
	int targetxsize;
	int targetysize;
	vector<vector<float3>> target;
	vector<float2> sampleoffsets;
	vector<RECT> workblocks;
	atomic<int> workblockindex;
	int blocksize;
	int numthreads;
	int bounces;
	int samples;
	
	// Utils
	float3 GetPixelWorldRay(float2 pixelpos);
	float2 PixelsToProjection(float2 pixelpos);
	float2 ProjectionToPixels(float2 projectionpos);
	void DrawProgressShape(RECT* boundingrect,HDC hdc);
	void DrawResultColor(RECT* boundingrect,HDC hdc);
	bool GetNextWorkBlock(RECT* block);
	public:
		Renderer();
		~Renderer();

		int GetNumThreads();
		int GetBlockSize();
		int GetResolutionX();
		int GetResolutionY();
		int GetBounces();
		int GetSamples();
		float3* GetPixelValue(int x,int y);
		void SetScene(Scene* scene);
		void SetCamera(Camera* camera);
		void SetResolution(int x,int y);
		void SetTargetWindow(HWND hwnd);
  		void SetNumThreads(int numthreads);
  		void SetBlockSize(int blocksize);
  		void SetBounces(int bounces);
  		void SetSamples(int samples);
		void Render();
		void Repaint(HWND hwnd);
		
		static void ThreadWork(Renderer* renderer);
};

extern Renderer* renderer;

#endif
