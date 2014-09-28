#include "Model.h"

Model::Model(const char* filename) {
	LoadFromOBJ(filename);
}

Model::~Model() {
}

void Model::Clear() {
	vertexbuffer.clear();
	indexbuffer.clear();
	numvertices = 0;
	numfaces = 0;
	filename.clear();
	fullpath.clear();
	ClearBoundingSphere();
}

Sphere& Model::GetBoundingSphere() {
	return boundingsphere;
}

vector<Vertex>& Model::GetVertexBuffer() {
	return vertexbuffer;
}
vector<int>& Model::GetIndexBuffer() {
	return indexbuffer;
}

unsigned int Model::GetNumFaces() {
	return numfaces;
}

void Model::ClearBoundingSphere() {
	boundingsphere.r = 0;
	boundingsphere.center = float3(0);
}

void Model::ComputeBoundingSphere() {
	boundingsphere.r = 0.0f;

	float len;
	float3 pair1,pair2,lenvec;
	int testindex = 0,farthestindex = 0;

	// Determine which vertex is farthest apart from index 'testindex'
	while(1) {
		for(unsigned int i = 0;i < numvertices;i++) {
			lenvec = vertexbuffer[i].pos - vertexbuffer[testindex].pos;
			len = lenvec.Length();

			if(len > boundingsphere.r) {
				boundingsphere.r = len;
				pair1 = vertexbuffer[i].pos;
				pair2 = vertexbuffer[testindex].pos;
				farthestindex = i;
			}
		}

		// Continue the search with the new farthest vertex
		if(farthestindex == testindex) { // no progress
			break;
		} else {
			testindex = farthestindex; // test against farthest vertex
		}
	}

	// Radius equals half the maximum distance, and apply scaling
	boundingsphere.r /= 2.0f;

	// Center is right in between the pair of vector2
	boundingsphere.center = 0.5f*(pair1 + pair2);
}

void Model::LoadFromOBJ(const char* filename) {

	// Remove old stuff
	Clear();

	// Reset OBJ loading counters
	unsigned int numnormals = 0;
	unsigned int numcoords = 0;

	// Obtain full path
	char* buffer = new char[MAX_PATH];
	GetFullPath(filename,"Data\\Models",buffer);
	this->fullpath = buffer;

	// Set name to loaded item
	ExtractFileName(this->fullpath.c_str(),buffer);
	this->filename = buffer;
	delete[] buffer;

	// Try to open filename, return dummy on failure
	FILE* modelfile = fopen(fullpath.c_str(),"rb");
	if(modelfile == NULL) {
		//console->Write("Error opening model %s\r\n",fullpath);
		return;
	}

	// Store each line here
	char currentline[300];
	char word1[100];

	while(fgets(currentline,sizeof(currentline),modelfile)) {
		if(sscanf(currentline,"%[^ #\r\n]",word1) == 1) {
			if(!strcmp(word1,"v")) {
				numvertices++;
			} else if(!strcmp(word1,"vn")) {
				numnormals++;
			} else if(!strcmp(word1,"vt")) {
				numcoords++;
			} else if(!strcmp(word1,"f")) {
				int spacecount = CountChar(TrimRight(currentline),' ');
				if(spacecount == 3) { // Add a triangle
					numfaces++;
				} else if(spacecount == 4) { // Add a quad (two triangles)
					numfaces += 2;
				}
			}
		}
	}

	fseek(modelfile,0,SEEK_SET);

	// Ruimte maken voor elke unieke pos/normal/texcoord
	float3* positions = new float3[numvertices];
	float3* normals = new float3[numnormals];
	float2* coords = new float2[numcoords];

	// Three for each separate triangle
	vertexbuffer.resize(3*numfaces);
	
	// An index for each triangle corner
	indexbuffer.resize(3*numfaces);
	for(unsigned int i = 0;i < 3*numfaces;i++) {
		indexbuffer[i] = i;
	}

	int curvertex = 0;
	int curnormal = 0;
	int curcoord = 0;
	int curface = 0;

	int p1,p2,p3,p4;
	int n1,n2,n3,n4;
	int c1,c2,c3,c4;

	// En nu alle regels die ertoe doen lezen en in de arrays zetten
	while(fgets(currentline,sizeof(currentline),modelfile)) {
		if(sscanf(currentline,"%[^ #\n]",word1) == 1) {
			if(!strcmp(word1,"v")) { // Vertex
				sscanf(currentline,"v %f %f %f",
					&positions[curvertex].x,
					&positions[curvertex].y,
					&positions[curvertex].z);
				curvertex++;
			} else if(!strcmp(word1,"vn")) { // Normal
				sscanf(currentline,"vn %f %f %f",
					&normals[curnormal].x,
					&normals[curnormal].y,
					&normals[curnormal].z);
				curnormal++;
			} else if(!strcmp(word1,"vt")) { // Texture coordinate
				sscanf(currentline,"vt %f %f",
					&coords[curcoord].x,
					&coords[curcoord].y);
				coords[curcoord].y = 1.0f - coords[curcoord].y; // why?
				curcoord++;
			} else if(!strcmp(word1,"f")) { // face, can be triangle or quad

				int spacecount = CountChar(TrimRight(currentline),' ');

				// Triangle...
				if(spacecount == 3) {

					if(!numnormals && !numcoords) {

						// f pos pos pos
						if(sscanf(currentline,"f %d %d %d",&p1,&p2,&p3) == 3) {

							// Vul geleverde XYZ in
							vertexbuffer[curface+0].pos = positions[p1-1];
							vertexbuffer[curface+1].pos = positions[p2-1];
							vertexbuffer[curface+2].pos = positions[p3-1];

							// Vul random normals in
							vertexbuffer[curface+0].nor = float3(0.0f,0.0f,0.0f);
							vertexbuffer[curface+1].nor = float3(0.0f,0.0f,0.0f);
							vertexbuffer[curface+2].nor = float3(0.0f,0.0f,0.0f);

							// Vul random texcoords in
							vertexbuffer[curface+0].tex = float2(0.0f,0.0f);
							vertexbuffer[curface+1].tex = float2(0.0f,0.0f);
							vertexbuffer[curface+2].tex = float2(0.0f,0.0f);
						}
					} else if(numnormals && !numcoords) {

						// f pos//nor pos//nor pos//nor
						if(sscanf(currentline,"f%d//%d %d//%d %d//%d",&p1,&n1,&p2,&n2,&p3,&n3) == 6) {

							// Vul geleverde XYZ in
							vertexbuffer[curface+0].pos = positions[p1-1];
							vertexbuffer[curface+1].pos = positions[p2-1];
							vertexbuffer[curface+2].pos = positions[p3-1];

							// Vul geleverde normals in
							vertexbuffer[curface+0].nor = normals[n1-1];
							vertexbuffer[curface+1].nor = normals[n2-1];
							vertexbuffer[curface+2].nor = normals[n3-1];

							// Vul random texcoords in
							vertexbuffer[curface+0].tex = float2(0.0f,0.0f);
							vertexbuffer[curface+1].tex = float2(0.0f,0.0f);
							vertexbuffer[curface+2].tex = float2(0.0f,0.0f);
						}
					} else if(numnormals && numcoords) {

						// f pos/nor/tex pos/nor/tex pos/nor/tex
						if(sscanf(currentline,"f %d/%d/%d %d/%d/%d %d/%d/%d",&p1,&c1,&n1,&p2,&c2,&n2,&p3,&c3,&n3) == 9) {

							// Vul geleverde XYZ in
							vertexbuffer[curface+0].pos = positions[p1-1];
							vertexbuffer[curface+1].pos = positions[p2-1];
							vertexbuffer[curface+2].pos = positions[p3-1];

							// Vul geleverde normals in
							vertexbuffer[curface+0].nor = normals[n1-1];
							vertexbuffer[curface+1].nor = normals[n2-1];
							vertexbuffer[curface+2].nor = normals[n3-1];

							// Vul geleverde texcoords in
							vertexbuffer[curface+0].tex = coords[c1-1];
							vertexbuffer[curface+1].tex = coords[c2-1];
							vertexbuffer[curface+2].tex = coords[c3-1];
						}
					}

					curface += 3;

				// Accepteer ook vierkanten...
				} else if(spacecount == 4) {

					// Add triangles for 1,2,4 and 3,4,2

					if(!numnormals && !numcoords) {

						// f pos pos pos pos
						if(sscanf(currentline,"f %d %d %d %d",&p1,&p2,&p3,&p4) == 4) {

							// Vul geleverde XYZ in
							vertexbuffer[curface+0].pos = positions[p1-1];
							vertexbuffer[curface+1].pos = positions[p2-1];
							vertexbuffer[curface+2].pos = positions[p4-1];

							// Vul random normals in
							vertexbuffer[curface+0].nor = float3(0.0f,0.0f,0.0f);
							vertexbuffer[curface+1].nor = float3(0.0f,0.0f,0.0f);
							vertexbuffer[curface+2].nor = float3(0.0f,0.0f,0.0f);

							// Vul random texcoords in
							vertexbuffer[curface+0].tex = float2(0.0f,0.0f);
							vertexbuffer[curface+1].tex = float2(0.0f,0.0f);
							vertexbuffer[curface+2].tex = float2(0.0f,0.0f);

							// Vul geleverde XYZ in
							vertexbuffer[curface+3].pos = positions[p3-1];
							vertexbuffer[curface+4].pos = positions[p4-1];
							vertexbuffer[curface+5].pos = positions[p2-1];

							// Vul random normals in
							vertexbuffer[curface+3].nor = float3(0.0f,0.0f,0.0f);
							vertexbuffer[curface+4].nor = float3(0.0f,0.0f,0.0f);
							vertexbuffer[curface+5].nor = float3(0.0f,0.0f,0.0f);

							// Vul random texcoords in
							vertexbuffer[curface+3].tex = float2(0.0f,0.0f);
							vertexbuffer[curface+4].tex = float2(0.0f,0.0f);
							vertexbuffer[curface+5].tex = float2(0.0f,0.0f);
						}
					} else if(numnormals && !numcoords) {

						// f pos//nor pos//nor pos//nor pos//nor
						if(sscanf(currentline,"f%d//%d %d//%d %d//%d %d//%d",&p1,&n1,&p2,&n2,&p3,&n3,&p4,&n4) == 8) {

							// Vul geleverde XYZ in
							vertexbuffer[curface+0].pos = positions[p1-1];
							vertexbuffer[curface+1].pos = positions[p2-1];
							vertexbuffer[curface+2].pos = positions[p4-1];

							// Vul geleverde normals in
							vertexbuffer[curface+0].nor = normals[n1-1];
							vertexbuffer[curface+1].nor = normals[n2-1];
							vertexbuffer[curface+2].nor = normals[n4-1];

							// Vul random texcoords in
							vertexbuffer[curface+0].tex = float2(0.0f,0.0f);
							vertexbuffer[curface+1].tex = float2(0.0f,0.0f);
							vertexbuffer[curface+2].tex = float2(0.0f,0.0f);

							// Vul geleverde XYZ in
							vertexbuffer[curface+3].pos = positions[p3-1];
							vertexbuffer[curface+4].pos = positions[p4-1];
							vertexbuffer[curface+5].pos = positions[p2-1];

							// Vul geleverde normals in
							vertexbuffer[curface+3].nor = normals[n3-1];
							vertexbuffer[curface+4].nor = normals[n4-1];
							vertexbuffer[curface+5].nor = normals[n2-1];

							// Vul random texcoords in
							vertexbuffer[curface+3].tex = float2(0.0f,0.0f);
							vertexbuffer[curface+4].tex = float2(0.0f,0.0f);
							vertexbuffer[curface+5].tex = float2(0.0f,0.0f);
						}
					} else if(numnormals && numcoords) {

						// f pos/tex/nor pos/tex/nor pos/tex/nor pos/tex/nor
						if(sscanf(currentline,"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",&p1,&c1,&n1,&p2,&c2,&n2,&p3,&c3,&n3,&p4,&c4,&n4) == 12) {

							// Vul geleverde XYZ in
							vertexbuffer[curface+0].pos = positions[p1-1];
							vertexbuffer[curface+1].pos = positions[p2-1];
							vertexbuffer[curface+2].pos = positions[p4-1];

							// Vul geleverde normals in
							vertexbuffer[curface+0].nor = normals[n1-1];
							vertexbuffer[curface+1].nor = normals[n2-1];
							vertexbuffer[curface+2].nor = normals[n4-1];

							// Vul geleverde texcoords in
							vertexbuffer[curface+0].tex = coords[c1-1];
							vertexbuffer[curface+1].tex = coords[c2-1];
							vertexbuffer[curface+2].tex = coords[c4-1];

							// Vul geleverde XYZ in
							vertexbuffer[curface+3].pos = positions[p3-1];
							vertexbuffer[curface+4].pos = positions[p4-1];
							vertexbuffer[curface+5].pos = positions[p2-1];

							// Vul geleverde normals in
							vertexbuffer[curface+3].nor = normals[n3-1];
							vertexbuffer[curface+4].nor = normals[n4-1];
							vertexbuffer[curface+5].nor = normals[n2-1];

							// Vul geleverde texcoords in
							vertexbuffer[curface+3].tex = coords[c3-1];
							vertexbuffer[curface+4].tex = coords[c4-1];
							vertexbuffer[curface+5].tex = coords[c2-1];
						}
					}

					curface += 6;
				}
			}
		}
	}
	fclose(modelfile);

	// En tijdelijke rommel opruimen
	delete[] positions;
	delete[] normals;
	delete[] coords;

	// Neem aan dat we alles m.b.v. triangles opbouwen (geen optimize)
	numvertices = 3*numfaces;
	
	ComputeBoundingSphere();
}
