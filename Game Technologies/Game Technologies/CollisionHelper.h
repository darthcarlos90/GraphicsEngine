#pragma once
#include "PhysicsNode.h"

class CollisionHelper{
public:

	static bool SphereSphereCollision (PhysicsNode& p0, PhysicsNode& p1, CollisionData* data = NULL);

	static bool SpherePlaneCollision(PhysicsNode& p0, PhysicsNode& p1, CollisionData* data = NULL);

	static void AddCollisionImpulse (PhysicsNode& p0, PhysicsNode& p1, CollisionData& data);
};