#ifndef FLOAT3_H
#define FLOAT3_H

#include <d3dx9math.h>

class float3 : public D3DXVECTOR3 {
	public:
		float3();
		float3(float x);
		float3(float x,float y,float z);
		float3(const D3DXVECTOR3& rvalue);

		float Length() const;
		float Dot(const float3& rvalue) const;
		float3 Min(const float3& rvalue) const;
		float3 Max(const float3& rvalue) const;
		float3 Pow(const float3& value) const;
		float3 SetLength(float value);
		float3 Div(const float3& rvalue) const;
		float3 Mul(const float3& rvalue) const;
		float3 Cross(const float3& rvalue) const;
		float3 Normalize();
		float3 Transform(const D3DXMATRIX& matrix) const;
		float3 TransformNormal(const D3DXMATRIX& matrix) const;// x,y,z,0
		float3 TransformCoord(const D3DXMATRIX& matrix) const;// x,y,z,1
		float3 Reflect(float3 normal,float3 input);
		float3 Refract(float3 normal,float3 input,float n1,float n2);
};

#endif
