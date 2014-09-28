#include "Camera.h"

Camera::Camera() {
	pos = float3(0,0,0);
	lookat = float3(1,0,0);
	up = float3(0,1,0);
	fov = DegToRad(60);
	ratio = 1.333f; // 4:3
	minviewdistance = 0.1f;
	maxviewdistance = 1000.0f;
	Update();
}

Camera::~Camera() {
	
}

float3 Camera::GetPos() {
	return pos;
}
float Camera::GetMaxViewDistance() {
	return maxviewdistance;
}
float4x4 Camera::GetViewMatrix() {
	return matView;
}
float4x4 Camera::GetProjMatrix() {
	return matProj;
}
float4x4 Camera::GetViewInverseMatrix() {
	return matViewInverse;
}
float4x4 Camera::GetProjInverseMatrix() {
	return matProjInverse;
}

void Camera::SetPos(float3 pos) {
	this->pos = pos;
	Update();
}
void Camera::SetLookAt(float3 lookat) {
	this->lookat = lookat;
	Update();
}
void Camera::SetFOV(float fov) {
	this->fov = fov;
	Update();
}
void Camera::SetRatio(float ratio) {
	this->ratio = ratio;
	Update();
}
void Camera::Update() {
	// Calculate matrices. Use D3D native values
	D3DXMATRIX matViewDummy,matProjDummy;
	D3DXMatrixLookAtLH(&matViewDummy,&pos,&lookat,&up);
	D3DXMatrixPerspectiveFovLH(&matProjDummy,fov,ratio,minviewdistance,maxviewdistance);

	matView = matViewDummy;
	matProj = matProjDummy;
	matViewInverse = matView.Inverse();
	matProjInverse = matProj.Inverse();
}
