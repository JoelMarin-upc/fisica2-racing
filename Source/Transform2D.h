#pragma once

#include "p2Point.h"

class Transform2D
{
public:
	Transform2D() : position({ 0, 0 }), rotation(0) {}
	Transform2D(float x, float y, float rotation) : position({ x, y }), rotation(rotation) {}

	vec2<float> position;
	float rotation;
	
};
static const Transform2D ZeroTransform = Transform2D();