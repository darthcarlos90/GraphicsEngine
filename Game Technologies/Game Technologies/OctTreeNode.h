/**
	Class OctTreeNode
	This class represents the octree, which recursively creates more nodes
**/

#pragma once

#include "../../nclgl/Vector3.h"
#include "PhysicsNode.h"
#include <vector>


using std::vector;

class OctTreeNode{
public:
	OctTreeNode(float width, float height, float length, Vector3 origin, vector<PhysicsNode*> elements, int threshold, int length_limit, int level);
	OctTreeNode(float width, float height, float length, Vector3 origin, int threshold, int length_limit, int level);
	~OctTreeNode(void);

	void BuildOctTree();
	void PushElement(PhysicsNode* element);

	float getXLimit() { return x_axis_limit; }
	float getLowerX() { return low_x_axis_limit; }
	float getYLimit() { return y_axis_limit; }
	float getLowerY() { return low_y_axis_limit; }
	float getZLimit() { return z_axis_limit; }
	float getLowerZ() { return low_z_axis_limit; }
	void DrawDebugLines();
	void pushElements(vector <PhysicsNode*> new_elems);
	vector<pair<PhysicsNode*, PhysicsNode*>> getPairedElements();
	

private:
	
	bool isLeaf;

	int threshold;
	int length_limit;
	int level;

	float width;
	float half_width;
	float height;
	float half_height;
	float length;
	float half_length;
	float x_axis_limit;
	float low_x_axis_limit;
	float y_axis_limit;
	float low_y_axis_limit;
	float z_axis_limit;
	float low_z_axis_limit;

	Vector3 origin;

	vector <OctTreeNode> children;

	vector<PhysicsNode*> elements;
};