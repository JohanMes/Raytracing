#ifndef MODEL_H
#define MODEL_H

#include <vector>
using std::vector;
#include <string>
using std::string;
#include "float2.h"
#include "float3.h"
#include "resource.h"

struct Vertex {
	float3 pos;
	float2 tex;
	float3 nor;
	float3 tan;
	float3 bin;
};

struct Sphere {
	float3 center;
	float r;
};

class Model {
	friend class Models; // manages creation and destruction
	
	vector<Vertex> vertexbuffer;
	vector<int> indexbuffer;
	
	unsigned int numvertices;
	unsigned int numfaces;

	string fullpath;
	string filename;
	
	Sphere boundingsphere;
	
	Model(const char* filename);
	~Model();
	
	void ComputeBoundingSphere();

	public:
		void LoadFromOBJ(const char* filename);
		void Clear();
		
		Sphere& GetBoundingSphere();
		vector<Vertex>& GetVertexBuffer();
		vector<int>& GetIndexBuffer();
		unsigned int GetNumFaces();
		void ClearBoundingSphere();
};

#endif
