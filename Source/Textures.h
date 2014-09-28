#ifndef TEXTURES_H
#define TEXTURES_H

#include "Texture.h"
#include <vector>
using std::vector;

class Textures {
	vector<Texture*> data;
	public:
		Textures();
		~Textures();
		
		Texture* Add(const char* name,const char* filename);
		Texture* Add(Texture* texture);
		Texture* Get(const char* name);
};

extern Textures* textures;

#endif
