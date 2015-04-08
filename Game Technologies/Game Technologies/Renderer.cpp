#include "Renderer.h"
#include <sstream>
#include "PhysicsSystem.h"

Renderer* Renderer::instance = NULL;

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{	
	camera			= NULL;

	root			= new SceneNode();

	quad = Mesh::GenerateQuad();

	framerate = 0.0f;
	msec_passed = 0.0f;
	physics_framerate = 0.0f;
	ballsize = 10.0f;
	speed = 10.0f;

	simpleShader	= new Shader(SHADERDIR"TechVertex.glsl", SHADERDIR"TechFragment.glsl");
	skyBoxShader = new Shader(SHADERDIR"skyboxVertex.glsl", SHADERDIR"skyboxFragment.glsl");
	textShader = new Shader (SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");


	if(!simpleShader->LinkProgram() || !skyBoxShader->LinkProgram() || !textShader->LinkProgram()){
		return;
	}

	
	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg", 
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB, 
		SOIL_CREATE_NEW_ID, 0);

		basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT),16, 16);


	//TODO glEnable(GL_CULL_FACE);

	instance		= this;

	init			= true;
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	score = 0;
	meatballs = -1;
	greanades = -1;
	current = -1;
}

Renderer::~Renderer(void)	{
	delete root;
	delete simpleShader;
	delete skyBoxShader;
	delete quad;
	delete basicFont;

	currentShader = NULL;
}

void Renderer::UpdateScene(float msec)	{
	if(camera) {
		camera->UpdateCamera(msec); 
	}
	root->Update(msec);
	framerate++;
	msec_passed = msec;
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

}

void Renderer::RenderScene()	{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	DrawSkyBox();
	DrawMessage();

	if(camera) {
		SetCurrentShader(simpleShader);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		

		textureMatrix.ToIdentity();
		modelMatrix.ToIdentity();
		viewMatrix		= camera->BuildViewMatrix();
		projMatrix		= Matrix4::Perspective(1.0f,10000.0f,(float)width / (float) height, 45.0f);
		frameFrustum.FromMatrix(projMatrix * viewMatrix);
		UpdateShaderMatrices();

		//Return to default 'usable' state every frame!
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDisable(GL_STENCIL_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		BuildNodeLists(root);
		SortNodeLists();
		DrawNodes();
		ClearNodeLists();
	}

	glUseProgram(0);
	SwapBuffers();
}

void	Renderer::DrawNode(SceneNode*n)	{
	if(n->GetMesh()) {
		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"),	1,false, (float*)&(n->GetWorldTransform()*Matrix4::Scale(n->GetModelScale())));
		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"),1,(float*)&n->GetColour());

		n->Draw(*this);
	}
}

void	Renderer::BuildNodeLists(SceneNode* from)	{
	Vector3 direction = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(direction,direction));

	//if(frameFrustum.InsideFrustum(*from)) {
		if(from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else{
			nodeList.push_back(from);
		}
	//}

	for(vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void	Renderer::DrawNodes()	 {
	for(vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i ) {
		if((*i)->IsHeightMap()){
			SetTextureRepeating((*i)->GetMesh()->GetTexture(), true);
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), (*i)->GetMesh()->GetTexture());
		}else {
			if((*i)->GetMesh()){
				glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), (*i)->GetMesh()->GetTexture());
			}
		}
		
		
		DrawNode((*i));
	}

	for(vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i ) {
		DrawNode((*i));
	}
}

void	Renderer::SortNodeLists()	{
	std::sort(transparentNodeList.begin(),	transparentNodeList.end(),	SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),				nodeList.end(),				SceneNode::CompareByCameraDistance);
}

void	Renderer::ClearNodeLists()	{
	transparentNodeList.clear();
	nodeList.clear();
}

void	Renderer::SetCamera(Camera*c) {
	camera = c;
}

void	Renderer::AddNode(SceneNode* n) {
	root->AddChild(n);
}

void	Renderer::RemoveNode(SceneNode* n) {
	root->RemoveChild(n);
}

	
void Renderer::DrawSkyBox(){
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyBoxShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 3);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

/*
Lets draw the elements
*/
void Renderer::DrawMessage(){
	SetCurrentShader(textShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	ostringstream str;
	ostringstream str2;
	ostringstream str3;
	ostringstream str4;
	ostringstream str5;
	ostringstream str6;
	ostringstream str7;
	ostringstream str8;
	ostringstream str9;

	float frames = (float)framerate/(msec_passed /1000.0f);
	str << "Framerate: " << frames << " fps"<<endl;
	PutText(str.str(), Vector3(0,0,0), 16.0f);
	float msec_physics = PhysicsSystem::GetPhysicsSystem().getMsec();
	physics_framerate = PhysicsSystem::GetPhysicsSystem().getframerate()/(msec_physics/1000.0f);
	str2<< "Physics Framerate: " << physics_framerate << " fps" << endl;
	PutText(str2.str(), Vector3(0,16.0f,0), 16.0f);
	int collisions = PhysicsSystem::GetPhysicsSystem().getColllisions();
	str3<< "Collisions: " << collisions << endl;
	PutText(str3.str(), Vector3(0, 32.0f, 0), 16.0f);
	str4 << "Score: " << score << endl;
	PutText(str4.str(), Vector3(0, 48.0f, 0), 16.0f);
	str5 << "Meatball ammo: " << meatballs << endl;
	PutText(str5.str(), Vector3(0.0f, 64.0f, 0.0f), 16.0f);
	str7 << "Greanade ammo: " << greanades << endl;
	PutText(str7.str(), Vector3(0.0f, 80.0f,0.0f), 16.0f);
	str6 << "Current secondary weapon: " ;
	switch(current){
	case 0:
		str6 << "Meat Ball Bait" << endl;
		break;
	case 1:
		str6 << "Greanades" << endl;
		break;
	}
	PutText(str6.str(), Vector3(0, 96.0f, 0), 16.0f);
	str8 << "BallSize: ";
	if(ballsize < 10.0f){
		str8 << "Smallish .. " << endl;
	} else if (ballsize == 10.0f){
		str8 << "Normal size" << endl;
	} else {
		str8 << "BIG" << endl;
	}

	PutText(str8.str(), Vector3(0.0f, 112.0f, 0.0f), 16.0f);
	str9 << "Speed: ";
	if(speed < 10.0f) {
		str9 << "Wow watchout hotshot!" << endl;
	} else if(speed == 10.0f){
		str9 << "Normal speed, good job" << endl;
	} else if(speed > 10.0f){
		str9<< "slowish .." << endl;
		
	}
	PutText(str9.str(), Vector3(0, 127.0f, 0), 16.0f);
	glUseProgram(0);
	framerate = 0;
}

void Renderer::PutText(const std::string &text, const Vector3 &position, const float size, const bool perspective){
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text,*basicFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if(perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size,size,1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f,10000.0f,(float)width / (float)height, 45.0f);
	}
	else{	
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		modelMatrix = Matrix4::Translation(Vector3(position.x,height-position.y, position.z)) * Matrix4::Scale(Vector3(size,size,1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f,1.0f,(float)width, 0.0f,(float)height, 0.0f);
	}
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();

	delete mesh; //Once it's drawn, we don't need it anymore!
}
