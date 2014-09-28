#ifndef OBJECT_H
#define OBJECT_H

#include "Model.h"
#include "Material.h"
#include "Models.h"
#include "Materials.h"
#include "float4x4.h"

class Object;

class IntersectFace {
	public:
		IntersectFace() {
			Clear();			
		}
		void Clear() {
			object = NULL;
			index = -1;				
		}
		Object* object;
		int index;
};

class IntersectData {
	public:
		IntersectData() {
			point = {0};
			normal = {0};
			
			object = NULL;
			faceindex = -1;
			t = -1.0f;
		}
		float3 point;
		float3 normal;
		float2 texcoord;
		Object* object;
		int faceindex;
		float t; // [0..1]
};

class Object {
	Model* model;
	Material* material;
	float3 rotate;
	float3 scale;
	float3 translate;
	float4x4 matRotate;
	float4x4 matScale;
	float4x4 matTranslate;
	float4x4 matWorld;
	float4x4 matWorldInverse;
	
	public:
		Object();
		~Object();
		
		void Clear();
		Model* GetModel();
		Material* GetMaterial();
		float3 GetRotation();
		float3 GetScaling();
		float3 GetTranslation();
		void SetModel(Model* model);
		void SetModel(const char* filename);
		void SetMaterial(Material* material);
		void SetMaterial(const char* name);
		void SetRotation(float3 rotate);
		void SetScaling(float3 scale);
		void SetTranslation(float3 translate);
		void Update();
		IntersectData IntersectSphere(float3 worldpos,float3 worlddir); // TODO: allow other bounding shapes
		IntersectData IntersectModel(float3 worldpos,float3 worlddir,int ignorefaceindex);
};

#endif
