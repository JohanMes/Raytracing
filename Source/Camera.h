#ifndef CAMERA_H
#define CAMERA_H

#include <d3dx9.h>
#include "float3.h"
#include "float4x4.h"
#include "resource.h"

class Camera {
	float3 pos;
	float3 lookat;
	float3 up;
	float fov;
	float ratio;
	float minviewdistance;
	float maxviewdistance;
	float4x4 matView;
	float4x4 matViewInverse;
	float4x4 matProj;
	float4x4 matProjInverse;
	public:
		Camera();
		~Camera();
		
		void Update();
		float3 GetPos();
		float GetMaxViewDistance();
		float4x4 GetViewMatrix();
		float4x4 GetProjMatrix();
		float4x4 GetViewInverseMatrix();
		float4x4 GetProjInverseMatrix();
		void SetPos(float3 pos);
		void SetLookAt(float3 lookat);
		void SetFOV(float fov);
		void SetRatio(float ratio);
};

extern Camera* camera;

#endif
