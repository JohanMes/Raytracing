#ifndef FLOAT4_H
#define FLOAT4_H

#include <d3dx9math.h>

class float4 : public D3DXVECTOR4 {
	public:
		float4();
		float4(float x,float y,float z,float w);
		float4(const D3DXVECTOR4& rvalue);

		float Length();
		float Dot(const float4& rvalue);
		float4 SetLength(float value);
		float4 Normalize();
};

#endif
