#include "MyGame.h"
#include "Branch.h"
#include "Sphere.h"
#include "Spring.h"
#include "SpringDemo.h"

/*
Creates a really simple scene for our game - A cube robot standing on
a floor. As the module progresses you'll see how to get the robot moving
around in a physically accurate manner, and how to stop it falling
through the floor as gravity is added to the scene. 

You can completely change all of this if you want, it's your game!

*/
MyGame::MyGame()	{
	gameCamera = new Camera(-30.0f,0.0f,Vector3(0,450,850));

	Renderer::GetRenderer().SetCamera(gameCamera);

	CubeRobot::CreateCube();
	Tree::CreateCylinder();
	Branch::CreateCylinder();
	Sphere::CreateSphere();
	
	

	/*
	We're going to manage the meshes we need in our game in the game class!

	You can do this with textures, too if you want - but you might want to make
	some sort of 'get / load texture' part of the Renderer or OGLRenderer, so as
	to encapsulate the API-specific parts of texture loading behind a class so
	we don't care whether the renderer is OpenGL / Direct3D / using SOIL or 
	something else...
	*/
	cube	= new OBJMesh(MESHDIR"cube.obj");
	quad	= Mesh::GenerateQuad();
	sphere	= new OBJMesh(MESHDIR"ico.obj");
	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	tree = NULL;
	meatIndex = -1;
	meat = 1;
	greanades = 3;
	current_weapon = 0;
	respawnTimer = 0.0f;
	waitRespawn = false;
	SpeedFactor = 10.0f;
	size_proyectile = 10.0f;

	Renderer::GetRenderer().setMeatballs(meat);
	Renderer::GetRenderer().setGreanades(greanades);
	Renderer::GetRenderer().setCurrent(current_weapon);

	//draw the springs
	doSpring();
	/*
	A more 'robust' system would check the entities vector for duplicates so as
	to not cause problems...why not give it a go?
	*/
	//allEntities.push_back(BuildQuadEntity(1000.0f));

	//create the heightmap and zombies
	//TODO Add heightmap collision at some point
	//it is 12/12/2014 8.30 am and there is still no heightmap collision
	allEntities.push_back(BuildHeightMap());

	//build the zombies
	allEntities.push_back(BuildZombie(30.0f, Vector3 (0.0f,360.0f,0.0f)));
	allEntities.push_back(BuildZombie(30.0f, Vector3 (200.0f,360.0f,-300.0f)));
	allEntities.push_back(BuildZombie(30.0f, Vector3 (-400.0f,360.0f,500.0f)));
	allEntities.push_back(BuildZombie(30.0f, Vector3 (400.0f,360.0f,-300.0f)));
	allEntities.push_back(BuildZombie(30.0f, Vector3 (-200.0f,360.0f,100.0f)));
	allEntities.push_back(BuildZombie(30.0f, Vector3 (-680.0f,360.0f,700.0f)));
	allEntities.push_back(BuildZombie(30.0f, Vector3 (10.0f,360.0f,900.0f)));
	allEntities.push_back(BuildZombie(30.0f, Vector3 (487.56f,360.0f,696.0f)));
	allEntities.push_back(BuildZombie(30.0f,Vector3	(500.0,360,100.0)));
	
	float radius = 30.0f;
	Vector3 position(800, 500, -300);
	for(int i = 0; i < 5; i++){
		allEntities.push_back(BuildStaticSphere(30.0f, position));
		position.x += (radius * 2) + 0.05f;
	}
	position = Vector3(800, 500, -300 + (radius * 2.0f));
	//Build the static spheres
	for(int i = 0; i < 5; i++){
		allEntities.push_back(BuildStaticSphere(30.0f, position));
		position.x += (radius * 2) + 0.05f;
	}
	//build the stones
	allEntities.push_back(BuildStone(30.0f, Vector3(170, 360, -630)));
	allEntities.push_back(BuildStone(30.0f, Vector3(170, 360, 630)));
	allEntities.push_back(BuildStone(30.0f, Vector3(-170, 360, -630)));
	allEntities.push_back(BuildStone(30.0f, Vector3(-170, 360, 630)));
	
	BuildTree();
	
}

MyGame::~MyGame(void)	{
	/*
	We're done with our assets now, so we can delete them
	*/
	delete cube;
	delete quad;
	delete sphere;
	delete heightMap;
	//delete tree;
	CubeRobot::DeleteCube();
	Tree::DeleteCylinder();
	Branch::DeleteCylinder();
	Sphere::DeleteSphere();

	//GameClass destructor will destroy your entities for you...
}

/*
Here's the base 'skeleton' of your game update loop! You will presumably
want your games to have some sort of internal logic to them, and this
logic will be added to this function.
*/
void MyGame::UpdateGame(float msec) {
	if(gameCamera) {
		gameCamera->UpdateCamera(msec);
	}
	
	for(vector<GameEntity*>::iterator i = allEntities.begin(); i != allEntities.end(); ++i) {
		(*i)->Update(msec);
	}

	


	/*
	Here's how we can use OGLRenderer's inbuilt debug-drawing functions! 
	I meant to talk about these in the graphics module - Oops!

	We can draw squares, lines, crosses and circles, of varying size and
	colour - in either perspective or orthographic mode.

	Orthographic debug drawing uses a 'virtual canvas' of 720 * 480 - 
	that is 0,0 is the top left, and 720,480 is the bottom right. A function
	inside OGLRenderer is provided to convert clip space coordinates into
	this canvas space coordinates. How you determine clip space is up to you -
	maybe your renderer has getters for the view and projection matrix?

	Or maybe your Camera class could be extended to contain a projection matrix?
	Then your game would be able to determine clip space coordinates for its
	active Camera without having to involve the Renderer at all?

	Perspective debug drawing relies on the view and projection matrices inside
	the renderer being correct at the point where 'SwapBuffers' is called. As
	long as these are valid, your perspective drawing will appear in the world.

	This gets a bit more tricky with advanced rendering techniques like deferred
	rendering, as there's no guarantee of the state of the depth buffer, or that
	the perspective matrix isn't orthographic. Therefore, you might want to draw
	your debug lines before the inbuilt position before SwapBuffers - there are
	two OGLRenderer functions DrawDebugPerspective and DrawDebugOrtho that can
	be called at the appropriate place in the pipeline. Both take in a viewProj
	matrix as an optional parameter.

	Debug rendering uses its own debug shader, and so should be unaffected by
	and shader changes made 'outside' of debug drawing

	*/
	//Lets draw a box around the cube robot!
	//Renderer::GetRenderer().DrawDebugBox(DEBUGDRAW_PERSPECTIVE, Vector3(0,51,0), Vector3(100,100,100), Vector3(1,0,0));

	////We'll assume he's aiming at something...so let's draw a line from the cube robot to the target
	////The 1 on the y axis is simply to prevent z-fighting!
	//Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(0,160,-4),Vector3(0,160,8), Vector3(0,0,1), Vector3(1,0,0));

	////Maybe he's looking for treasure? X marks the spot!
	//Renderer::GetRenderer().DrawDebugCross(DEBUGDRAW_PERSPECTIVE, Vector3(0, 113.2982387, 28.77523287),Vector3(50,50,50), Vector3(0,0,0));

	////CubeRobot is looking at his treasure map upside down!, the treasure's really here...
	//Renderer::GetRenderer().DrawDebugCircle(DEBUGDRAW_PERSPECTIVE, Vector3(-200,1,-200),50.0f, Vector3(0,1,0));

	//if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)){

	//respawn timer
	if(waitRespawn){
		respawnTimer += msec;
	}
	//respawn the elements destroyed in their position
	if(respawn.size() > 0){
		if(respawnTimer > 10000.0f){
			allEntities.push_back(BuildStone(30.0f, respawn[0]));
			respawn.erase(respawn.begin());
			respawnTimer = 0.0f;
		}
	}

	//left mouse shoot!
	if(Window::GetMouse()->clickTriggered(MOUSE_LEFT)){
		Matrix4 force = gameCamera->BuildViewMatrix();
		allEntities.push_back(BuildSphereEntity(2.5f * size_proyectile, gameCamera->GetPosition(), Vector3(-force.values[2]/(0.50f * SpeedFactor), -force.values[6]/(0.50f * SpeedFactor), -force.values[10]/(0.50f * SpeedFactor)), Vector4(0,0,1,1)));
		
	}

	//right mouse special weapon
	if(Window::GetMouse()->clickTriggered(MOUSE_RIGHT)){
		switch(current_weapon){
			case 0:
				if(meat > 0){
					Matrix4 force = gameCamera->BuildViewMatrix();
					allEntities.push_back(BuildMeatball( gameCamera->GetPosition(), Vector3(-force.values[2]/(2.0f * SpeedFactor), -force.values[6]/(2.0f * SpeedFactor), -force.values[10]/(2.0f * SpeedFactor))));
					meatIndex = allEntities.size();
				}
				break;
			case 1:
				if(greanades > 0){
					Matrix4 force = gameCamera->BuildViewMatrix();
					allEntities.push_back(BuildGreanade(gameCamera->GetPosition(), Vector3(-force.values[2]/(0.50f * SpeedFactor), -force.values[6]/(0.50f * SpeedFactor), -force.values[10]/(0.50f * SpeedFactor))));
					greanade_index = allEntities.size();
				}
		}
	}

	//keyboard 1 tree grow faster
	if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_1)){
		tree->toggleFaster();
	}
	//keyboard 2 tree grow slower
	if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_2)){
		tree->toggleSlower();
	}
	//change secondary weapon
	if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_3)){
		toggleWeapon();
	}
	//increase force of launching
	if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_4)){
		increaseSpeed();
	}
	//decrease the force of launching
	if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_5)){
		decreaseSpeed();
	}
	//increase size of ball
	if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_6)){
		increaseSize();
	}
	//decrease size of ball
	if(Window::GetKeyboard()->KeyTriggered(KEYBOARD_7)){
		decreaseSize();
	}


	//when there is an explosion, draw many balls
	for(unsigned int i = 0; i < allEntities.size(); i++){
		if(allEntities[i]->GetPhysicsNode().isBroken()){
			
			Vector3 position = allEntities[i]->GetPhysicsNode().GetPosition();
			if(allEntities[i]->GetPhysicsNode().getGreanade()){
				allEntities.push_back(BuildSphereEntity(10.0f, Vector3(position.x + 20.0f, position.y, position.z), Vector3(0.1f, 0.0f, 0.0f), Vector4(0.2f,1.0f,0.5f,1.0f)));
				allEntities.push_back(BuildSphereEntity(10.0f, Vector3(position.x - 20.0f, position.y, position.z), Vector3(-0.1f, 0.0f, 0.0f), Vector4(0.2f,1,0.5,1)));
				allEntities.push_back(BuildSphereEntity(10.0f, Vector3(position.x, position.y + 20.0f, position.z), Vector3(0.0f, 0.1f, 0.0f), Vector4(0.2f,1,0.5,1)));
				allEntities.push_back(BuildSphereEntity(10.0f, Vector3(position.x, position.y - 20.0f, position.z), Vector3(0.0f,-0.1f, 0.0f), Vector4(0.2f,1,0.5,1)));
				allEntities.push_back(BuildSphereEntity(10.0f, Vector3(position.x, position.y, position.z + 20.0f), Vector3(0.0f, 0.0f, 0.1f), Vector4(0.2f,1,0.5,1)));
				allEntities.push_back(BuildSphereEntity(10.0f, Vector3(position.x , position.y, position.z - 20.0f), Vector3(0.0f, 0.0f,-0.1f), Vector4(0.2f,1,0.5,1)));
			}else {
				allEntities.push_back(BuildSphereEntity(20.0f, Vector3(position.x + 20.0f, position.y, position.z), Vector3(0.1f, 0.0f, 0.0f), Vector4(0,1,0,1)));
				allEntities.push_back(BuildSphereEntity(20.0f, Vector3(position.x - 20.0f, position.y, position.z), Vector3(-0.1f, 0.0f, 0.0f), Vector4(0,0,1,1)));
				allEntities.push_back(BuildSphereEntity(20.0f, Vector3(position.x, position.y + 20.0f, position.z), Vector3(0.0f, 0.1f, 0.0f), Vector4(0,1,1,1)));
				allEntities.push_back(BuildSphereEntity(20.0f, Vector3(position.x, position.y - 20.0f, position.z), Vector3(0.0f,-0.1f, 0.0f), Vector4(0.2f,0.5f,0,1)));
				allEntities.push_back(BuildSphereEntity(20.0f, Vector3(position.x, position.y, position.z + 20.0f), Vector3(0.0f, 0.0f, 0.1f), Vector4(0,0.3f,0.5f,1)));
				allEntities.push_back(BuildSphereEntity(20.0f, Vector3(position.x , position.y, position.z - 20.0f), Vector3(0.0f, 0.0f,-0.1f), Vector4(0.5,0.5,0.5,1)));
				respawn.push_back(position);
				waitRespawn = true;
			}
			
			allEntities[i]->dissappear();
			allEntities.erase(allEntities.begin() + i);
		}

		
	}

	//if there is meat, set the zombies to look for it
	if(meatIndex > -1){
		meatlocation = allEntities[meatIndex - 1]->GetPhysicsNode().GetPosition();
		for(unsigned int i = 0; i < zombies.size(); i++){
			if (meatlocation.y >= 0.0f){
				
				zombies[i]->newMeat(meatlocation);
			} else {
			zombies[i]->meatDissappeared();
			}
		}
	}

	//keep the zombies updated where the camera is
	for(unsigned int i = 0; i < zombies.size(); i++){
		zombies[i]->setCamera(gameCamera->GetPosition());
	}
	
	//if the zombies attacked you, decrease your score
	for(unsigned int i = 0; i < zombies.size(); i++){
		Vector3 zombiePosition = zombies[i]->GetPhysicsNode().GetPosition();
		Vector3 cameraPosition = gameCamera->GetPosition();
		if(zombiePosition.x == cameraPosition.x){
			if(zombiePosition.y == cameraPosition.y){
				if(zombiePosition.z == cameraPosition.z){
					Renderer::GetRenderer().decreaseScore();
				}
			}
		}
	}
	//set the counters for display
	Renderer::GetRenderer().setMeatballs(meat);
	Renderer::GetRenderer().setGreanades(greanades);
	Renderer::GetRenderer().setCurrent(current_weapon);
	
	//draw a debug line
	PhysicsSystem::GetPhysicsSystem().DrawDebug();
}



/*
Makes an entity that looks like a CubeRobot! You'll probably want to modify
this so that you can have different sized robots, with different masses and
so on!
*/
GameEntity* MyGame::BuildRobotEntity() {
	GameEntity*g = new GameEntity(new CubeRobot(), new PhysicsNode());
	g->ConnectToSystems();
	g->GetPhysicsNode().SetPosition(Vector3(-1000, 0, 0));
	g->GetPhysicsNode().SetCollisionSphere(CollisionSphere(g->GetPhysicsNode().GetPosition(), 70.0f));
	g->GetPhysicsNode().SetLinearMotion();
	return g;
}

//handles the tree building
void MyGame::BuildTree(){
	tree = new Tree(2, Vector3(-400,100,-400));
	GameEntity *g = new GameEntity(tree, new PhysicsNode());
	g->ConnectToSystems();
	g->GetPhysicsNode().Stop();
	g->GetPhysicsNode().setType(4);
	g->GetPhysicsNode().putRest(true);
	allEntities.push_back(g);
	
}

//creates the zombies and adds them textures :)
IntelligentEntity* MyGame::BuildZombie(float size, Vector3 position){
	cube->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"zombie.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	IntelligentEntity* ie = new IntelligentEntity(new SceneNode(cube), new PhysicsNode(position, size, 5, 100.0f), size);
	zombies.push_back(ie);
	if(zombies.size() > 0){
		for(unsigned int i = 0; i < zombies.size(); i++){
			zombies[i]->addBrother(ie);
			ie->addBrother(zombies[i]);
		}
	}
	return ie;
	
}

/*
Makes a cube. Every game has a crate in it somewhere!
*/
GameEntity* MyGame::BuildCubeEntity(float size, Vector3 position) {
	
	GameEntity*g = new GameEntity(new SceneNode(cube), new PhysicsNode(position, size, 2, 100.0f));
	g->ConnectToSystems();
	g->GetPhysicsNode().SetLinearMotion();
	g->GetPhysicsNode().SetAngularMotion();
	g->GetPhysicsNode().putRest(true);

	g->GetPhysicsNode().setGravity(false);
	
	SceneNode &test = g->GetRenderNode();
	test.SetModelScale(Vector3(size,size,size));
	test.SetBoundingRadius(size);
	test.SetColour(Vector4(1, 1, 1, 1));
	

	return g;
}

//builds a sphere in state of rest
GameEntity* MyGame::BuildStaticSphere( float radius, Vector3 position){
	SceneNode* s = new SceneNode(sphere);
	s->SetModelScale(Vector3(radius,radius,radius));
	s->SetBoundingRadius(radius);
	s->SetColour(Vector4(1.0f,0.8f,0.6f,1.0f));

	GameEntity*g = new GameEntity(s, new PhysicsNode(position, radius, 1, 2.0f));
	g->GetPhysicsNode().SetLinearMotion();
	g->GetPhysicsNode().SetAngularMotion();
	g->GetPhysicsNode().putRest(true);
	g->GetPhysicsNode().setGravity(false);
	g->ConnectToSystems();
	return g;
}

//builds a sphere that is inamovible
GameEntity* MyGame::BuildStone(float radius, Vector3 position){
	SceneNode* s = new SceneNode(sphere);
	s->SetModelScale(Vector3(radius,radius,radius));
	s->SetBoundingRadius(radius);
	s->SetColour(Vector4(1,1,1,1));

	GameEntity*g = new GameEntity(s, new PhysicsNode(position, radius, 1, 0.0f));
	g->GetPhysicsNode().SetLinearMotion();
	g->GetPhysicsNode().SetAngularMotion();
	g->GetPhysicsNode().putRest(true);
	g->GetPhysicsNode().setGravity(false);
	g->GetPhysicsNode().SetStone(true);
	g->ConnectToSystems();
	return g;
}

//greanade creating
GameEntity* MyGame::BuildGreanade(Vector3 position, Vector3 force){
	SceneNode* s = new SceneNode(sphere);
	s->SetModelScale(Vector3(30.0f,30.0f,30.0f));
	s->SetBoundingRadius(30.0f);
	s->SetColour(Vector4(0.2f,1.0f,0.5f,1.0f));
	
	
	GameEntity*g = new GameEntity(s, new PhysicsNode(position, 30.0f, 1));
	g->GetPhysicsNode().setForce(force);
	g->GetPhysicsNode().SetLinearMotion();
	g->GetPhysicsNode().SetAngularMotion();
	g->GetPhysicsNode().SetGreanade(true);
	g->ConnectToSystems();
	

	greanades--;
	return g;
}



/*
Makes a sphere.
*/
GameEntity* MyGame::BuildSphereEntity(float radius, Vector3 position, Vector3 force, Vector4 color) {
	SceneNode* s = new SceneNode(sphere);
	s->SetModelScale(Vector3(radius,radius,radius));
	s->SetBoundingRadius(radius);
	s->SetColour(color);
	
	GameEntity*g = new GameEntity(s, new PhysicsNode(position, radius, 1));
	g->GetPhysicsNode().setForce(force);
	g->GetPhysicsNode().SetLinearMotion();
	g->GetPhysicsNode().SetAngularMotion();
	g->ConnectToSystems();
	return g;
}

//create the meatball
GameEntity* MyGame:: BuildMeatball(Vector3 position, Vector3 force){
	
	SceneNode* s = new SceneNode(sphere);
	s->SetModelScale(Vector3(30.0f,30.0f,30.0f));
	s->SetBoundingRadius(30.0f);
	s->SetColour(Vector4(1,0,0,1));
	
	
	GameEntity*g = new GameEntity(s, new PhysicsNode(position, 30.0f, 1));
	g->GetPhysicsNode().setForce(force);
	g->GetPhysicsNode().SetLinearMotion();
	g->GetPhysicsNode().SetAngularMotion();
	g->ConnectToSystems();

	g->setMeat(true);
	meat--;
	return g;
}



/*
Makes a flat quad, initially oriented such that we can use it as a simple
floor. 
*/
GameEntity* MyGame::BuildQuadEntity(float size) {
	SceneNode* s = new SceneNode(quad);

	s->SetModelScale(Vector3(size,size,size));
	//Oh if only we had a set texture function...we could make our brick floor again WINK WINK
	s->SetBoundingRadius(size);
	s->SetColour(Vector4(1,1,0,1));
	
	PhysicsNode*p = new PhysicsNode(Quaternion::AxisAngleToQuaterion(Vector3(1,0,0), 90.0f), Vector3(),3);

	GameEntity*g = new GameEntity(s, p);
	g->ConnectToSystems();
	return g;
}

//create the heoightmap
GameEntity* MyGame::BuildHeightMap(){
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"sand_texture.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	SceneNode* s = new SceneNode(heightMap);
	s->setHeightMap(true);
	
	PhysicsNode*p = new PhysicsNode(Vector3(-(RAW_WIDTH * HEIGHTMAP_X/2.0f), 0.0f, -(RAW_HEIGHT * HEIGHTMAP_Z)/2.0f),0.0f, 3, 0.0f);
	p->putRest(true);
	
	GameEntity *g = new GameEntity(s, p);
	g->ConnectToSystems();
	return g;

}
//show the spring on screen
void MyGame::doSpring(){
	
	GameEntity* ball0 = BuildSphereEntity(15.0f, Vector3(-300, 800, -100), Vector3(0, 0, 0), Vector4(1,0,0,1));
	ball0->GetPhysicsNode().putRest(true);
	ball0->GetPhysicsNode().setGravity(false);
	ball0->GetPhysicsNode().SetStone(true);
	allEntities.push_back(ball0);
	
	GameEntity* ball1 = BuildSphereEntity(15.0f, Vector3(300, 800, -100), Vector3(0, 0, 0), Vector4(1,0,0,1));
	ball1->GetPhysicsNode().putRest(true);
	ball1->GetPhysicsNode().setGravity(false);
	ball1->GetPhysicsNode().SetStone(true);
	allEntities.push_back(ball1);

	GameEntity* ball2 = BuildSphereEntity(15.0f, Vector3(-300, 800, 100), Vector3(0, 0, 0), Vector4(1,0,0,1));
	ball2->GetPhysicsNode().putRest(true);
	ball2->GetPhysicsNode().setGravity(false);
	allEntities.push_back(ball2);

	GameEntity* ball3 = BuildSphereEntity(15.0f, Vector3(300, 800, 100), Vector3(0, 0, 0), Vector4(1,0,0,1));
	ball3->GetPhysicsNode().putRest(true);
	ball3->GetPhysicsNode().setGravity(false);
	allEntities.push_back(ball3);

	GameEntity* ball4 = BuildSphereEntity(15.0f, Vector3(300, 800, 300), Vector3(0, 0, 0), Vector4(1,0,0,1));
	ball4->GetPhysicsNode().putRest(true);
	ball4->GetPhysicsNode().setGravity(false);
	ball4->GetPhysicsNode().SetStone(true);
	allEntities.push_back(ball4);

	GameEntity* ball5 = BuildSphereEntity(15.0f, Vector3(-300, 800, 300), Vector3(0, 0, 0), Vector4(1,0,0,1));
	ball5->GetPhysicsNode().putRest(true);
	ball5->GetPhysicsNode().setGravity(false);
	ball5->GetPhysicsNode().SetStone(true);
	allEntities.push_back(ball5);

	GameEntity* ballcenter = BuildSphereEntity(15.0f, Vector3(0, 800, 100), Vector3(0, 0, 0), Vector4(1,0,0,1));
	ballcenter->GetPhysicsNode().putRest(true);
	ballcenter->GetPhysicsNode().setGravity(false);
	allEntities.push_back(ballcenter);

	
	Spring *s = new Spring(&ball0->GetPhysicsNode(), Vector3(0,100,0), &ball1->GetPhysicsNode(), Vector3(0,100,0));
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);

	s = new Spring(&ball0->GetPhysicsNode(), Vector3(0,100,0), &ballcenter->GetPhysicsNode(), Vector3(0,100,0));
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);
	
	s = new Spring(&ball0->GetPhysicsNode(), Vector3(0,100,0), &ball2->GetPhysicsNode(), Vector3(0,100,0));
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);

	s = new Spring(&ball1->GetPhysicsNode(), Vector3(0,100,0), &ball3->GetPhysicsNode(), Vector3(0,100,0));
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);

	s = new Spring(&ball1->GetPhysicsNode(), Vector3(0,100,0), &ball4->GetPhysicsNode(), Vector3(0,100,0));
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);

	s = new Spring(&ball1->GetPhysicsNode(), Vector3(0,100,0), &ballcenter->GetPhysicsNode(), Vector3(0,100,0));
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);

	s = new Spring(&ball2->GetPhysicsNode(), Vector3(0,100,0), &ball5->GetPhysicsNode(), Vector3(0,100,0));
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);
	
	s = new Spring(&ball2->GetPhysicsNode(), Vector3(0,100,0), &ball3->GetPhysicsNode(), Vector3(0,100,0));
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);

	s = new Spring(&ball2->GetPhysicsNode(), Vector3(0,100,0), &ballcenter->GetPhysicsNode(), Vector3(0,100,0));
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);

	s = new Spring(&ball3->GetPhysicsNode(), Vector3(0,100,0), &ballcenter->GetPhysicsNode(), Vector3(0,100,0));
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);

	s = new Spring(&ball4->GetPhysicsNode(), Vector3(0,100,0), &ballcenter->GetPhysicsNode(), Vector3(0,100,0));
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);

	s = new Spring(&ball5->GetPhysicsNode(), Vector3(0,100,0), &ball4->GetPhysicsNode(), Vector3(0,100,0));
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);

	s = new Spring(&ball5->GetPhysicsNode(), Vector3(0,100,0), &ballcenter->GetPhysicsNode(), Vector3(0,100,0));
	PhysicsSystem::GetPhysicsSystem().AddConstraint(s);
	PhysicsSystem::GetPhysicsSystem().AddDebugDraw(s);
}