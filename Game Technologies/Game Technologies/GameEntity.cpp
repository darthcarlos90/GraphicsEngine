#include "GameEntity.h"
#include "Renderer.h"
#include "PhysicsSystem.h"

GameEntity::GameEntity(void)	{
	renderNode	= NULL;
	physicsNode = NULL;
	inUse = false;
	disconnected = false;
	isMeat = false;
}

GameEntity::GameEntity(SceneNode* s, PhysicsNode* p) {
	renderNode	= s;
	physicsNode = p;
	inUse = false;
	disconnected = false;
	isMeat = false;
}

GameEntity::~GameEntity(void)	{
	if(!disconnected){
		DisconnectFromSystems();

		delete renderNode;
		delete physicsNode;
	}
}

void GameEntity::dissappear(){
	DisconnectFromSystems();
		disconnected = true;
}

void	GameEntity::Update(float msec) {
	Vector3 position = physicsNode->GetPosition();
	//if the element is out of screen, eliminate it
	if(!disconnected){
		if(position.x >= OCTREE_SIZES ||
			position.y >= OCTREE_SIZES ||
			position.z >= OCTREE_SIZES){
				dissappear();
		}
	}

	if(!physicsNode->getUsing()){
		dissappear();
	}

}

void	GameEntity::ConnectToSystems() {
	if(renderNode) {
		Renderer::GetRenderer().AddNode(renderNode);
	}

	if(physicsNode) {
		PhysicsSystem::GetPhysicsSystem().AddNode(physicsNode);
	}

	if(renderNode && physicsNode) {
		physicsNode->SetTarget(renderNode);
	}
}

void	GameEntity::DisconnectFromSystems() {
	if(renderNode) {
		Renderer::GetRenderer().RemoveNode(renderNode);
	}

	if(physicsNode) {
		PhysicsSystem::GetPhysicsSystem().RemoveNode(physicsNode);
	}
}