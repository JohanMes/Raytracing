#include "Models.h"

Models::Models() {
}

Models::~Models() {
	for(unsigned int i = 0;i < data.size();i++) {
		delete data[i];
	}
}

Model* Models::Add(Model* model) {
	data.push_back(model);
	return model;
}

Model* Models::Add(const char* filename) {
	return Add(new Model(filename));
}
