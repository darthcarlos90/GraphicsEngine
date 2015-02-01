#pragma once
#include "../../nclgl/Vector3.h"

class CollisionSphere {
public:
	CollisionSphere(const Vector3 &p, float r){
		m_pos = p;
		m_radius = r;
	}
	CollisionSphere(){};
	~CollisionSphere(){};

	Vector3 m_pos;
	float m_radius;

};