#pragma once
#include "PhysicsNode.h"

class CollisionHelper{
public:

	static bool SphereSphereCollision (PhysicsNode& p0, PhysicsNode& p1, CollisionData* data = NULL);

	static bool SpherePlaneCollision(PhysicsNode& p0, PhysicsNode& p1, CollisionData* data = NULL);
	static bool SpherePlaneCollision(PhysicsNode& p0, PhysicsNode& p1, Vector3& position, CollisionData* data = NULL);
	static bool HighMapCollision(PhysicsNode&p0, Vector3* hm_vertices, int numVertices, CollisionData* data = NULL);

	static void AddCollisionImpulse (PhysicsNode& p0, PhysicsNode& p1, CollisionData& data);

	static void HeightMapCollision(Vector3* v, Vector3* normals, PhysicsNode& p);
};