#ifndef FLOAT2_H
#define FLOAT2_H

#include <d3dx9math.h>

class float2 : public D3DXVECTOR2 {
	public:
		float2();
		float2(float x,float y);
		float2(const D3DXVECTOR2& rvalue);
		float2(const POINT& rvalue);
		
		float Length();
		float Dot(const float2& rvalue);
		float2 Abs();
		float2 SetLength(float value);
		float2 Normalize();
};

#endif
