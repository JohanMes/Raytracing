#include "Textures.h"

Textures::Textures() {
	
}

Textures::~Textures() {
	for(unsigned int i = 0;i < data.size();i++) {
		delete data[i];
	}	
}

Texture* Textures::Add(const char* name,const char* filename) {
	return Add(new Texture(name,filename));
}

Texture* Textures::Add(Texture* texture) {
	data.push_back(texture);
	return texture;	
}

Texture* Textures::Get(const char* name) {
	for(int i = 0;i < (int)data.size();i++) {
		if(data[i]->GetName() == name) {
			return data[i];
		}
	}
	return NULL;
}
