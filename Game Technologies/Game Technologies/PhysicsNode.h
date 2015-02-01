/******************************************************************************
Class:PhysicsNode
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk> and YOU!
Description: This class represents the physical properties of your game's
entities - their position, orientation, mass, collision volume, and so on.
Most of the first few tutorials will be based around adding code to this class
in order to add correct physical integration of velocity / acceleration etc to
your game objects. 


In addition to the tutorial code, this class contains a pointer to a SceneNode.
This pointer is to the 'graphical' representation of your game object, just 
like the SceneNode's used in the graphics module. However, instead of 
calculating positions etc as part of the SceneNode, it should instead be up
to your 'physics' representation to determine - so we calculate a transform
matrix for your SceneNode here, and apply it to the pointer. 

Your SceneNode should still have an Update function, though, in order to
update any graphical effects applied to your object - anything that will end
up modifying a uniform in a shader should still be the job of the SceneNode. 

Note that since the SceneNode can still have children, we can represent our
entire CubeRobot with a single PhysicsNode, and a single SceneNode 'root'.

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////


#pragma once

#include "../../nclgl/Quaternion.h"
#include "../../nclgl/Vector3.h"
#include "../../nclgl/Matrix4.h"
#include "../../nclgl/SceneNode.h"
#include "../../nclgl/Matrix3.h"
#include "../../nclgl/Plane.h"

#include "CollisionAABB.h"
#include "CollisionData.h"
#include "CollisionSphere.h"
#include "CollisionCylinder.h"
#include "Renderer.h"

//The collision data types
enum CollisionType {
	CYLINDER, 
	SPHERE, 
	CUBE, 
	PLANE,
	IMAGINARY,
	INTELLIGENT,
	N_ELEMENTS
};


class PhysicsNode	{
public:
	PhysicsNode(void);
	PhysicsNode(Quaternion orientation, Vector3 position, float radius,int id, float mass = 1.0f);//id used for THE ENUM
	PhysicsNode(Quaternion orientation, Vector3 position, int id, float mass = 1.0f);
	PhysicsNode(Vector3 position, float radius, int id, float mass = 1.0f);
	~PhysicsNode(void);

	Vector3		GetPosition()			{ return m_position;}
	
	void SetPosition(Vector3 new_position) {
		m_position = new_position; 
	}

	Vector3		GetLinearVelocity()		{ return m_linearVelocity;}
	void SetLinearVelocity (Vector3 v)	{ m_linearVelocity = v; }
	void SetAngularVelocity (Vector3 v) { m_angularVelocity = v; }
	
	float		GetInverseMass()		{ return m_invMass; }

	Quaternion	GetOrientation()		{ return m_orientation;}
	void SetOrientation(Quaternion orientation) { m_orientation = orientation; }
	Vector3		GetAngularVelocity()	{ return m_angularVelocity;}

	Matrix4		BuildTransform();

	inline void AddForce(Vector3 point, Vector3 force) {
		m_force = m_force + force;
		m_torque = m_torque + Vector3::Cross(point - m_position, force);
	}

	virtual void		Update(float msec);

	void	SetTarget(SceneNode *s) { target = s;}
	SceneNode* getTarget() { return target; }

	Vector3	GetForce()	{ return m_force;}
	Vector3	GetTorque() { return m_torque;}

	void SetMass (float m);
	float GetMass() { return m_Mass;}

	void SetLinearMotion() {linearMotion = true;}
	void SetAngularMotion() {angularMotion = true;}

	void ExplicitEuler(Vector3 acceleration, float msec);
	void VerletIntegration(Vector3 acceleration, float msec);
	Vector3 calculateAcceleration ();

	void SetInertiaMatricesSphere(float radius, float mass);
	void SetInertiaMatricesCuboid(float length, float height, float width, float mass);
	void SetInertiaMatricesCylinder(float radius, float height, float mass);
	void AngularVelocity(float msec);

	CollisionSphere& GetCollisionSphere() {return collisionSphere;}
	void SetCollisionSphereRadius (float r) {collisionSphere.m_radius = r; }
	float GetCollisionSphereRadius() { return collisionSphere.m_radius;}
	void SetCollisionSphere (CollisionSphere cs) { collisionSphere = cs;}
	void Stop();

	void SetCollisionCylinderRadius(float r) { collisionCylinder.radius = r; }
	void SetCollisionCylinderHeight(float h) { collisionCylinder.height = h; }
	void SetCollisionCylinderPosition(Vector3 p) { collisionCylinder.position = p; }
	void SetCollisionCylinderEndPoint(Vector3 ep) { collisionCylinder.endpoint = ep; }
	void SetCollisionCylinder(CollisionCylinder cc) { collisionCylinder = cc; }
	CollisionCylinder& GetCollisionCylinder() { return collisionCylinder; }
	CollisionAABB& GetCollisionCube() { return collisionCube; }

	Plane *getPlane() { return m_plane; }

	int getId() { return m_type; }
	void setForce(Vector3 force) {m_force = force;}

	Matrix3 getInvInertia() {return inverse_inertia_matrix;}
	bool isRest() { return rest; }
	void putRest(bool val) ;
	bool isPlane() { if(m_type == PLANE) return true; else return false;}
	bool isTree() {if(m_type == CYLINDER) return true; else return false; }
	bool isImaginary() { if (m_type == IMAGINARY) return true; else return false; }
	bool isCube() { if(m_type == CUBE) return true; else return false; }
	bool isSphere() { if(m_type == SPHERE) return true; else return false; }
	bool isIntelligent() { if(m_type == INTELLIGENT) return true; else return false; }
	void setType(int id);
	void setGravity(bool val) { toggle_gravity = val; }
	void setUse(bool val) { inUse = val; }
	bool getUsing() { return inUse; }

	//if this element was shoot at
	inline void setShootAt(bool val, Vector3 bulletPosition = Vector3(0,0,0)){ 
		if(val){
			this->bulletPosition = bulletPosition;
		}
		shootAt = val; 
	}
	Vector3 getBulletPosition() { return bulletPosition; }
	bool getShootAt() { return shootAt; }

	void BuildIntelligetnSphere();
	//set this element inamovible
	inline void SetStone(bool val) { 
		if(val){
			SetMass(0.0f);
			inverse_inertia_matrix.SetBalanceInertiaMatrix(0,0,0);
		}
		isStone = val; 
	}
	bool is_Stone() {return isStone; }
	bool isBroken() {return broken; }

	//if this element exploded
	inline void setBroken (bool val){
		if(val){
			Renderer::GetRenderer().increaseScore();
		}
		broken = val;
	}

	void SetGreanade(bool val) { isGreanade = val; }
	bool getGreanade() { return isGreanade; }
	int getHits() { return hits; }
	void increasehits() { hits++; }
	

protected:
	void BuildSphereCollisionSphere();
	void BuildCubeCollision();

	
	//<---------LINEAR-------------->
	Vector3		m_position;
	Vector3		m_linearVelocity;
	Vector3		m_force;
	float		m_invMass;
	float		m_Mass;
	

	Vector3		m_last_position;

	//<----------ANGULAR--------------->
	Quaternion  m_orientation;
	Vector3		m_angularVelocity;
	Vector3		m_torque;
	Matrix4     m_invInertia;

	SceneNode*	target;
	bool linearMotion;
	bool angularMotion;
	bool rest;
	bool toggle_gravity;

	//custom class Matrix3 :)
	Matrix3 inertia_matrix;
	Matrix3 inverse_inertia_matrix;
	Vector3 d;
	
	CollisionSphere collisionSphere;
	CollisionCylinder collisionCylinder;
	CollisionAABB collisionCube;

	Plane *m_plane;
	CollisionType m_type;
	float radius;

	bool inUse;
	bool shootAt;
	Vector3 bulletPosition;
	bool isStone;
	bool broken;
	bool isGreanade;

	int hits;
	

};