#pragma once
#include "../nclgl/SceneNode.h"
#include "../nclgl/OBJMesh.h"
#include "PhysicsNode.h"
/*
	Class: Branch
	This class has been used with two purposes, the first purpose is to create the smaller brances
	of the tree, the oter purpose is to create the trunks of the torches that are displayed on the scene.
	This class inherits from SceneNode class, so it can be added to a Scene Tree for its management.
*/
class Branch : public SceneNode{
public:
	/* Constructor for the branches. 
		Params:
			depth: The Depth of the general tree.
			actual_level: The level in which this branch positions itself.
			parent_height: The height where the parent is positioned.
			id: The type of branch, depending on the number (from 1 to 4) is how
				the branch will grow and in what angle.
			colur: The colour of the branch.
			sons: How many sons will this branch have.
			texture: The texture for this branch.
			branching_level: This variable is used for the creation of random trees, if this variable is bigger than 1,
				the tree in the middle is obtained, from here I can play with the value setting it bigger than 1 or smaller
				than 1 to obtain different kind of trees.
	*/
	Branch(int depth, int actual_level, float parent_height, int id,
		 Vector4 colour, int sons, GLuint texture, float branching_level = 1.0f);
	
	/*
		This constructor is used to create the torch base.
		Params:
			position: The position where the torch will be.
	*/
	Branch(Vector3 position);
	
	/* Deconstructor. */
	~Branch(void){};

	/* This static method is used to create a cylinder that is going to be used by all the branches. The code
		for this method was obtained from Tutorial 6.*/
	static void CreateCylinder(){
		OBJMesh *m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"cylinder_test_2.obj");
		cylinder = m;
	}
	/* This method is used to destroy the cylinder figure. */
	static void DeleteCylinder(){ delete cylinder; }

	/* Inherited Update Method. */
	virtual void Update(float msec);
	
private:
	//The level of this tree.
	int current_level;

	// Variable used to calculate the seconds passed to make the growth frame independent.
	float growth;

	//The cylinder used for the figure.
	static Mesh* cylinder;

	// The height of this branch.
	float height;
	
	//The amount of sons this branch is going to have.
	int sons;

	//variables only used by son branch
	int depth_factor;

	// The growing limit of this branch.
	float height_limit;

	// The actual angle that this branch will be bending towards.
	float angle;

	//The amount of units this branch will move from the center when it's bending.
	float distance_radius_z;

	//The level of the parent.
	int parent_level;

	// Coefficient used for the calculation of the limit growth of this branch.
	// To be honest I can't remember how or why I created this variable, but everything works so ..
	float growth_coefficient;
	
	//in case the base is not a complete circle
	float diameter_x;
	float diameter_z;

	//The branching level explained above
	float branching_level;

	//A vector that states in what axis will this branche bend.
	Vector3 direction;

	//True if this branch has already grown a bush (at the end it eneded up being a fruit)
	bool grew_bush;

	//Methods explained on the .cpp class
	void GrowBranch(float value);
	void Bend();

	//The identifier of the texture beign used.
	GLuint texture;

	//True if this is a torch, false if it is a branch.
	bool isTorch;

	PhysicsNode* branch_physics;

	float accum_angle;
	
};