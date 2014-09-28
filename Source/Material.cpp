#include "Material.h"

Material::Material(string name) {
	this->name = name;
	this->color = float3(1,1,1);
	this->specular = 0;
	this->shininess = 0;
	this->refractiveindex = 1; // air
	this->diffusetexture = NULL;
	this->opacity = 1.0f;
}
Material::~Material() {
	
}

float3 Material::GetColor(float2 coordinate,float3 lightdirection,float3 lightreflect,float3 normal,float3 eyedirection) {
	float3 result = {0,0,0}; // TODO: ambient term
	
	// Phong diffuse
	float phongdiffusefactor = std::max(0.0f,lightdirection.Dot(normal));
	result += phongdiffusefactor * GetDiffuseValue(coordinate);
	
	// Phong specular
	float phongspecularfactor = pow(std::max(0.0f,lightreflect.Dot(eyedirection)),shininess);
	result += float3(phongspecularfactor * GetSpecularValue());
	
	return result;
}

//			// Get normalized light dir
//			float3 lightdirection = lighttestraydirection;
//			lightdirection.Normalize();
//			
//			// Surface normal should already be normalized
//			float3 normal = intersect->normal;
//			
//			// Reflect lightdir on surface for specular factor
//			float3 lightreflect;
//			lightreflect.Reflect(normal,-lightdirection);
//			
//			// Get original eye point
//			float3 eyedirection = eyepoint - intersect->point;
//			eyedirection.Normalize();
//			
//			// Cache this for easier typing
//			Material* material = intersect->object->GetMaterial();
//			
//			// Sample the surface texture once
//			float3 materialdiffusevalue = material->GetDiffuseValue(intersect->texcoord);
//			float3 materialspecularvalue = material->GetSpecularValue(); // TODO: specular map
//			float3 lightcolor = thislight->GetColor() * lightpower;
//			
//			// Phong diffuse
//			float diffusefactor = std::max(0.0f,lightdirection.Dot(normal));
//			result += materialdiffusevalue.Mul(lightcolor) * diffusefactor;
//		
//			// Phong specular
//			float specularfactor = pow(lightreflect.Dot(eyedirection),material->GetShininess());
//			result += materialspecularvalue.Mul(lightcolor) * specularfactor;
			
			
			
float3 Material::GetDiffuseValue(float2 coordinate) {
	if(diffusetexture) {
		return diffusetexture->Sample(coordinate);
	} else {
		return color;
	}
}
float Material::GetSpecularValue() {
	return specular;
}
float Material::GetShininess() {
	return shininess;
}
float Material::GetRefractiveIndex() {
	return refractiveindex;
}
float Material::GetOpacity() {
	return opacity;
}
string& Material::GetName() {
	return name;
}

void Material::SetColor(float3 color) {
	this->color = color;
}
void Material::SetSpecularValue(float specular) {
	this->specular = specular;
}
void Material::SetShininess(float shininess) {
	this->shininess = shininess;
}
void Material::SetRefractiveIndex(float refractiveindex) {
	this->refractiveindex = refractiveindex;
}
void Material::SetTexture(const char* name,const char* filename) {
	this->diffusetexture = textures->Add(name,filename);
}
void Material::SetOpacity(float opacity) {
	this->opacity = opacity;
}
