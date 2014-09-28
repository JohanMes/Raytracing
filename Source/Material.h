#ifndef MATERIAL_H
#define MATERIAL_H

#include "float3.h"
#include "Textures.h"
#include <algorithm>
#include <string>
using std::string;

class Material {
	friend class Materials;
	
	string name;
	float3 color;
	float specular;
	float shininess;
	float refractiveindex;
	float opacity;
	Texture* diffusetexture;
	
	Material(string name);
	~Material();

	public:
		float3 GetDiffuseValue(float2 coordinate);
		float GetSpecularValue();
		float GetShininess();
		float GetRefractiveIndex();
		float GetOpacity();
		float3 GetColor(float2 coordinate,float3 lightdirection,float3 lightreflect,float3 normal,float3 eyedirection);
		string& GetName();
		void SetColor(float3 color);
		void SetSpecularValue(float specular);
		void SetShininess(float shininess);
		void SetRefractiveIndex(float refractiveindex);
		void SetTexture(const char* name,const char* filename);
		void SetOpacity(float opacity);
};

#endif
