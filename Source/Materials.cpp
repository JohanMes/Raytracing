#include "Materials.h"

Materials::Materials() {
	
}
Materials::~Materials() {
	for(unsigned int i = 0;i < data.size();i++) {
		delete data[i];
	}
}

Material* Materials::Add(const char* name) {
	return Add(new Material(name));
}

Material* Materials::Add(Material* material) {
	data.push_back(material);
	return material;
}

Material* Materials::Get(const char* name) {
	for(int i = 0;i < (int)data.size();i++) {
		if(data[i]->GetName() == name) {
			return data[i];
		}
	}
	return NULL;
}
