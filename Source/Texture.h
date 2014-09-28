#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
using std::vector;
#include <string>
using std::string;
#include "float2.h"
#include "float3.h"
#include "resource.h"

class Texture {
	string name;
	string filename;
	string fullpath;
	unsigned int width;
	unsigned int height;
	vector<vector<float3>> data;
	public:
		Texture(const char* name,const char* filename);
		~Texture();
		
		string& GetName();
		void Load(const char* filename);
		float3 Sample(float2 coordinate);
};

#endif
