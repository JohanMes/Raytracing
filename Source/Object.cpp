#include "Object.h"

Object::Object() {
	model = NULL;
	material = NULL;
	rotate = float3(0,0,0);
	scale = float3(1,1,1);
	translate = float3(0,0,0);
	Update();
}

Object::~Object() {
}

void Object::Clear() {
	model = NULL; // we do not own the model
	material = NULL; // same
}
Model* Object::GetModel() {
	return model;
}
Material* Object::GetMaterial() {
	return material;
}
float3 Object::GetRotation() {
	return rotate;
}
float3 Object::GetScaling() {
	return scale;
}
float3 Object::GetTranslation() {
	return translate;
}
void Object::SetModel(Model* model) {
	this->model = model;
}
void Object::SetModel(const char* filename) {
	this->model = models->Add(filename);
}
void Object::SetMaterial(Material* material) {
	this->material = material;
}
void Object::SetMaterial(const char* name) {
	this->material = materials->Get(name);
}
void Object::SetRotation(float3 rotate) {
	this->rotate = rotate;
	Update();
}
void Object::SetScaling(float3 scale) {
	this->scale = scale;
	Update();
}
void Object::SetTranslation(float3 translate) {
	this->translate = translate;
	Update();
}
void Object::Update() {
	matRotate.EulerRotationDeg(rotate);
	matScale.Scaling(scale);
	matTranslate.Translation(translate);
	matWorld = matRotate*matScale*matTranslate;
	matWorldInverse = matWorld.Inverse();
}

IntersectData Object::IntersectSphere(float3 worldpos,float3 worlddir) { // TODO: allow other bounding shapes
	IntersectData result;
	
	if(!model) {
		//console->Write("Cannot intersect object %s without mesh\r\n",name);
		return result; // object without a mesh assigned to it
	}
	
	Sphere& boundingsphere = model->GetBoundingSphere();

	// Ray dir in object space
	float3 r1 = worlddir.Transform(matWorldInverse);

	// Ray pos in object space
	float3 r0 = worldpos.Transform(matWorldInverse);

	// Sphere: (p - p0) dot (p - p0) = r*r
	// Ray: r0 + t * r1
	// Solve for t: (r0 + t * r1 - p0) dot (r0 + t * r1 - p0) - r*r = 0

	float3 p0 = boundingsphere.center;
	float3 r0minusp0 = r0 - p0;

	// a = r1 dot r1
	float a = r1.Dot(r1);

	// b = 2 * r1 dot (r0 - p0)
	float b = 2.0f * r1.Dot(r0minusp0);

	// c = (r0 - p0) dot (r0 - p0) - r^2
	float c = r0minusp0.Dot(r0minusp0) - boundingsphere.r*boundingsphere.r;

	// ABC
	float d = b*b - 4.0f * a * c;

	// d >= 0 -> pass
	if(d >= 0.0f) {
		result.object = this;
		result.t = (-b - sqrt(d))/(2.0f * a); // return lowest time
	}

	return result;
}

IntersectData Object::IntersectModel(float3 worldpos,float3 worlddir,int ignorefaceindex) {
	IntersectData result;
	
	if(!model) {
		return result; // object without a mesh assigned to it
	}

	// Ray dir
	float3 r1 = worlddir.Transform(matWorldInverse);
	
	// Ray pos
	float3 r0 = worldpos.Transform(matWorldInverse);
	
	// Return the closest intersect available
	float minimumtime = 1.0f;

	// Walk each triangle constructed the index buffer
	vector<Vertex>& vertexbuffer = model->GetVertexBuffer();
	vector<int>& indexbuffer = model->GetIndexBuffer();
	for(unsigned int j = 0;j < 3 * model->GetNumFaces();j+=3) {
		
		// The user can choose to avoid certain triangles to decrease aliasing
		if((int)j == ignorefaceindex) {
			continue;
		}

		// Ga alle triangles af...
		float3 p0 = vertexbuffer[indexbuffer[j+0]].pos;
		float3 p1 = vertexbuffer[indexbuffer[j+1]].pos;
		float3 p2 = vertexbuffer[indexbuffer[j+2]].pos;
		
		// Triangle: (1 - u - v) * p0 + u * p1 + v * p2
		// Ray: r0 + t * r1
		// [-r1,p1 - p0,p2 - p0][t,u,v]^T = r0 - p0]
		// inverteer, cramer:

		// Edge vectors
		float3 E1 = p1 - p0;
		float3 E2 = p2 - p0;

		// Recurring factors
		float3 T = r0 - p0;
		float3 P = r1.Cross(E2);
		float3 Q = T.Cross(E1);

		// Inverse van determinant, handig voor berekenen inverse
		float recipdet = 1.0f/(P.Dot(E1));

		// Bereken zo weinig mogelijk
		float u = recipdet * (P.Dot(T));
		float v = recipdet * (Q.Dot(r1));
		if(u >= 0.0f and v >= 0.0f and u + v <= 1.0f) {

			float t = recipdet * (Q.Dot(E2));
			if(t > 1e-6f and t < minimumtime) {
				result.object = this;
				
				// Interpolate to get normal on intersection point
				result.normal = (1 - u - v) * vertexbuffer[indexbuffer[j+0]].nor +
				                          u * vertexbuffer[indexbuffer[j+1]].nor +
				                          v * vertexbuffer[indexbuffer[j+2]].nor;

				// Interpolate to get texcoord on intersection point
				result.texcoord = (1 - u - v) * vertexbuffer[indexbuffer[j+0]].tex +
				                            u * vertexbuffer[indexbuffer[j+1]].tex +
				                            v * vertexbuffer[indexbuffer[j+2]].tex;
				
				// Remember what we hit for future intersects
				result.faceindex = j;
				
				// Now, only accept intersects before this point
				minimumtime = t;
			}
		}
	}

	// We hebben een triangle geraakt!
	if(result.object) {
		result.t = minimumtime;
		
		// Retourneer het raakpunt in world space
		result.point = r0 + result.t * r1;
		result.point = result.point.Transform(matWorld);
		
		// Zelfde geldt voor de normaal
		result.normal.Normalize();
		result.normal = result.normal.TransformNormal(matWorld);
	}

	return result;
}
