/*
	Class: Tree
		This class represents the tree and the default branches created for that tree.
		Inherits from scenenode.
*/

#pragma once
#include "../nclgl/OBJMesh.h"
#include "../nclgl/SceneNode.h"
#include "GameEntity.h"

class Tree : public SceneNode{
public:
	/*
		Constructor.
		Params:
			level: The number of leves the tree will have.
			location: Where will the tree be position.
			branching_level: explained on the branch class.
	*/
	Tree( int level, Vector3 location, float branching_level = 1.0f);
	~Tree(void){};

	virtual void Update(float msec); 

	static void CreateCylinder(){
		OBJMesh *m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"cylinder_test_2.obj");
		cylinder = m;
		
	}

	static void CreateSphere(){
		OBJMesh *m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"sphere.obj");
		sphere = m;
	}

	static void DeleteCylinder(){ delete cylinder; }
	static void DeleteSphere(){ delete sphere; }	

private:
	
	int current_level; //The level of this tree
	static Mesh* cylinder;
	static Mesh* sphere;
	

	SceneNode* trunk;
	PhysicsNode* trunk_physics;
	
	float growth;
	float height;
	
	//in case the base is not a complete circle
	float diameter_x;
	float diameter_z;
	float branching_level;
	
	int sons;

	Vector4 branchColour;

	void ScaleTree(Vector3 value);
	GLuint texture;

	Vector3 getStartPoint(Vector3 midPoint, float angle, float length, Vector3 rotationAngle);
	Vector3 getEndPoint(Vector3 midpoint, float angle, float length, Vector3 rotationAngle);


	
};