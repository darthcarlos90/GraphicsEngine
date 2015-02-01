/******************************************************************************
Class:MyGame
Implements:GameEntity
Author:Rich Davison	<richard.davison4@newcastle.ac.uk> and YOU!
Description: This is an example of a derived game class - all of your game 
specific things will go here. Such things would include setting up the game 
environment (level, enemies, pickups, whatever), and keeping track of what the
gameplay state is (Is the player dead? Has he/she won the game? And so on).

To get you started, there's a few static functions included that will create 
create some simple game objects - ideally your game object loading would be
handled by config files or somesuch, but for a simple demo game, these 
functions are a good start. Don't be afraid to modify/delete/add more of these!

Finally, there's some calls to the OGLRenderer debug draw functions that have
been added to the course this year. These are really useful for debugging the
state of your game in a graphical way - i.e drawing a line from a character
to the item it is aiming at, or pointing in the direction it is moving, or
for drawing debug bounding volumes to see when objects collide. Feel free
to add more of these debug functions - how they are implemented is covered
in the code definitions for them, and also in the extra introduction 
document on the research website.

Have fun!


-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////



#pragma once
#include "GameEntity.h"
#include "GameClass.h"
#include "../nclgl/Camera.h"
#include "../nclgl/CubeRobot.h"
#include "../nclgl/HeightMap.h"
#include "Tree.h"
#include "IntelligentEntity.h"

class MyGame : public GameClass	{
public:
	MyGame(bool debugOptions = false);
	~MyGame(void);

	virtual void UpdateGame(float msec);

protected:
	GameEntity* BuildRobotEntity();

	void BuildTree();

	GameEntity* BuildCubeEntity(float size, Vector3 position);

	GameEntity* BuildSphereEntity(float radius, Vector3 position, Vector3 force, Vector4 color);

	GameEntity* BuildQuadEntity(float size);

	GameEntity* BuildHeightMap();
	//Builds a zombie
	IntelligentEntity* BuildZombie(float size, Vector3 position);
	//Builds a sphere in state of rest
	GameEntity* BuildStaticSphere(float radius, Vector3 position);
	//builds a stone (unamovible object)
	GameEntity* BuildStone(float radius, Vector3 position);
	//creates the secondary weapons
	GameEntity* BuildMeatball(Vector3 position, Vector3 force);
	GameEntity* BuildGreanade(Vector3 position, Vector3 force);
	//creates the spring
	void doSpring();

	//This constructos build the elements in the engine
	//Debugging only builds the heightmap and the ball shooting
	void debuggingConstructor();
	//The "with Everything" means that the tree, targets, scores, and the AI is also included
	void withEverythingConstructor();
	
	//changes the type of weapon
	void toggleWeapon() { if(current_weapon <1) current_weapon++; else current_weapon = 0; }
	inline void increaseSpeed() { 
		if(SpeedFactor < 20.0f)
			SpeedFactor ++;
		Renderer::GetRenderer().setSpeed(SpeedFactor);
	}
	inline void decreaseSpeed() { 
		if(SpeedFactor > 2.0f)
			SpeedFactor --; 
		Renderer::GetRenderer().setSpeed(SpeedFactor);

	}
	inline void increaseSize() { 
		if(size_proyectile < 40.0f)
			size_proyectile++; 
		Renderer::GetRenderer().setBallSize(size_proyectile);
	}
	inline void decreaseSize() { 
		if(size_proyectile > 2.0f)
			size_proyectile--; 
		Renderer::GetRenderer().setBallSize(size_proyectile);
	}
	


	Mesh* cube;
	Mesh* quad;
	Mesh* sphere;
	HeightMap* heightMap;
	Tree * tree;
	vector <IntelligentEntity*> zombies;
	vector <Vector3> respawn;
	Vector3 meatlocation;
	
	int greanades;
	int greanade_index;
	int meat;
	int meatIndex;
	int current_weapon;
	float respawnTimer;
	bool waitRespawn;
	float size_proyectile;
	float SpeedFactor;

	bool debugging;

};

