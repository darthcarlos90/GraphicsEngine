/******************************************************************************
Class:PhysicsSystem
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk> and YOU!
Description: A very simple physics engine class, within which to implement the
material introduced in the Game Technologies module. This is just a rough 
skeleton of how the material could be integrated into the existing codebase -
it is still entirely up to you how the specifics should work. Now C++ and
graphics are out of the way, you should be starting to get a feel for class
structures, and how to communicate data between systems.

It is worth poinitng out that the PhysicsSystem is constructed and destructed
manually using static functions. Why? Well, we probably only want a single
physics system to control the entire state of our game world, so why allow 
multiple systems to be made? So instead, the constructor / destructor are 
hidden, and we 'get' a single instance of a physics system with a getter.
This is known as a 'singleton' design pattern, and some developers don't like 
it - but for systems that it doesn't really make sense to have multiples of, 
it is fine!

-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////


#pragma once

#include "PhysicsNode.h"
#include "CollisionAABB.h"
#include "CollisionData.h"
#include "CollisionSphere.h"
#include "OctTreeNode.h"
#include "Line.h"
#include <vector>
#include "DebugDrawer.h"
#include "Constraint.h"


using std::vector;
using std::pair;

/*
Rich: 
There are a few ways of integrating the collision volumes
discussed in this module into your little physics engine.
You could keep pointers to all types inside a physics node,
and check to see which ones are NULL. Or you could make all
collision volumes inherit from a base class, so we only need
a single pointer inside each Physics Node. We can then either
use a process of dynamic casting to determine which exact type
of volume the pointer is, or just cheat and use an enum member
variable (I do this in my own work, you can do whichever you
feel comfortable with!).

enum CollisionVolumeType {
	COLLISION_SPHERE,
	COLLISION_AABB,
	COLLISION_PLANE
}


class CollisionVolume {
public:
	CollisionVolumeType GetType() { return type;}


protected:
	CollisionVolumeType type;
};

*/

//

#define OCTREE_SIZES (257 * 16) /** 2*/
class PhysicsSystem	{
public:
	friend class GameClass;

	void		Update(float msec);

	void		BroadPhaseCollisions();
	void		NarrowPhaseCollisions();

	//Statics
	static void Initialise() {
		
		instance = new PhysicsSystem();
	}

	static void Destroy() {
		delete instance;
	}

	static PhysicsSystem& GetPhysicsSystem() {
		return *instance;
	}

	void	AddNode(PhysicsNode* n);

	void	RemoveNode(PhysicsNode* n);

	void	AddConstraint(Constraint* c);

	void	RemoveConstraint(Constraint* c);

	void	AddDebugDraw(DebugDrawer* d);

	void	RemoveDebugDraw(DebugDrawer* d);
	void    DrawDebug();

	float getframerate() { float result = framerate; framerate = 0.0f; return result; }
	float getMsec() { return msec_passed; }
	int getColllisions() { return collision_count; }

protected:
	PhysicsSystem(void);
	~PhysicsSystem(void);


	

	bool SphereSphereCollision(const CollisionSphere &s0, const CollisionSphere &s1, CollisionData *collisionData = NULL) const;
	bool AABBCollision(const CollisionAABB &cube0, const CollisionAABB &cube1) const;

	bool SphereAABBCollision(PhysicsNode &sphere, CollisionAABB &cube, CollisionData *collisionData = NULL); //Research!!!! :-)
	float helperCollision(float cube_pos_min,float cube_pos_max, float sphere_pos) const;

	//Sphere plane collisions we started in the previous module, and expand upon via the Plane class..
	bool PointInConvexPolygon(const Vector3& testPosition, vector<Vector3> convexShapePoints, int numPointsL) const;
	bool LineLineIntersection(const Line& line0, const Line& line1, float * t0 = NULL, float *t1 = NULL) const;

	static void AddCollisionImpulse(PhysicsNode &n0, PhysicsNode &n1, const Vector3& hitPoint, const Vector3& normal, float penetration);
	static void AddCollisionImpulsePlane(PhysicsNode &node, const Vector3&hitPoint, const Vector3& normal, float penetration);
	bool InsideConcaveShape(const Vector3 * shapePoints, const int numPoints, const Vector3& testPoint);
	bool SpherePlaneCollision (PhysicsNode& p0, Plane& p1, CollisionData* data = NULL);
	bool SphereCylinderCollision (PhysicsNode& sphere, PhysicsNode& cylinder, CollisionData* data = NULL);


//Statics
	static PhysicsSystem* instance;
	
	OctTreeNode* root;

	vector<PhysicsNode*> allNodes;
	vector<pair<PhysicsNode* , PhysicsNode*>> onCollision;
	float framerate;
	float msec_passed;
	int collision_count;
	int element_number;
	vector<Constraint*> allSprings;
	vector<DebugDrawer*> allDebug;
	vector <Vector3> debug_positions;
	
	
};

