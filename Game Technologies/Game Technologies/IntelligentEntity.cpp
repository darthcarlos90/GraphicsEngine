/**
	Class that represents the intelligent being
**/

#include "IntelligentEntity.h"
#include "PhysicsSystem.h"
#include "Renderer.h"

IntelligentEntity::IntelligentEntity(SceneNode* sn, PhysicsNode* pn, float size):GameEntity(sn, pn){
	wander.SetProperties(true, false, false, false,  false, false);
	evade.SetProperties(false, true, false, false,  false, false);
	hunt.SetProperties(false, false,true, false,  false, false);
	dead.SetProperties(false, false, false, true,  false, false);
	huntCamera.SetProperties(false, false, false, false,false, true);
	
	hits = 0;
	shoot_at = false;
	count = 0;
	UpdateState();
	meatKnown = false;
	
	currentState = wander;
	
	
	GetPhysicsNode().SetLinearMotion();
	GetPhysicsNode().SetAngularMotion();
	GetPhysicsNode().putRest(true);

	GetPhysicsNode().setGravity(false);
	SceneNode &test = GetRenderNode();
	test.SetModelScale(Vector3(size,size,size));
	test.SetBoundingRadius(size);
	test.SetColour(Vector4(1, 1, 1, 1));
	

	originalPosition = pn->GetPosition();
	ConnectToSystems();
}


//Update the state of the zombie depending on what is happening
void IntelligentEntity::UpdateState(){
	if(hits > 0){
		currentState = dead;
		return;
	}else if(shoot_at){
		currentState = evade;
		return;
	} else if(meatKnown){
		currentState = hunt;
		return;
	}else{
		for(unsigned int i = 0; i < brothers.size(); i++){
			if(brothers[i]->isDead()){
				currentState = huntCamera;
				return;
			}
		}

		currentState = wander;
		return;
	}
}

//if there is a new meatball in the field
void IntelligentEntity::newMeat( Vector3 position){
	meatKnown = true;
	target = position;
}


//method to evade the element being fired at it, it is also used 
//at flocking so each of the elements wont occupy the same position
void IntelligentEntity::evadeObject(PhysicsNode &pn){
	Vector3 bulletPosition = pn.getBulletPosition();	
	Vector3 newPosition;
	hits = pn.getHits();
	if(bulletPosition.x < pn.GetPosition().x)
		newPosition.x = 10;
	else newPosition.x = -10;

	if(bulletPosition.y < pn.GetPosition().y)
		newPosition.y = 10;
	else newPosition.y = -10;

	if(bulletPosition.z < pn.GetPosition().z)
		newPosition.z = 10;
	else newPosition.z = -10;
	pn.SetPosition(pn.GetPosition() + newPosition);
	shoot_at = false;
	pn.setShootAt(false);
}

//update method
void IntelligentEntity::Update(float msec){
	PhysicsNode & pn = GetPhysicsNode();
	if(pn.getShootAt()){
		shoot_at = true;
	}
	UpdateState();
	direction = rand();
	if(currentState.isWandering()){
		Vector3 old_pos = pn.GetPosition();
		if(old_pos.x != originalPosition.x){
			if(old_pos.y != originalPosition.y){
				if(old_pos.z != originalPosition.z){
					Move(originalPosition);
				}
			}
		} 

	} else if(currentState.isEvading()){
		//MOVE
		evadeObject(pn);
	} else if(currentState.isHunting()){
		Move(target);
	} else if(currentState.isDead()){
		Vector3 position = pn.GetPosition();
		Move(Vector3(position.x, 0.0f, position.z));
	} else if(currentState.isHuntingCamera()){
		Move(camera);
	}

	GameEntity::Update(msec);
}

//move this element towards an element
void IntelligentEntity::Move(Vector3 position){
	PhysicsNode& pn = GetPhysicsNode();
	Vector3 myPosition = pn.GetPosition();
	Vector3 resultPosition;
	if(position.x != myPosition.x) {
		if(position.x < myPosition.x){
			resultPosition.x = myPosition.x - 0.4f;
		} else resultPosition.x = myPosition.x + 0.4f;
	} else {
		resultPosition.x = myPosition.x;
	}
	
	if(position.y != myPosition.y){
		if(position.y < myPosition.y){
			resultPosition.y = myPosition.y - 0.4f;
		} else resultPosition.y = myPosition.y + 0.4f;
	} else {
		resultPosition.y = myPosition.y;
	}

	if(position.z != myPosition.z){
		if(position.z < myPosition.z){
			resultPosition.z = myPosition.z - 0.4f;
		} else resultPosition.z = myPosition.z + 0.4f;
	}else {
		resultPosition.z = myPosition.z;
	}

	pn.SetPosition(resultPosition);
	
}