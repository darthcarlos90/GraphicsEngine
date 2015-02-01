#pragma once
#include "../../nclgl/Vector3.h"
#include "../../nclgl/Plane.h"

class CollisionAABB {
public:
	CollisionAABB(const Vector3 &p, float w, float h, float l){
		position = p;
		width = w;
		height = h;
		length = l;
		halfdims = position / 2;
		min_positions = Vector3(position.x - (width/2.0f), position.y - (height/2.0f), position.z - (length/2.0f));
		max_positions = Vector3(position.x + (width/2.0f), position.y + (height/2.0f), position.z + (length/2.0f));
		//Creating planes of the cube, this code is not used though
		//top plane
		top_plane.push_back(Vector3(min_positions.x, max_positions.y, min_positions.z));
		top_plane.push_back(Vector3(max_positions.x, max_positions.y, min_positions.z));
		top_plane.push_back(Vector3(max_positions.x, max_positions.y, max_positions.z));
		top_plane.push_back(Vector3(min_positions.x, max_positions.y, max_positions.z));
		Vector3 normal_top_plane(position.x, max_positions.y + 1.0f, position.z);
		planes.push_back(Plane(normal_top_plane, Vector3::Dot(normal_top_plane, Vector3(min_positions.x, max_positions.y, max_positions.z)), true));
		//bottom plane
		bottom_plane.push_back(Vector3(min_positions.x, min_positions.y, min_positions.z));
		bottom_plane.push_back(Vector3(max_positions.x, min_positions.y, min_positions.z));
		bottom_plane.push_back(Vector3(max_positions.x, min_positions.y, max_positions.z));
		bottom_plane.push_back(Vector3(min_positions.x, min_positions.y, max_positions.z));
		Vector3 normal_bottom_plane(position.x, min_positions.y - 1.0f, position.z);
		planes.push_back(Plane(normal_bottom_plane, Vector3::Dot(normal_bottom_plane, Vector3(min_positions.x, min_positions.y, max_positions.z)), true));

		//left most plane
		left_plane.push_back(Vector3(min_positions.x, max_positions.y, min_positions.z));
		left_plane.push_back(Vector3(min_positions.x, max_positions.y, max_positions.z));
		left_plane.push_back(Vector3(min_positions.x, min_positions.y, min_positions.z));
		left_plane.push_back(Vector3(min_positions.x, min_positions.y, max_positions.z));
		Vector3 normal_leftmost_plane(min_positions.x - 1.0f, position.y, position.z);
		planes.push_back(Plane(normal_leftmost_plane, Vector3::Dot(normal_leftmost_plane, Vector3(min_positions.x, min_positions.y,max_positions.z)), true));
		//right most plane
		right_plane.push_back(Vector3(max_positions.x, max_positions.y, min_positions.z));
		right_plane.push_back(Vector3(max_positions.x, max_positions.y, max_positions.z));
		right_plane.push_back(Vector3(max_positions.x, min_positions.y, min_positions.z));
		right_plane.push_back(Vector3(max_positions.x, min_positions.y, max_positions.z));
		Vector3 normal_rightmost_plane(max_positions.x + 1.0f, position.y, position.z);
		planes.push_back(Plane(normal_rightmost_plane, Vector3::Dot(normal_rightmost_plane, Vector3(max_positions.x, min_positions.y, max_positions.z)), true));

		//nearest plane
		near_plane.push_back(Vector3(min_positions.x, max_positions.y, max_positions.z));
		near_plane.push_back(Vector3(max_positions.x, max_positions.y, max_positions.z));
		near_plane.push_back(Vector3(max_positions.x, min_positions.y, max_positions.z));
		near_plane.push_back(Vector3(min_positions.x, min_positions.y, max_positions.z));
		Vector3 normal_nearest_plane(position.x, position.y, max_positions.z + 1.0f);
		planes.push_back(Plane(normal_nearest_plane, Vector3::Dot(normal_nearest_plane, Vector3(max_positions.x, min_positions.y, max_positions.z)), true));

		//farest plane
		far_plane.push_back(Vector3(min_positions.x, max_positions.y, min_positions.z));
		far_plane.push_back(Vector3(max_positions.x, max_positions.y, min_positions.z));
		far_plane.push_back(Vector3(max_positions.x, min_positions.y, min_positions.z));
		far_plane.push_back(Vector3(min_positions.x, min_positions.y, min_positions.z));
		Vector3 normal_farest_plane(position.x, position.y, min_positions.z -1.0f);
		planes.push_back(Plane(normal_farest_plane, Vector3::Dot(normal_farest_plane, Vector3(max_positions.x, min_positions.y, min_positions.z)), true));

	}


	CollisionAABB(void){};

	Vector3 position;
	Vector3 halfdims;
	Vector3 min_positions;
	Vector3 max_positions;
	float width;
	float height;
	float length;
	vector<Plane> planes;
	vector<Vector3> top_plane;
	vector<Vector3> bottom_plane;
	vector<Vector3> right_plane;
	vector<Vector3> left_plane;
	vector<Vector3> near_plane;
	vector<Vector3> far_plane;
};