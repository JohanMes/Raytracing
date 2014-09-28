#include "Scene.h"

Scene::Scene() {
}

Scene::~Scene() {
	// Objects are not owned
}

void Scene::Clear() {
	objects.clear();
}

vector<Light*>& Scene::GetLights() {
	return lights;
}

void Scene::AddLight(Light* light) {
	lights.push_back(light);
}

void Scene::AddObject(Object* object) {
	objects.push_back(object);
}

IntersectData Scene::Intersect(IntersectFace* face,float3 worldpos,float3 worlddir) {
	IntersectData result;

	// For each object in scene, do a fast sphere test
	std::vector<Object*> boundingpass;

	// Bekijk welke objecten er achter het punt zitten, door ray door bounding sphere heen te trekken
	for(unsigned int i = 0;i < objects.size();i++) {
		Object* thisobject = objects[i];
		IntersectData intersect = thisobject->IntersectSphere(worldpos,worlddir);
		if(intersect.object) {
			boundingpass.push_back(thisobject);
		}
	}
	
	// Return the closest intersect available
	float minimumtime = 1.0f;

	// Now, for each object in spherepass, do a slow triangle test
	for(unsigned int i = 0;i < boundingpass.size();i++) {
		Object* thisobject = boundingpass[i];
		IntersectData intersect;
		
		// Tell the intersect to ignore the face we came from
		if(face->object == thisobject) {
			intersect = thisobject->IntersectModel(worldpos,worlddir,face->index);
		} else {
			intersect = thisobject->IntersectModel(worldpos,worlddir,-1);
		}
		
		// Did we hit something?
		if(intersect.object) {
			if(intersect.t > 1e-6f && intersect.t < minimumtime) {
				result = intersect;
				
				// Now, only accept intersects before this point
				minimumtime = intersect.t;
			}
		}
	}
	return result;
}
