#include "Plane.h"

Plane::Plane(void){
	normal = Vector3(0,1,0);
	float length = Vector3::Dot(normal, normal);
	normal = normal / length;
	distance = 0.0f;
	distance = distance / length; 
}


Plane::Plane(const Vector3 &normal, float distance, bool normalise) {
	if(normalise) {
		float length = Vector3::Dot(normal,normal);

		this->normal   = normal		/ length;
		this->distance = distance	/ length;
	}
	else{
		this->normal = normal;
		this->distance = distance;
	}
}

Plane::Plane (Vector3 a, Vector3 b, Vector3 c, bool normalise){
	normal = Vector3::Cross(b - a, c - a);
	//Testing
	//normal = Vector3(0,1,0);
	if(normalise){
		float length = Vector3::Dot(normal, normal);
		normal = normal / length;
		distance = Vector3::Dot(normal, a);
		distance = distance / length;
	} 
}



bool Plane::SphereInPlane(const Vector3 &position, float radius) const {
	if(Vector3::Dot(position,normal)+distance <= -radius) {
		return false;
	}

	return true;	
}

bool Plane::PointInPlane(const Vector3 &position) const {
	float test = Vector3::Dot(position,normal);
	float test2 = test + distance;
	
	if(Vector3::Dot(position,normal)+distance <= 0.0f) {
		return false;
	}

	return true;
}