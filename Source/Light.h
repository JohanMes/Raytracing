#ifndef LIGHT_H
#define LIGHT_H

#include "float3.h"

enum LightType {
	ltNone,
	ltDirection,
	ltPoint,
	ltSpot,
};

class Light {
	float3 position;
	float3 direction;
	float3 color;
	LightType type;
	public:
		Light(LightType type);
		~Light();
		
		float3 GetPosition();
		float3 GetDirection();
		float3 GetColor();
		LightType GetType();
		void SetPosition(float3 position);
		void SetDirection(float3 direction);
		void SetColor(float3 color);
};

#endif
