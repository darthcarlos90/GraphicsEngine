#include <assert.h>
#include "PhysicsSystem.h"
#include "CollisionHelper.h"
#include "Renderer.h"



PhysicsSystem* PhysicsSystem::instance = 0;

//12.0f
const float GROUND_FACTOR = 12.0f;//NOT USED

PhysicsSystem::PhysicsSystem(void)	{
	framerate = 0.0f;
	msec_passed = 0.0f;
	collision_count = 0;
	element_number = 0.0f;
}

PhysicsSystem::~PhysicsSystem(void)	{
	for (unsigned int i = 0; i < allSprings.size(); i++) {
		delete allSprings[i];
	}
	delete root;
}

void	PhysicsSystem::Update(float msec) {	
	BroadPhaseCollisions();
	NarrowPhaseCollisions();
	framerate ++;
	msec_passed = msec;

	for(vector<Constraint*>::iterator i = allSprings.begin(); i != allSprings.end(); ++i) {
		(*i)->Update(msec);
	}
	for(unsigned int i = 0; i < allNodes.size(); i++) {
		allNodes[i]->Update(msec);
	}
}

void	PhysicsSystem::BroadPhaseCollisions() {
	vector<PhysicsNode*> elements;
	for(unsigned int i = 0; i < allNodes.size(); i++){

		if(!allNodes[i]->isPlane() && !allNodes[i]->isImaginary()){
			elements.push_back(allNodes[i]);
		}
	}

	//Uncomment line 52, and change the 200 number to 41 to see how beautifully the octree draws itself :)
	//the problem is that it doesnt fully work, DAMN MURPHY LAW AT 5 AM
	root = new OctTreeNode(OCTREE_SIZES, OCTREE_SIZES, OCTREE_SIZES, Vector3(0,0,0),elements,10, 40, 0);
	root->BuildOctTree();
	root->DrawDebugLines();
	onCollision = root->getPairedElements();

	//Uncomment tis lines for a element vs element collision detection, and see that
	//eventhough my metod doesnt work, it is more efficient than the usual method
	/*for(int i = 0; i < elements.size(); i++){
		for (int j = i + 1; j < elements.size(); j++){
			pair<PhysicsNode*, PhysicsNode*> col;
			col = std::make_pair(elements[i], elements[j]);

			onCollision.push_back(col);
		}
	}*/

	
}

//handle all collisions
void	PhysicsSystem::NarrowPhaseCollisions() {
	int planeLocation = 0;
	bool doPlaneCollision = false;
	for(unsigned int i = 0; i < allNodes.size(); i++){
		if(allNodes[i]->isPlane()){
			planeLocation = i;
			doPlaneCollision = true;
			break;
		}
	}
	//first handle the plane
	if(doPlaneCollision){
		//Get the location of the plane
		vector<Vector3> points;
		float location = (257.0f * 16.0f) / 2.0f;
		points.push_back( Vector3(-location, 0, -location));
		points.push_back(Vector3(location, 0, -location));
		points.push_back(Vector3(location, 0, location));
		points.push_back(Vector3(-location, 0, location));
		for(unsigned int i = 0; i < allNodes.size(); i++){
			if(i != planeLocation){
				//If the element is inside the plane ....
				if(PointInConvexPolygon(allNodes[i]->GetPosition(),points, 4)){
				//if(InsideConcaveShape(points, 4, allNodes[i]->GetPosition())){
					CollisionData colDat;
					//and if it is not a tree
					if(!allNodes[i]->isTree()){
						//NEW CHANGES!!!!!
						//First, get the radius of the element.
						float radius = allNodes[i]->GetCollisionSphereRadius();
						Vector3 location = allNodes[i]->GetPosition();
						//Now, we are going to check which vertices are inside the area created by the radius
					
						vector <int> insideVertices;
						Vector3 averageLocation;
						int limit = allNodes[planeLocation]->getTarget()->GetMesh()->getNumVertices();
						for (unsigned int verts = 0; verts < limit; verts++){
							Vector3 temp = allNodes[planeLocation]->getTarget()->GetMesh()->getVertexAt(verts);
							if (temp.x >= location.x - radius && temp.x <= location.x + radius){
								if (temp.z >= location.z - radius && temp.z <= location.z + radius){
									insideVertices.push_back(verts);
									averageLocation += temp;
								}
							}
						}
						averageLocation = Vector3((averageLocation.x / (float)insideVertices.size()), (averageLocation.y / (float)insideVertices.size()), (averageLocation.z / (float)insideVertices.size()));
						float length = averageLocation.Length();
						//Now, we are going to get the normals of those elements
						vector <Vector3> plane_normals;
						for(unsigned int index = 0; index < insideVertices.size(); index++){
							plane_normals.push_back(allNodes[planeLocation]->getTarget()->GetMesh()->getNormalAt(insideVertices[index]));
						}

						//Now that we've got the normals, we interpolate them into one single normal
						Vector3 final_normal;
						for (unsigned int index = 0; index < plane_normals.size(); index++){
							final_normal += plane_normals[index];
						}
						//We normalize the result and weve got a normal for that piece of plane
						final_normal.Normalise();
						//Now, we treat that as a plane, create one, and check for collitions
						Plane plane(final_normal, length);
						
						
						if(CollisionHelper::SpherePlaneCollision(*allNodes[i],plane/* *allNodes[planeLocation]*/, &colDat)){
							if(allNodes[i]->getGreanade()){
								allNodes[i]->setBroken(true);
							}else if(!allNodes[i]->isRest()){
								//CollisionHelper::AddCollisionImpulse(*allNodes[i], *allNodes[planeLocation], colDat);
								allNodes[i]->Stop();
							} else {
								//if the element is at rest, continue at rest
								allNodes[i]->putRest(true);
							}
						}
					}

				}
			}
			
		}

		//now handle the collision of the paired elements
		for(vector<pair<PhysicsNode* , PhysicsNode*>>::iterator i = onCollision.begin(); i != onCollision.end(); i++){
			CollisionData  cd;
			//if there is no tree
			if(!i->first->isTree() && !i->second->isTree()){
				if(i->first->isIntelligent() && i->second->isIntelligent()){
					if(CollisionHelper::SphereSphereCollision(*i->first, *i->second, &cd)){
						i->first->setShootAt(true, i->second->GetPosition());
						i->second->setShootAt(true, i->first->GetPosition());
					}
				}

				if(i->first->isIntelligent() && i->second->isSphere()){
					if(CollisionHelper::SphereSphereCollision(*i->first, *i->second, &cd)){
						if(SphereAABBCollision(*i->second, i->first->GetCollisionCube(), &cd)){
							//cout << "ive been hit! :(" << endl;
							i->first->increasehits();
							Renderer::GetRenderer().increaseScore();
							//i->second->Stop();
						}else {
							i->first->setShootAt(true, i->second->GetPosition());
						}
					} 
				}else if(i->first->isSphere() && i->second->isIntelligent()){
					if(CollisionHelper::SphereSphereCollision(*i->first, *i->second, &cd)){
						if(SphereAABBCollision(*i->first, i->second->GetCollisionCube(), &cd)){
							//cout << "ive been hit! :(" << endl;
							Renderer::GetRenderer().increaseScore();
							//i->first->Stop();
							i->second->increasehits();
						}else {
							i->second->setShootAt(true, i->first->GetPosition());
						}
						
					} 
					
				} else if(i->first->isSphere() && i->second->isSphere()){
					if(CollisionHelper::SphereSphereCollision(*i->first, *i->second, &cd)){
						if(i->first->is_Stone()){
							if(!i->first->isBroken()){
								Vector4 old_color = i->first->getTarget()->GetColour();
								Vector4 new_color = Vector4(old_color.x - 0.1f, old_color.y - 0.1f, old_color.z - 0.1f,1) ;
								i->first->getTarget()->SetColour(new_color);
								if (new_color.x <= 0.0f && new_color.y <= 0.0f && new_color.z <= 0.0f){
									i->first->setBroken(true);
								}
							}if(i->second->getGreanade()){
								i->second->setBroken(true);
							}
						} else if(i->second->is_Stone()){
							if(!i->second->isBroken()){
								Vector4 old_color = i->second->getTarget()->GetColour();
								Vector4 new_color = Vector4(old_color.x - 0.1f, old_color.y - 0.1f, old_color.z - 0.1f,1) ;
								i->second->getTarget()->SetColour( new_color);
								if (new_color.x <= 0.0f && new_color.y <= 0.0f && new_color.z <= 0.0f){
									i->second->setBroken(true);
								}
							}

							if(i->first->getGreanade()){
								i->first->setBroken(true);
							}
						} else if(i->first->getGreanade()){
							i->first->setBroken(true);
						} else if (i->second->getGreanade()){
							i->second->setBroken(true);
						}						
						else {
							collision_count++;
							i->first->putRest(false);
							i->second->putRest(false);
						}
						CollisionHelper::AddCollisionImpulse(*i->first, *i->second, cd); 
					}
				}else if (i->first->isCube() && i->second->isCube()){
					if(AABBCollision(i->first->GetCollisionCube(), i->second->GetCollisionCube()))
						cout << "Cube cube collision" << endl;
				} else {
					if(i->first->isCube()){
						if(SphereAABBCollision(*i->second, i->first->GetCollisionCube(), &cd)){
							Renderer::GetRenderer().increaseScore();
							//i->second->putRest(true);
							i->second->setUse(false);
							//cout << "Cube sphere collision" << endl;
						}
					} else {
						if(SphereAABBCollision(*i->first, i->second->GetCollisionCube(), &cd)){
							//CollisionHelper::AddCollisionImpulse(*i->first, *i->second, cd);
							i->first->Stop();
							//cout << "Sphere cube collision" << endl;
						}
					}
				}
			}	else {
				if(i->first->isTree()){
					if(!i->second->isTree()){
						if(SphereCylinderCollision(*i->second, *i->first, &cd)){
							collision_count++;
							if(i->second->getGreanade()){
								i->second->setBroken(true);
							} else {
								CollisionHelper::AddCollisionImpulse(*i->second, *i->first, cd);
								Renderer::GetRenderer().decreaseScore();
							}
						}
					}
				} else {
					if(!i->first->isTree()){
						if(SphereCylinderCollision(*i->first, *i->second, &cd)){
							collision_count++;
							if(i->first->getGreanade()){
								i->first->setBroken(true);
							} else {
								CollisionHelper::AddCollisionImpulse(*i->first, *i->second, cd);
								Renderer::GetRenderer().decreaseScore();
							}
						}
					}
				}
			}
		}
	}
}

void	PhysicsSystem::AddNode(PhysicsNode* n) {
	allNodes.push_back(n);
}

void	PhysicsSystem::RemoveNode(PhysicsNode* n) {
	for(vector<PhysicsNode*>::iterator i = allNodes.begin(); i != allNodes.end(); ++i) {
		if((*i) == n) {
			allNodes.erase(i);
			return;
		}
	}
}

bool PhysicsSystem::SphereSphereCollision(const CollisionSphere &s0, const CollisionSphere &s1, CollisionData *collisionData) const {
	Vector3 temp = s0.m_pos - s1.m_pos;
	const float distSq = Vector3::LengthSq(temp);
	//assert(distSq > 0.00001f);

	const float sumRadius = (s0.m_radius + s1.m_radius);
	if(distSq < (sumRadius * sumRadius)){
		if(collisionData){
			collisionData->m_penetration = sumRadius - sqrtf(distSq);
			Vector3 normal = s0.m_pos - s1.m_pos;
			normal.Normalise();
			collisionData->m_normal = normal;
			collisionData->m_point = s0.m_pos - normal * (s0.m_radius - collisionData->m_penetration * 0.5f);
		}

		return true; //Collision
	}
	return false; // No collision
}

bool PhysicsSystem::AABBCollision(const CollisionAABB &cube0, const CollisionAABB &cube1) const{
	// Test along the x axis
	float dist = cube0.position.x - cube1.position.x;
	float sum = (cube0.halfdims.x + cube1.halfdims.x);
	if(dist < sum){
		dist = cube0.position.y - cube1.position.y;
		sum = cube0.halfdims.y + cube1.halfdims.y;
		if(dist < sum){
			float dist = cube0.position.z - cube1.position.z;
			float sum = cube0.halfdims.z + cube1.halfdims.z;
			if(dist < sum){
				//There is an overlap in all axis
				return true;
			}
		}
	}
	
	return false;
}

bool PhysicsSystem::SphereAABBCollision(PhysicsNode &sphere, CollisionAABB &cube, CollisionData *data){
	float distance = 0.0f;
	float radius = sphere.GetCollisionSphereRadius();
	Vector3 position = sphere.GetPosition();
	distance += helperCollision(cube.min_positions.x, cube.max_positions.x, position.x);
	distance += helperCollision(cube.min_positions.y, cube.max_positions.y, position.y);
	distance += helperCollision(cube.min_positions.z, cube.max_positions.z, position.z);

	if( distance <= (radius * radius)){
		
		return true;

	}

	return false;
}

float PhysicsSystem::helperCollision (float cube_pos_min,float cube_pos_max, float sphere_pos) const{
	float result = 0.0f;
	float separation = 0.0f;
	if(sphere_pos < cube_pos_min){
		separation = sphere_pos - cube_pos_min;
		result = separation * separation;
	} else if (sphere_pos > cube_pos_max){
		separation = sphere_pos - cube_pos_max;
		result = separation * separation;
	} 

	return result;
}

bool PhysicsSystem::PointInConvexPolygon(const Vector3& testPosition, vector<Vector3> convexShapePoints, int numPointsL) const {
	
	//Check if our test point is inside our complex shape
	for(int i = 0; i < numPointsL; i++){
		const int i0 = i;
		const int i1 = (i + 1)%numPointsL;

		const Vector3& p0 = convexShapePoints[i0];
		const Vector3& p1 = convexShapePoints[i1];
		
		//We need two things for each edge, a point on the edge, and a normal
		Vector3 difference = p0 - p1;
		difference.Normalise();
		const Vector3 n = Vector3::Cross(Vector3(0,-1,0), difference);
		const float d = Vector3::Dot(n, p0);
		const float s = d - Vector3::Dot(n, testPosition);

		if(s < 0.0f){
			return false;
		}
	}

	return true;

}


bool PhysicsSystem::InsideConcaveShape(const Vector3 * shapePoints, const int numPoints, const Vector3& testPoint){
	
	int intersectionCount = 0;

	for(int i = 0; i < numPoints; i++){
		const int i0 = i;
		const int i1 = (i + 1)%numPoints;

		const Vector3& p0 = shapePoints[i0];
		const Vector3& p1 = shapePoints[i1];
		Line l(testPoint, Vector3(0,0,0));
		
		bool intesect = LineLineIntersection(Line(p0, p1), Line(testPoint, testPoint + Vector3(3000.0f, 0.0f,2000.0f)));

		if(intesect){
			intersectionCount ++;
		}
	}

	if(intersectionCount % 2 == 0)
		return false;

	return true;
}

bool PhysicsSystem::LineLineIntersection(const Line& line0, const Line& line1, float * t0, float * t1) const{
	
	const Vector3& point0 = line0.point0;
	const Vector3& point1 = line0.point1;
	const Vector3& point2 = line1.point0;
	const Vector3& point3 = line1.point1;

	const float div = (point3.z - point2.z) * (point1.x - point0.x) - (point3.x - point2.x) * (point1.z - point0.z);

	if(abs(div) < 0.000001f)
		return false;

	const float ta = ((point3.x - point2.x) * (point0.z - point2.z) - (point3.z - point2.z) * (point0.x - point2.x)) / div;

	if(ta < 0.0f || ta > 1.0f)
		return false;

	const float tb = ((point1.x - point0.x) * (point0.z - point2.z) - (point1.z - point0.z) * (point0.x - point2.x)) / div;
	
	if(tb < 0.0f || tb > 1.0f)
		return false;

	if(t0)
		(*t0) = ta;

	if(t1)
		(*t1) = tb;

	return true;
}


void PhysicsSystem::AddCollisionImpulse(PhysicsNode& n0, PhysicsNode& n1, const Vector3& hitPoint, const Vector3& normal, float penetration){
	float invMass0 = n0.GetInverseMass();
	float invMass1 = n1.GetInverseMass();

	const Matrix3 worldInvInertia0 = n0.getInvInertia();
	const Matrix3 worldInvInertia1 = n1.getInvInertia();

	//Both objects are non movable
	if((invMass0 + invMass1) == 0.0) 
		return;

	Vector3 r0 = hitPoint - n0.GetPosition();
	Vector3 r1 = hitPoint- n1.GetPosition();

	Vector3 v0 = n0.GetLinearVelocity() + Vector3::Cross(n0.GetAngularVelocity(), r0);
	Vector3 v1 = n1.GetLinearVelocity() + Vector3::Cross(n1.GetAngularVelocity(), r1);

	//Relative velocity
	Vector3 dv = v0 - v1;

	//If the objects are moving away from each other
	//we don't need to apply an impuse
	float relativeMovement = -Vector3::Dot(dv, normal);
	
	if(relativeMovement < 0.01f)
		return;

	//NORMAL Impulse
	{
		//Coefficient of Restitution
		float e = 0.0f;

		float normalDiv = Vector3::Dot(normal, normal) * ((invMass0 + invMass1)
			+ Vector3::Dot(normal, Vector3::Cross(Matrix3::Transform(worldInvInertia0, Vector3::Cross(r0, normal)), r0)
			+ Vector3::Cross( Matrix3::Transform(worldInvInertia1, Vector3::Cross(r1, normal)), r1)));

		float jn = -1 * (1 + e) * Vector3::Dot(dv, normal) / normalDiv;

		//Hack, fix to stop sinking
		// bias impulse proportional to penetration distance
		//wtf with this????
		jn = jn + (penetration*1.5f);

		n0.SetLinearVelocity(n0.GetLinearVelocity() + (normal * invMass0 * jn));
		n0.SetAngularVelocity(n0.GetAngularVelocity() + Matrix3::Transform(worldInvInertia0, Vector3::Cross(r0, normal * jn)));

		n1.SetLinearVelocity(n1.GetLinearVelocity() - (normal * invMass1 * jn));
		n1.SetAngularVelocity(n1.GetAngularVelocity() - Matrix3::Transform(worldInvInertia1, Vector3::Cross(r1, normal * jn)));
	}

	//TANGENT Impulse Code
	{
		//Work out out tangent vector, with it's perpendicular
		// to our collision normal
		Vector3 tangent = Vector3(0,0,0);
		tangent = dv - (normal * Vector3::Dot(dv, normal));
		tangent.Normalise();

		float tangDiv = invMass0 + invMass1
			+ Vector3::Dot(tangent, Vector3::Cross(
			(Matrix3::Transform(n0.getInvInertia(), Vector3::Cross(r0, tangent))), r0) + 
			Vector3::Cross((Matrix3::Transform(n1.getInvInertia(), Vector3::Cross(r1, tangent))), r1));

		float jt = -1 * Vector3::Dot(dv, tangent) / tangDiv;
		//Clamp min/max tangential component

		//Apply contact impulse
		n0.SetLinearVelocity(n0.GetLinearVelocity() +  ( tangent * jt * invMass0));
		n0.SetAngularVelocity(n0.GetAngularVelocity() + Matrix3::Transform(worldInvInertia0, Vector3::Cross(r0, tangent * jt)));

		n1.SetLinearVelocity(n1.GetLinearVelocity() - (tangent * invMass1 * jt));
		n1.SetAngularVelocity(n1.GetAngularVelocity() - Matrix3::Transform(worldInvInertia1, Vector3::Cross(r1, tangent * jt)));
	}//TANGENT
}

bool PhysicsSystem::SpherePlaneCollision(PhysicsNode& p0, Plane& p1, CollisionData* data){
	CollisionSphere& s0 = p0.GetCollisionSphere();

	float separation = Vector3::Dot(p0.GetPosition(), p1.GetNormal()) - p1.GetDistance();

	if(separation > s0.m_radius)
		return false;

	if(data){
		data->m_penetration = s0.m_radius - separation;
		data->m_normal = p1.GetNormal();
		data->m_point = p0.GetPosition() - p1.GetNormal() * separation;
	}

	return true;
}

void PhysicsSystem::AddCollisionImpulsePlane(PhysicsNode & node, const Vector3& hitPoint, const Vector3& normal, float penetration){
	
	//Both objects are non movable
	if(node.GetInverseMass()  == 0.0) 
		return;

	Vector3 r = hitPoint - node.GetPosition();

	Vector3 v = node.GetLinearVelocity() + Vector3::Cross(node.GetAngularVelocity(), r);

	//Relative velocity
	Vector3 dv = v;

	//If the objects are moving away from each other
	//we don't need to apply an impuse
	float relativeMovement = -Vector3::Dot(dv, normal);
	
	if(relativeMovement < 0.01f)
		return;

	//NORMAL Impulse
	{
		//Coefficient of Restitution
		float e = 0.0f;

		float normalDiv = (node.GetInverseMass()) + /*Vector3::Dot(normal, normal) * */
			+ Vector3::Dot(normal, Vector3::Cross(Matrix3::Transform(node.getInvInertia(), Vector3::Cross(r, normal)), r));

		float jn = -1 * (1 + e) * Vector3::Dot(dv, normal) / normalDiv;

		//	Hack, fix to stop sinking
		// bias impulse proportional to penetration distance
	
		jn = jn + (penetration* 0.1f);
		//the impulse is divided by tw0 to simulate ground floor
		//jn = jn/GROUND_FACTOR;
		//if(jn > 0.005){
			node.SetLinearVelocity((node.GetLinearVelocity() + normal * (node.GetInverseMass() * jn)));
			node.SetAngularVelocity(node.GetAngularVelocity() + Matrix3::Transform(node.getInvInertia(), Vector3::Cross(r, normal * jn)));
		/*} else {
			node.putRest(true);
		
		}*/
		
	}

	//TANGENT Impulse Code
	{
		//Work out out tangent vector, with it's perpendicular
		// to our collision normal
		Vector3 tangent = dv - (normal * Vector3::Dot(dv, normal));
		tangent.Normalise();

		float tangDiv = node.GetInverseMass()
			+ Vector3::Dot(tangent, Vector3::Cross(
			(Matrix3::Transform(node.getInvInertia(), Vector3::Cross(r, tangent))), r));

		float jt = -1 * Vector3::Dot(dv, tangent) / tangDiv;
		

		//Apply contact impulse
		node.SetLinearVelocity(node.GetLinearVelocity() +  tangent * (jt * node.GetInverseMass()));
		node.SetAngularVelocity(node.GetAngularVelocity() + Matrix3::Transform(node.getInvInertia(), Vector3::Cross(r, tangent * jt)));
	}//TANGENT
	
	
}


bool PhysicsSystem::SphereCylinderCollision (PhysicsNode& sphere, PhysicsNode& cylinder, CollisionData* data){
	
	CollisionSphere cs = sphere.GetCollisionSphere();
	CollisionCylinder cc = cylinder.GetCollisionCylinder();

	Vector3 center_line = cc.endpoint - cc.startPoint;
	Vector3 normal_direction = center_line;
	normal_direction.Normalise();
	float distanceFromBase = Vector3::Dot((cs.m_pos - cc.startPoint), normal_direction) /center_line.Length();
	if(distanceFromBase < 0)
		distanceFromBase = 0.0f;
	if(distanceFromBase > 1)
		distanceFromBase = 1.0f;
	Vector3 close_point = cc.startPoint  + (center_line * distanceFromBase);

	Vector3 collisionVector = cs.m_pos - close_point;
	float distance = collisionVector.Length();
	Vector3 collisionNormal = collisionVector ;
	collisionNormal.Normalise();
	float sumRadius = cs.m_radius + cc.radius;
	
	if(distance < sumRadius){
		//cout << distance << endl;
		data->m_normal = collisionNormal;
		data->m_penetration = sumRadius - distance;
		data->m_point = close_point - data->m_normal * (cc.radius - data->m_penetration * 0.5f);
		return true;
	}

	return false;
}

void	PhysicsSystem::AddConstraint(Constraint* s) {
	allSprings.push_back(s);
}

void	PhysicsSystem::RemoveConstraint(Constraint* c) {
	for(vector<Constraint*>::iterator i = allSprings.begin(); i != allSprings.end(); ++i) {
		if((*i) == c) {
			allSprings.erase(i);
			return;
		}
	}
}

void	PhysicsSystem::AddDebugDraw(DebugDrawer* d) {
	allDebug.push_back(d);
}

void	PhysicsSystem::RemoveDebugDraw(DebugDrawer* d) {
	for(vector<DebugDrawer*>::iterator i = allDebug.begin(); i != allDebug.end(); ++i) {
		if((*i) == d) {
			allDebug.erase(i);
			return;
		}
	}
}

void    PhysicsSystem::DrawDebug() {
	for(vector<DebugDrawer*>::iterator i = allDebug.begin(); i != allDebug.end(); ++i) {
		(*i)->DebugDraw();
	}
}