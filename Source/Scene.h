#ifndef SCENE_H
#define SCENE_H

#include <vector>
using std::vector;
#include "Object.h"
#include "Light.h"

class Scene {
	vector<Light*> lights;
	vector<Object*> objects;
	public:
		Scene();
		~Scene();
		
		vector<Light*>& GetLights();
		void AddLight(Light* light);
		void AddObject(Object* object);
		IntersectData Intersect(IntersectFace* face,float3 worldpos,float3 worlddir);
		void Clear();
};

extern Scene* scene;

#endif
