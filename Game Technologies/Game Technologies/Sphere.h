/*
	Class: Sphere.
		Represents the fruits, the bushes, and the rain.
*/

#pragma once
#include "../nclgl/SceneNode.h"
#include "../nclgl/OBJMesh.h"

class Sphere : public SceneNode{
public:
	/*
		Constructor.
		Params:
			colour: the colour of the sphere
	*/
	Sphere(Vector4 colour);

	/*
		Constructor.
		Params:
			location: Where is the rain located.
			rain_height: from where is falling.
			colour: The colour of the sphere
	*/
	Sphere(Vector3 location, float rain_height, Vector4 color);
	
	//Destructor
	~Sphere(void);

	static void CreateSphere(){
		OBJMesh *m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"sphere.obj");
		sphere = m;
	}

	static void DeleteSphere(){ delete sphere; }

	virtual void Update(float msec);

private:
	static Mesh* sphere;
	int grow_limit; // the limit a bush/fruit will grow
	float growth;
	bool is_rain;//if this is rain
	float rain_height; // the height of the rain
};