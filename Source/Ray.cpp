#include "Ray.h"

Ray::Ray() {
	scene = NULL;
	eyepoint = float3(0,0,0);
	origin = float3(0,0,0);
	direction = float3(0,0,0);
	bounces = 0;
	originface.Clear();
	refractiveindex = 1; // air
	transmitrefractiveindex = 1; // material we will be passing through...
}
Ray::Ray(IntersectFace originface,float3 origin,float3 direction,float3 eyepoint,Scene* scene,int bounces,float refractiveindex) {
	SetOriginFace(originface);
	SetOrigin(origin);
	SetDirection(direction);
	SetEyePoint(eyepoint);
	SetScene(scene);
	SetBounces(bounces);
	SetRefractiveIndex(refractiveindex);
}
Ray::~Ray() {
	
}

float3 Ray::GetOrigin() {
	return origin;
}
float3 Ray::GetDirection() {
	return direction;
}
void Ray::ClearOriginFace() {
	originface.Clear();
}
void Ray::SetOriginFace(IntersectFace originface) {
	this->originface = originface;
}
void Ray::SetEyePoint(float3 eyepoint) {
	this->eyepoint = eyepoint;
}
void Ray::SetOrigin(float3 origin) {
	this->origin = origin;
}
void Ray::SetDirection(float3 direction) {
	this->direction = direction;
	this->direction.SetLength(1000.0f); // the higher the value, the more accurate intersects become somehow
}
void Ray::SetScene(Scene* scene) {
	this->scene = scene;
}
void Ray::SetBounces(int bounces) {
	this->bounces = bounces;
}
void Ray::SetRefractiveIndex(float refractiveindex) {
	this->refractiveindex = refractiveindex;
}
float3 Ray::GatherLightSourcesFrom(IntersectData* intersect) {
	float3 result = {0,0,0};
	
	// Cast a ray to each light source...
	// Do not recurse and create new rays. Just reuse functionality.
	vector<Light*>& lights = scene->GetLights();
	for(unsigned int i = 0;i < lights.size();i++) {
		Light* light = lights[i];
		
		// Determine if this light is lighting this surface or not
		float3 lighttestrayorigin = intersect->point;
		float3 lighttestraydirection = float3(0);
		switch(light->GetType()) {
			case ltDirection: {
				lighttestraydirection = light->GetDirection().SetLength(1000.0f); // do NOT test for really far away objects
				break;
			}
			case ltPoint: {
				lighttestraydirection = light->GetPosition() - intersect->point;
				break;
			}
			default: {
				continue;
			}
		}
		
		// Check the opacity of the objects between us and the light source
		IntersectData shadowintersect = scene->Intersect(&originface,lighttestrayorigin,lighttestraydirection);
		
		// Account for opacity of objects in front of the light source
		float objectopacity = 1.0f;
		if(shadowintersect.object) {
			objectopacity = 1.0f - shadowintersect.object->GetMaterial()->GetOpacity();
		}

		// Determine how power fades with distance
		float lightfalloff = 0.0f;
		switch(light->GetType()) {
			case ltPoint: {
				float lightdistance = lighttestraydirection.Length();
				lightfalloff = 1.0f/(lightdistance * lightdistance);	
				break;
			}
			case ltDirection: {
				lightfalloff = 1.0f;
				break;
			}
		}
		
		// Get normalized light dir
		float3 lightdirection = lighttestraydirection;
		lightdirection.Normalize();
		
		// Reflect lightdir on surface for specular factor
		float3 lightreflect;
		lightreflect.Reflect(intersect->normal,-lightdirection);
		
		// Get original eye point
		float3 eyedirection = eyepoint - intersect->point;
		eyedirection.Normalize();
		
		// Let the shader inside Material do the hard work
		Material* material = intersect->object->GetMaterial();
		result += objectopacity * lightfalloff * light->GetColor().Mul(material->GetColor(
			intersect->texcoord,
			lightdirection,
			lightreflect,
			intersect->normal,
			eyedirection));
	}
	return result;
}
float3 Ray::GatherReflectionFrom(IntersectData* intersect) {
	float3 reflectivecolor = {0,0,0};
	
	// What direction would a ray have to come from to bounce in our direction?
	float3 reflect, diffusenormal;

	// Sample a bunch of reflected vectors
	const int numsamples = 1;
	for(int i = 0;i < numsamples;i++) {
		
		// Randomize surface normal to simulate diffuse lighting
		// TODO: RandomRange is not thread safe
		diffusenormal = frontfacenormal;// + float3(RandomRange(-0.005f,0.005f));
		reflect.Reflect(diffusenormal,direction);
		
		// Add result to original, but reduce power using material properties
		Ray* reflectedray = new Ray(
			originface, // face we have hit just now
			intersect->point, // new origin, oint on the face we have hit
			reflect, // new direction
			eyepoint, // same eyepoint for Phong
			scene, // same scene graph
			bounces-1, // we have spent a bounce
			refractiveindex); // ray does not enter a different material
		reflectivecolor += reflectedray->Gather();
		delete reflectedray;
	}

	// Add to final
	return reflectivecolor/numsamples;
}
float3 Ray::GatherRefractionFrom(IntersectData* intersect) {
	float3 refractivecolor = {0,0,0};

	// Calculate refractive vector. Ignore fresnel equations
	// TODO: remember what material we came from
	float3 refract, diffusenormal;
	
	// Sample a bunch of refracted vectors
	const int numsamples = 1;
	for(int i = 0;i < numsamples;i++) {
		
		// Randomize surface normal to simulate diffuse lighting
		// TODO: RandomRange is not thread safe
		diffusenormal = frontfacenormal;// + float3(RandomRange(-0.005f,0.005f));	
		refract.Refract(
			diffusenormal, // normal
			direction, // direction
			refractiveindex, // n1
			transmitrefractiveindex); // n2

		// Add result to original, but reduce power using material properties
		Ray* refractedray = new Ray(
			originface, // face we have hit just now
			intersect->point, // new origin, oint on the face we have hit
			refract, // new direction
			eyepoint, // same eyepoint for Phong
			scene, // same scene graph
			bounces-1, // we have spent a bounce
			transmitrefractiveindex); // refraction enters a different material
		refractivecolor += refractedray->Gather();
		delete refractedray;
	}
	
	// Add to final
	return refractivecolor/numsamples;
}
float3 Ray::Gather() {
	
	// Gather light for current ray
	float3 result = {0,0,0};
	
	// Intersect the scene from the start point
	IntersectData intersect = scene->Intersect(&originface,origin,direction);
	
	// We have hit an object...
	if(intersect.object) {
		
		// Remember where we came from
		originface.object = intersect.object;
		originface.index = intersect.faceindex;
		
		// Add contribution of every light source that is in view from this point
		result += GatherLightSourcesFrom(&intersect);
		
		// If we are allowed to, reflect vector and repeat process
		if(bounces > 0) {
			
			// We need the normalized form for lighting calculations
			direction.Normalize();

			// Cache this for easier typing
			Material* material = intersect.object->GetMaterial();
			
			// Determine if we hit a back or front face
			if(direction.Dot(intersect.normal) < 0) { // air -> thing
				transmitrefractiveindex = material->GetRefractiveIndex();
				frontfacenormal = intersect.normal;
			} else { // thing -> air
				transmitrefractiveindex = 1.0;
				frontfacenormal = -intersect.normal;
			}
			
			// Calculate fresnel terms, account for TIR
			float reflectance = GetReflectance(
				direction, // input vector
				frontfacenormal, // surface normal pointing TOWARDS us
				refractiveindex, // current material
				transmitrefractiveindex); // material we could be transmitting to
			
			// Add color from reflected ray	
			if(material->GetOpacity() > 0 and reflectance > 0) {
				result += GatherReflectionFrom(&intersect) * reflectance * material->GetOpacity();
			}
			
			// Add color from refracted ray if TIR is not happening
			if(material->GetOpacity() < 1 and reflectance < 1) {
				result += GatherRefractionFrom(&intersect) * (1.0f - reflectance) * (1.0f - material->GetOpacity());
			}
		}
	}
	
	return result;
}
