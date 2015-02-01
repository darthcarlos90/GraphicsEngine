#pragma once
#include "../../nclgl/Vector3.h"
/**
	Class CollisionCylinder.
	Description:
		This class is used for the collision detection of the tree;
**/
const float DEG_RAD = 0.0174532925f;

class CollisionCylinder{
public:
	CollisionCylinder(const Vector3 &p, float r, float h, const Vector3& ep, const Vector3& sp){
		position = p;
		radius = r;
		height = h;
		endpoint = ep;
		startPoint = sp;
	}
	CollisionCylinder(){};
	~CollisionCylinder(){};

	void updateCylinder(Vector3 p){
		position = p;
		startPoint = Vector3(position.x, position.y - height/2.0f, position.z);
		endpoint = Vector3(position.x, position.y + height/2.0f, position.z);
	}
	//Calculate the angle of inclination of the tree.
	void updateCylinderAngle(Vector3 p, float angle, Vector3 rotationAxis){
		position = p;
		startPoint = calculateStartPoint(angle, rotationAxis);
		endpoint = calculateEndPoint(angle, rotationAxis);
	}

	Vector3 position;
	float radius;
	float height;
	Vector3 endpoint;
	Vector3 startPoint;

private:
	Vector3 calculateStartPoint(float angle, Vector3 rotationAngle){
		Vector3 result;
	
	if(rotationAngle.x > 0){
		result.x = position.x;
		result.y = position.y - (sin(angle * DEG_RAD) * (height/ 2.0f));
		result.z = position.z - (cos(angle * DEG_RAD) * (height/ 2.0f));
	} else if (rotationAngle.y > 0){
		result.x = position.x - (sin(angle* DEG_RAD) * (height / 2.0f));
		result.y = position.y;
		result.z = position.z - (cos(angle* DEG_RAD) * (height / 2.0f));
	} else if(rotationAngle.z > 0){
		result.x = position.x - (cos(angle* DEG_RAD) * (height / 2.0f));
		result.y = position.y - (sin(angle* DEG_RAD) * (height / 2.0f));
		result.z = position.z;
	}

	return result;
	}

	Vector3 calculateEndPoint(float angle, Vector3 rotationAngle){
		Vector3 result;
	
	if(rotationAngle.x > 0){
		result.x = position.x;
		result.y = position.y + (sin(angle* DEG_RAD) * height/ 2.0f);
		result.z = position.z + (cos(angle* DEG_RAD) * height/ 2.0f);
	} else if (rotationAngle.y > 0){
		result.x = position.x + (sin(angle* DEG_RAD) * height / 2.0f);
		result.y = position.y;
		result.z = position.z + (cos(angle* DEG_RAD) * height / 2.0f);
	} else if(rotationAngle.z > 0){
		result.x = position.x + (cos(angle* DEG_RAD) * height / 2.0f);
		result.y = position.y + (sin(angle* DEG_RAD) * height / 2.0f);
		result.z = position.z;
	}

	return result;
	}

};