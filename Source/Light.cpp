#include "Light.h"

Light::Light(LightType type) {
	this->type = type;
	this->position = float3(0,0,0);
	this->direction = float3(1,0,0);
	this->color = float3(1,1,1);
}

Light::~Light() {
	
}

float3 Light::GetPosition() {
	return position;
}
float3 Light::GetDirection() {
	return direction;
}
float3 Light::GetColor() {
	return color;
}
LightType Light::GetType() {
	return type;
}
void Light::SetPosition(float3 position) {
	this->position = position;
}
void Light::SetDirection(float3 direction) {
	this->direction = direction;
	this->direction.Normalize();
}
void Light::SetColor(float3 color) {
	this->color = color;
}
