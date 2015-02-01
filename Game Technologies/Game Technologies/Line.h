#pragma once
#include "../../nclgl/Vector3.h"

class Line{
public:
	Line(const Vector3& point0, const Vector3& point1){
		this->point0 = point0;
		this->point1 = point1;
	}

	Vector3 point0;
	Vector3 point1;
};