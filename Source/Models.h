#ifndef MODELS_H
#define MODELS_H

#include <vector>
using std::vector;
#include "Model.h"

class Models {
	vector<Model*> data;
	public:
		Models();
		~Models();
		
		Model* Add(Model* model);
		Model* Add(const char* filename);
};

extern Models* models;

#endif
