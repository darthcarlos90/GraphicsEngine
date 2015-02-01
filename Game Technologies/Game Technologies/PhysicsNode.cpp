#include "PhysicsNode.h"
#include "../../nclgl/Matrix3.h"
#include "Renderer.h"

const Vector3 GRAVITY(0.0f, -0.000981f, 0.0f); //TODO GRAVITY IS AN ACCELERATION OR A FORCE?
const Vector3 DAMPING_FACTOR(0.8, 0.8, 0.8); //NOT USED


PhysicsNode::PhysicsNode(void)	{
	target = NULL;
	m_plane = NULL;
	linearMotion = false;
	angularMotion = false;
	rest = false;
	toggle_gravity = false;
	inUse = true;
	shootAt = false;
	SetMass(0.0f);
	isStone = false;
	broken = false;
	isGreanade = false;
	hits = 0;

}

PhysicsNode::PhysicsNode(Quaternion orientation, Vector3 position, int id, float mass) {
	m_orientation	= orientation;
	m_position		= position;
	//collisionSphere.m_pos = m_position;
	target = NULL;
	m_plane = NULL;
	linearMotion = false;
	angularMotion = false;
	SetMass(mass);
	setType(id);
	/*if(m_type == PLANE){
		
	}*/
	rest = false;
	toggle_gravity = true;
	inUse = true;
	shootAt = false;
	isStone = false;
	broken = false;
	isGreanade = false;
	hits = 0;
}

PhysicsNode::PhysicsNode(Quaternion orientation, Vector3 position, float radius,int id, float mass) {
	m_orientation	= orientation;
	m_position		= position;
	target = NULL;
	m_plane = NULL;
	linearMotion = false;
	angularMotion = false;
	this->radius = radius;
	setType(id);
	m_invMass = 1;
	//if(m_type == PLANE){
	//	/*m_plane = new Plane(
	//		Vector3(-1.0f, 0.0f, 1.0f), 
	//		Vector3(-1.0f, 0.0f,-1.0f), 
	//		Vector3( 1.0f, 0.0f,-1.0f),
	//		true);*/
	//	m_plane = new Plane();
	//}
	SetMass(mass);
	rest = false;
	toggle_gravity = true;
	inUse = true;
	shootAt = false;
	isStone = false;
	broken = false;
	isGreanade = false;
	hits = 0;

}

PhysicsNode::PhysicsNode(Vector3 position, float radius, int id, float mass){
	m_position = position;
	m_last_position = Vector3(0,0,0);
	//collisionSphere.m_pos = m_position;
	m_plane = NULL;
	target = NULL;
	//collisionSphere.m_radius = radius;
	linearMotion = false;
	angularMotion = false;
	this->radius = radius;
	setType(id);
	/*if(m_type == PLANE){
		m_plane = new Plane();
	}*/
	SetMass(mass);
	rest = false;
	toggle_gravity = true;
	inUse = true;
	shootAt = false;
	isStone = false;
	broken = false;
	isGreanade = false;
	hits = 0;

}



PhysicsNode::~PhysicsNode(void)	{
	delete m_plane;
}

//You will perform your per-object physics integration, here!
//I've added in a bit that will set the transform of the
//graphical representation of this object, too.
void	PhysicsNode::Update(float msec) {
	//FUN GOES HERE
	
		if(linearMotion){
			ExplicitEuler(calculateAcceleration(), msec);
		}

		if(angularMotion){
			//DEBUG VECTOR:
			//Vector3(0.0002f,0.0001f,0.0003f)
			AngularVelocity( msec);
		}
		m_force = Vector3(0,0,0);
		
		if(target) {	
			target->GetTransform().GetPositionVector();
			target->SetTransform(BuildTransform());
		}

		//Update the collision elements
		if(m_type == SPHERE){
			BuildSphereCollisionSphere();
		}

		if(m_type == CUBE){
			BuildCubeCollision();
		}

		if(m_type == INTELLIGENT){
			BuildCubeCollision();
			BuildIntelligetnSphere();
		}
	
	
}

/*
This function simply turns the orientation and position
of our physics node into a transformation matrix, suitable
for plugging into our Renderer!

It is cleaner to work with matrices when it comes to rendering,
as it is what shaders expect, and allow us to keep all of our
transforms together in a single construct. But when it comes to
physics processing and 'game-side' logic, it is much neater to
have seperate orientations and positions.

*/
Matrix4		PhysicsNode::BuildTransform() {
	Matrix4 m = m_orientation.ToMatrix();
	m.SetPositionVector(m_position);
	return m;
}

void PhysicsNode::ExplicitEuler(Vector3 acceleration, float msec){
	
	m_linearVelocity = m_linearVelocity + acceleration * msec;
	if(m_type == CUBE){
		if(m_linearVelocity.x < 0.001f || 
			m_linearVelocity.y < 0.001f || 
			m_linearVelocity.z < 0.001f){
				m_linearVelocity = Vector3();
		}
	}

	if(abs(m_linearVelocity.x) < 0.001f)
		m_linearVelocity.x = 0.0f;
	if(abs(m_linearVelocity.y) < 0.001f)
		m_linearVelocity.y = 0.0f;
	if(abs(m_linearVelocity.z) < 0.001f)
		m_linearVelocity.z = 0.0f;
		
	m_position = m_position + m_linearVelocity * msec;
	
}

void PhysicsNode::VerletIntegration(Vector3 acceleration, float msec){
	Vector3 NextPosition = m_position + (m_position - m_last_position) + (acceleration) * msec * msec;
	m_last_position = m_position;
	SetPosition(NextPosition);
	m_linearVelocity = (m_position - m_last_position) / msec;
}

Vector3 PhysicsNode::calculateAcceleration (){
	Vector3 result = m_force * m_invMass;
	if(toggle_gravity)
		result = result + GRAVITY ;
	return result;
}

void PhysicsNode::SetInertiaMatricesSphere(float radius, float mass){
	float i = (2 * mass * (radius * radius) )/5.0f;
	inertia_matrix.SetBalanceInertiaMatrix(i, i, i);
	inverse_inertia_matrix = inertia_matrix.Inverse();
	d = Vector3(radius / 2, radius / 2, radius / 2);
}

void PhysicsNode::SetInertiaMatricesCuboid(float length, float height, float width, float mass){
	float xx = (mass * ((height * height) + (width * width))) / 12.0f;
	float yy = (mass * ((length * length) + (width * width))) / 12.0f;
	float zz = (mass * ((height * height) + (length * length))) / 12.0f;
	inertia_matrix.SetBalanceInertiaMatrix(xx, yy, zz);
	inverse_inertia_matrix = inertia_matrix.Inverse();
	// A little bit of cheating, we are supposing that every push is made on the half of the object
	d = Vector3 (length / 2, height / 2, width / 2);
}

void PhysicsNode::SetInertiaMatricesCylinder(float radius, float height, float mass){
	float xx = (mass / 12.0f) * ((3 * (radius * radius)) + (height * height));
	float yy = xx;
	float zz = (mass * (radius * radius)) / 2.0f;
	inertia_matrix.SetBalanceInertiaMatrix(xx, yy, zz);
	inverse_inertia_matrix = inertia_matrix.Inverse();
	//We are supposing that the force applied was on the middle of the object
	d = Vector3(height /2, height / 2, height / 2);
}

void PhysicsNode::AngularVelocity(float msec){
	//Step1: torque of the object
	Vector3 m_torque =  Vector3::Cross(d, m_force );

	//Step 2 calculate the angular aceleration
	Vector3 angular_acceleration = inverse_inertia_matrix * m_torque;

	//Step 3 the angular velocity
	m_angularVelocity = m_angularVelocity + angular_acceleration * msec;
	if(m_type == CUBE){
		if(m_angularVelocity.x < 0.0000001 ||
			m_angularVelocity.y < 0.0000001 || 
			m_angularVelocity.z < 0.0000001){
				m_angularVelocity = Vector3();
			
		}
	}
	m_orientation = m_orientation + (m_orientation * (m_angularVelocity * (msec/2)));
	m_orientation.Normalise();
	
}

void PhysicsNode::Stop(){
	m_linearVelocity = Vector3(0,0,0);
	m_angularVelocity = Vector3(0,0,0);
	angularMotion = false;
	linearMotion = false;
}

void PhysicsNode::BuildSphereCollisionSphere(){
	CollisionSphere c(GetPosition(), radius);
	SetCollisionSphere(c);
}

void PhysicsNode::BuildIntelligetnSphere(){
	CollisionSphere c (GetPosition(), radius * 2.0f);
	SetCollisionSphere(c);
}

void PhysicsNode::BuildCubeCollision(){
	collisionCube = CollisionAABB(Vector3(m_position.x, m_position.y, m_position.z),radius, radius, radius);
}

void PhysicsNode::putRest(bool val){
	rest = val;
	if(val){
		Stop();
	} else {
		angularMotion = true;
		linearMotion = true;
		setGravity(true);
	}
}

void PhysicsNode::setType(int id){
	switch(id){
		case 0:
			m_type = CYLINDER;
			SetMass(0.0f);
			inverse_inertia_matrix.SetBalanceInertiaMatrix(0,0,0);
			break;
		case 1:
			m_type = SPHERE;
			BuildSphereCollisionSphere();
			SetInertiaMatricesSphere(radius, m_Mass);
			break;
		case 2:
			m_type = CUBE;
			BuildCubeCollision();
			SetInertiaMatricesCuboid(radius, radius, radius, m_Mass);
			break;
		case 3:
			m_type = PLANE;
			m_plane = new Plane();
			SetMass(0.0f);
			inverse_inertia_matrix.SetBalanceInertiaMatrix(0,0,0);
			break;
		case 4:
			m_type = IMAGINARY;
			SetMass(0.0f);
			inverse_inertia_matrix.SetBalanceInertiaMatrix(0,0,0);
			break;
		case 5:
			m_type = INTELLIGENT;
			BuildCubeCollision();
			BuildIntelligetnSphere();
			SetInertiaMatricesCuboid(radius, radius, radius, m_Mass);
			break;
	}
}

void PhysicsNode::SetMass(float m){
	if(m > 0){
		m_Mass = m;
		m_invMass = 1.0f / m_Mass;
	}else {
		m_Mass = 0;
		m_invMass = 0;
	}
	if(m_type == SPHERE)
		SetInertiaMatricesSphere(radius, m_Mass);
	else if (m_type == CUBE)
		SetInertiaMatricesCuboid(radius, radius, radius, m_Mass);
}