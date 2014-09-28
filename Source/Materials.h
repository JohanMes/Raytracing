#ifndef MATERIALS_H
#define MATERIALS_H

#include "float3.h"
#include "Material.h"
#include <vector>
using std::vector;

class Materials {
	vector<Material*> data;
	public:
		Materials();
		~Materials();
		
		Material* Add(const char* name);
		Material* Add(Material* material);
		Material* Get(const char* name);
};

extern Materials* materials;

#endif
