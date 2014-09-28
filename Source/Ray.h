#ifndef RAY_H
#define RAY_H

#include <algorithm>
#include "float3.h"
#include "Scene.h"

class Ray { // not strictly a ray, because it has an origin
	Scene* scene; // scene graph
	float3 eyepoint; // keep eye point
	float3 origin; // start of ray
	float3 direction; // direction of ray. not normalized
	int bounces; // number of bounces left
	IntersectFace originface; // face we bounced off of
	float refractiveindex; // refractive index of current material (n1)
	float transmitrefractiveindex; // refractive index of next material (n2)
	float3 frontfacenormal; // intersection normal that faces the incoming ray
	float3 GatherLightSourcesFrom(IntersectData* intersect);
	float3 GatherReflectionFrom(IntersectData* intersect);
	float3 GatherRefractionFrom(IntersectData* intersect);
	public:
		Ray();
		Ray(IntersectFace originface,float3 origin,float3 direction,float3 eyepoint,Scene* scene,int bounces,float refractiveindex);
		~Ray();
		
		float3 GetOrigin();
		float3 GetDirection();
		void ClearOriginFace();
		void SetOriginFace(IntersectFace originface);
		void SetEyePoint(float3 eyepoint);
		void SetOrigin(float3 origin);
		void SetDirection(float3 direction);
		void SetScene(Scene* scene);
		void SetBounces(int bounces);
		void SetRefractiveIndex(float refractiveindex);
		float3 Gather();
};

#endif
