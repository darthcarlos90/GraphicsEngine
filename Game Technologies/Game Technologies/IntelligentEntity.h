/**
	Class: Intelligent Entity
	Inherits: Game Entity
	Description:
		This class represents the zombie squares in the environment. This class handles the state machine of 
		the "decisions" each zombie takes.
**/
#pragma once
#include "GameEntity.h"
#include "ZombieState.h"

class IntelligentEntity: public GameEntity {
public:
	IntelligentEntity(SceneNode* sn, PhysicsNode* pn, float size);
	~IntelligentEntity(void){};
	inline ZombieState returnCurrentState() { return currentState; }
	bool isDead() { return currentState.isDead(); }
	void UpdateState();
	virtual void Update (float msec);
	void addBrother (IntelligentEntity* b) { brothers.push_back(b); }
	void newMeat(Vector3 position);
	void meatDissappeared() { meatKnown = false; }
	void evadeObject(PhysicsNode& pn);
	Vector3 getActualTarget() { return target; }
	bool following_meat() { return meatKnown; }
	void setCamera(Vector3 val) { camera = val; }
	
	

private:
	void Move(Vector3 position);

	vector<IntelligentEntity*> brothers;
	ZombieState currentState;
	ZombieState wander;
	ZombieState evade;
	ZombieState hunt;
	ZombieState dead;
	ZombieState huntCamera;
	

	Vector3 originalPosition;
	Vector3 toMove;
	Vector3 target;
	Vector3 camera;

	int hits;
	bool shoot_at;
	int direction;
	int count;
	bool meatKnown;
};