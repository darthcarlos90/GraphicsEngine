#include "Branch.h"
#include "Sphere.h"
#include "PhysicsSystem.h"
#include "Renderer.h"

//Set the cylinder to false.
Mesh* Branch::cylinder = NULL;
//Constructor explained on the .h file.
Branch::Branch(int depth, int actual_level, float parent_height, int id, Vector4 colour, int sons, GLuint texture, float branching_level){
	//Setting the values to the variables.
	growth = 0.0f;
	grew_bush = false;
	this->sons = sons;
	this->texture = texture;
	this->branching_level = branching_level;
	isTorch = false;
	this->colour = colour;
	current_level = actual_level;
	parent_level = depth;
	accum_angle = 0.0f;
	speed_factor = 500.0f;

	float cylinder_h = 50.0f;
	switch(actual_level){
	case 1:
		cylinder_h = 25.0f;
		break;
	case 0 :
		cylinder_h = 12.5f;
		break;
	}

	//mesh and texture management
	mesh = cylinder;
	mesh->SetTexture(texture);
	glBindTexture(GL_TEXTURE_2D, mesh->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);


	//calculating stuff
	depth_factor = depth - current_level;
	growth_coefficient = (float)depth_factor/10.0f;
	height = parent_height /1.6f;
	height_limit = parent_height + height/(1.5f + growth_coefficient);

	//setting bounding radious for frustum culling
	this->SetBoundingRadius(height);
	
	/*
		Switching the value of the id, to establish the direction of the bending, the angle, and their movement.
	*/
	switch(id){
	case 1:
		angle = 0.5f;
		distance_radius_z = -0.12f;
		direction = Vector3(1, 0, 0);
		break;
	case 2:
		angle = -0.5f;
		distance_radius_z = 0.12f;
		direction = Vector3(1, 0, 0);
		break;
	case 3:
		angle = 0.5f;
		distance_radius_z = 0.0f;
		direction = Vector3(0, 0, 1);
		break;
	case 4:
		angle = -0.5f;
		distance_radius_z = 0.0f;
		direction = Vector3(0, 0, 1);
		break;

	}

	

	/*
		Scaling the Vector, the deeper in the tree is this branch, the smaller it will be.
	*/
	Vector3 scale(1, 1, 1);
	for(int i = 0; i < depth_factor; i++){
		scale = scale * Vector3(0.6f, 0.6f, 0.6f);
	}

	//Set this branch to the 0,0,0, so it will appear as if this branch is comming out of the parent.
	SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));

	//scale
	SetModelScale(scale);

	branch_physics = new PhysicsNode();
	Matrix4 transform = GetWorldTransform();
	branch_physics->SetPosition(transform.GetPositionVector());
	branch_physics->SetOrientation(Quaternion::FromMatrix(transform));
	branch_physics->Stop();
	branch_physics->putRest(true);
	branch_physics->setType(0);
	CollisionCylinder cc;
	cc.position = transform.GetPositionVector();
	cc.radius = 8.0f * scale.x;
	cc.height = cylinder_h;
	cc.updateCylinderAngle(cc.position,angle, direction);
	branch_physics->SetCollisionCylinder(cc);
	
	PhysicsSystem::GetPhysicsSystem().AddNode(branch_physics);

}

//Constructor explained at .h file
Branch::Branch(Vector3 position){
	isTorch = true;
	this->mesh = cylinder;
	this->SetBoundingRadius(10.0f);
	SetTransform(Matrix4::Translation(position));
}
/*
	Method: GrowBranch.
		This Method moves the branch upwards so it looks like if the tree is growing.
	Params:
		value: The amount of units the tree will grow.
*/
void Branch::GrowBranch(float value){
	transform = transform* Matrix4::Translation(Vector3(0.0f, value, distance_radius_z));
	height += value;
}

/*
	Method: Bend.
		This method bends the Branch the amount of units depending of the type of branch stated on the constructor.
*/
void Branch::Bend (){
	accum_angle += angle;
	transform = transform * Matrix4::Rotation(angle, direction);
}

/*
	Method: Update.
		This inherited method updates the state of the branch. If it is a branch, it moves it and bends it.
		When it is finishes, It either grows a bus (a fruit, sorry) or grows another branch depending of the
		depth of the tree.
	Params:
		msec: The amount of milliseconds passed since the las frame update.
*/
void Branch::Update(float msec){
	growth += msec;
	
	if(growth >= speed_factor){
		//if it hasn't reached the growing limit
		if(height < height_limit){
			//grow and bend
			GrowBranch(branching_level);
			Bend();
		}else {
			//if it hasn't grown a bush (fruit) and it is at level 0, grow it
			if(!grew_bush && current_level == 0){
				Sphere* bush = new Sphere(Vector4(1.0f,0.0f,0.0f, 1.0f));
				AddChild(bush);
				grew_bush = true;
			}
		}
		branch_physics->SetPosition(this->GetWorldTransform().GetPositionVector());
		branch_physics->SetOrientation(Quaternion::FromMatrix(this->GetWorldTransform()));
		branch_physics->GetCollisionCylinder().updateCylinderAngle(branch_physics->GetPosition(), accum_angle, direction);
	//if the level is not 0, grow another branch
	if(current_level> 0){
		if(sons > 0){
			Branch* b = new Branch(parent_level, current_level-1, height, sons,Vector4(colour.x, colour.y + 0.10f, colour.z, 1.0f), 2, texture);
			AddChild(b);
			sons--;
		}
	}
	//flush the growth
	growth = 0.0f;
	} 
	// Update the scene
	SceneNode::Update(msec);
}
