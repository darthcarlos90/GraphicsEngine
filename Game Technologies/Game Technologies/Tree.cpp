#include "Tree.h"
#include "Branch.h"
#include "Renderer.h"
#include "PhysicsSystem.h"

Mesh* Tree::cylinder = NULL;
Mesh* Tree::sphere = NULL;
const Vector3 TRUNK_GROWTH_RATE(0.001f, 0.001f, 0.001f); //how will the tree grow
const Vector3 BRANCH_GROWTH_RATE(0, 0.01f, 0); //how much will each branch grow
//the scaling of the shapes
const float SCALE_FACTOR_X = 0.6f;
const float SCALE_FACTOR_Y = 0.1f;
const float SCALE_FACTOR_Z = 0.6f;




Tree::Tree(int level, Vector3 location, float branching_level){
	growth = 0.0f;
	sons = 4;
	height = 70.0f;
	branchColour = Vector4(0.15f, 0.25f, 0.00001f, 1.0f);
	this->branching_level = branching_level;
	diameter_x = diameter_z = 8.0f;
	current_level = level;
	speed_factor = 100.0f;

	/*
		The colours used in the cylinders and spheres below are used only for testing purposes and to identify each root,
		they don't have any other importance than that.
	*/
	//create the trunk
	trunk = new SceneNode(cylinder, Vector4(1, 0, 0, 1));
	trunk->SetTransform(Matrix4::Translation(location));
	texture = SOIL_load_OGL_texture(TEXTUREDIR"tree_tex.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	trunk->GetMesh()->SetTexture(texture);
	glBindTexture(GL_TEXTURE_2D, trunk->GetMesh()->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	trunk->SetBoundingRadius(height);
		

	if(!trunk->GetMesh()->GetTexture())
		return;
	
	trunk_physics = new PhysicsNode();
	trunk_physics->SetPosition(location);
	trunk_physics->Stop();
	trunk_physics->putRest(true);
	trunk_physics->setType(0);
	//Height of the first trunk is 160
	CollisionCylinder cc(location, 8.0f,160.0f, Vector3(location.x, location.y + 80.0f, location.z), Vector3(location.x, location.y - 80.0f, location.z));
	trunk_physics->SetCollisionCylinder(cc);
	trunk_physics->SetTarget(trunk);
	PhysicsSystem::GetPhysicsSystem().AddNode(trunk_physics);
	AddChild(trunk);
}



void Tree::Update(float msec){
	//The growth variable saves how many seconds have passed
	growth+= msec;
	//if 1 second has passed
	//The smaller the number, the faster
	if(growth >= speed_factor){		
		//grow until you reach a limit
		if(height < 100.0f){
			ScaleTree(GetModelScale() + TRUNK_GROWTH_RATE);
			SetTransform(GetTransform() * Matrix4::Scale(GetModelScale()) * Matrix4::Translation(Vector3(0.0f, TRUNK_GROWTH_RATE.y * 500.0f, 0.0f)));
			trunk_physics->GetCollisionCylinder().updateCylinder(trunk->GetWorldTransform().GetPositionVector());
		} 
		//when you are finished growing, grow childs
		else {
			if(sons > 0){
				Branch* b = new Branch(current_level, current_level - 1, height, sons, branchColour, current_level, texture, branching_level);
				trunk->AddChild(b);
				sons--;
			}
		}
		//if a second has passed, reset the buffer
		growth = 0.0f;
	}

	//finish updating
	SceneNode::Update(msec);
}

/*
	Method: Scale Tree
		Method that scales the tree to a value.
	Params:
		value: the value to scale the tree to.
*/
void Tree::ScaleTree(Vector3 value){
	SetModelScale(value);
	
	height += value.y;
	diameter_x += value.x;
	diameter_z += value.z;
}

Vector3 Tree::getStartPoint(Vector3 midPoint, float angle, float length, Vector3 rotationAngle){
	Vector3 result;
	
	if(rotationAngle.x > 0){
		result.x = midPoint.x;
		result.y = midPoint.y - (sin(angle * DEG_RAD) * (length/ 2.0f));
		result.z = midPoint.z - (cos(angle * DEG_RAD) * (length/ 2.0f));
	} else if (rotationAngle.y > 0){
		result.x = midPoint.x - (sin(angle* DEG_RAD) * (length / 2.0f));
		result.y = midPoint.y;
		result.z = midPoint.z - (cos(angle* DEG_RAD) * (length / 2.0f));
	} else if(rotationAngle.z > 0){
		result.x = midPoint.x - (cos(angle* DEG_RAD) * (length / 2.0f));
		result.y = midPoint.y - (sin(angle* DEG_RAD) * (length / 2.0f));
		result.z = midPoint.z;
	}

	return result;
}

Vector3 Tree::getEndPoint(Vector3 midPoint, float angle, float length, Vector3 rotationAngle){
	Vector3 result;
	
	if(rotationAngle.x > 0){
		result.x = midPoint.x;
		result.y = midPoint.y + (sin(angle* DEG_RAD) * length/ 2.0f);
		result.z = midPoint.z + (cos(angle* DEG_RAD) * length/ 2.0f);
	} else if (rotationAngle.y > 0){
		result.x = midPoint.x + (sin(angle* DEG_RAD) * length / 2.0f);
		result.y = midPoint.y;
		result.z = midPoint.z + (cos(angle* DEG_RAD) * length / 2.0f);
	} else if(rotationAngle.z > 0){
		result.x = midPoint.x + (cos(angle* DEG_RAD) * length / 2.0f);
		result.y = midPoint.y + (sin(angle* DEG_RAD) * length / 2.0f);
		result.z = midPoint.z;
	}

	return result;
}