/*
	Octree class, partially works
*/
#include "OctTreeNode.h"
#include "Renderer.h"
//constructors
OctTreeNode::OctTreeNode (float width, float height, float length, Vector3 origin, vector<PhysicsNode*> elements, 
						  int threshold, int length_limit, int level){
	this->width = width;
	half_width = width/2.0f;
	this->height = height;
	half_height = height/2.0f;
	this->length = length;
	half_length = length/2.0f;
	this->origin = origin;
	this->elements = elements;
	this->threshold = threshold;
	this->length_limit = length_limit;
	this->level = level;
	x_axis_limit = origin.x + half_width;
	y_axis_limit = origin.y + half_height;
	z_axis_limit = origin.z + half_length;
	low_x_axis_limit = origin.x - half_width;
	low_y_axis_limit = origin.y - half_height;
	low_z_axis_limit = origin.z - half_length;
	isLeaf = false;
	if(x_axis_limit == low_x_axis_limit)
		return;
	if(y_axis_limit == low_y_axis_limit)
		return;
	if(z_axis_limit == low_z_axis_limit)
		return;
	
}

OctTreeNode::OctTreeNode (float width, float height, float length, Vector3 origin, int threshold, int length_limit, int level){
	this->width = width;
	half_width = width/2.0f;
	this->height = height;
	half_height = height/2.0f;
	this->length = length;
	half_length = length/2.0f;
	this->origin = origin;
	this->threshold = threshold;
	this->length_limit = length_limit;
	this->level = level;
	x_axis_limit = origin.x + half_width;
	y_axis_limit = origin.y + half_height;
	z_axis_limit = origin.z + half_length;
	low_x_axis_limit = origin.x - half_width;
	low_y_axis_limit = origin.y - half_height;
	low_z_axis_limit = origin.z - half_length;
	isLeaf = false;

	if(x_axis_limit == low_x_axis_limit)
		return;
	if(y_axis_limit == low_y_axis_limit)
		return;
	if(z_axis_limit == low_z_axis_limit)
		return;
	
}

OctTreeNode::~OctTreeNode(void){
}

//recursively build the tree
void OctTreeNode::BuildOctTree(){
	
	if(elements.size() >0){
		
		if(elements.size() > threshold && level <= length_limit){
			children.push_back(OctTreeNode(half_width, 
											half_height, 
											half_length, 
											Vector3(origin.x - half_width/2.0f, origin.y - half_height/2.0f, origin.z - half_length/2.0f), 
											threshold, 
											length_limit, 
											level+1));

			children.push_back(OctTreeNode(half_width,
											half_height, 
											half_length, 
											Vector3(origin.x - half_width/2.0f, origin.y - half_height/2.0f, origin.z + half_length/2.0f), 
											threshold, 
											length_limit, 
											level+1));

			children.push_back(OctTreeNode(half_width, 
											half_height,
											half_length, 
											Vector3(origin.x + half_width/2.0f, origin.y - half_height/2.0f, origin.z - half_length/2.0f), 
											threshold, 
											length_limit, 
											level+1));
			children.push_back(OctTreeNode(half_width, 
										half_height, 
										half_length, 
				Vector3(origin.x + half_width/2.0f, origin.y - half_height/2.0f, origin.z + half_length/2.0f), 
				threshold, length_limit, level+1));

			children.push_back(OctTreeNode(half_width, half_height, half_length, 
				Vector3(origin.x - half_width/2.0f, origin.y + half_height/2.0f, origin.z - half_length/2.0f) , 
				threshold, length_limit, level+1));
			children.push_back(OctTreeNode(half_width, half_height, half_length, 
				Vector3(origin.x - half_width/2.0f, origin.y + half_height/2.0f, origin.z + half_length/2.0f) , 
				threshold, length_limit, level+1));
			children.push_back(OctTreeNode(half_width, half_height, half_length, 
				Vector3(origin.x + half_width/2.0f, origin.y + half_height/2.0f, origin.z - half_length/2.0f) ,
				threshold, length_limit, level+1));
			children.push_back(OctTreeNode(half_width, half_height, half_length, 
				Vector3(origin.x + half_width/2.0f, origin.y + half_height/2.0f, origin.z - half_length/2.0f) , 
				threshold, length_limit, level+1));
		
			for(unsigned int j = 0; j < children.size(); j++){
				for(unsigned int i = 0; i < elements.size(); i++){
					if(elements[i]->isIntelligent()){
						cout << endl;
					}
					Vector3 position = elements[i]->GetPosition();
					
					if((position.x <= children[j].getXLimit() && position.x >= children[j].getLowerX())){
						if((position.y <= children[j].getYLimit() && position.y >= children[j].getLowerY())){
							if((position.z <= children[j].getZLimit() && position.z >= children[j].getLowerZ())){
								children[j].PushElement(elements[i]);
							}
						}
					}
				} 
			}

			for(int i = 0; i < children.size(); i++){
				children[i].BuildOctTree();
			}
		} else {
			isLeaf = true;
		
		}
	}

}

void OctTreeNode::PushElement(PhysicsNode* element){
	elements.push_back(element);
}

//get the paired elements
vector<pair<PhysicsNode*, PhysicsNode*>> OctTreeNode::getPairedElements(){
	vector <pair<PhysicsNode* , PhysicsNode*>> result;
	if(isLeaf){
		if(elements.size() > 1){
			for(unsigned int i = 0; i < elements.size(); i++){
				for(unsigned int j = i + 1; j < elements.size(); j++){
					pair<PhysicsNode*, PhysicsNode*> collision;
					collision = std::make_pair((elements[i]), (elements[j]));
					result.push_back(collision);
				}
			}
		}
	} else{
		vector<pair<PhysicsNode* , PhysicsNode*>> elems;
		for(unsigned int j = 0; j < children.size(); j++){
			elems = children[j].getPairedElements();
			for(unsigned int i = 0; i < elems.size(); i++){
					result.push_back(elems[i]);
			}
		}
	}
	return result;
}

//draw debug lines to see how beautifully the tree draws itself
void OctTreeNode::DrawDebugLines(){
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(low_x_axis_limit, low_y_axis_limit, low_z_axis_limit), Vector3(x_axis_limit, low_y_axis_limit, low_z_axis_limit));
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(low_x_axis_limit, low_y_axis_limit, low_z_axis_limit), Vector3(low_x_axis_limit, y_axis_limit, low_z_axis_limit));
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(low_x_axis_limit, low_y_axis_limit, low_z_axis_limit), Vector3(low_x_axis_limit, low_y_axis_limit, z_axis_limit));
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(x_axis_limit, low_y_axis_limit, low_z_axis_limit), Vector3(x_axis_limit, low_y_axis_limit, z_axis_limit));
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(x_axis_limit, low_y_axis_limit, low_z_axis_limit), Vector3(x_axis_limit, y_axis_limit, low_z_axis_limit));
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(x_axis_limit, low_y_axis_limit, z_axis_limit), Vector3(x_axis_limit, low_y_axis_limit, low_z_axis_limit));
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(x_axis_limit, low_y_axis_limit, z_axis_limit), Vector3(x_axis_limit, y_axis_limit, z_axis_limit));
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(low_x_axis_limit, low_y_axis_limit, z_axis_limit), Vector3(low_x_axis_limit, y_axis_limit, z_axis_limit));
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(low_x_axis_limit, low_y_axis_limit, z_axis_limit), Vector3(x_axis_limit, low_y_axis_limit, z_axis_limit));
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(low_x_axis_limit, y_axis_limit, z_axis_limit), Vector3(x_axis_limit, y_axis_limit, z_axis_limit));
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(low_x_axis_limit, y_axis_limit, z_axis_limit), Vector3(low_x_axis_limit, y_axis_limit, low_z_axis_limit));
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(x_axis_limit, y_axis_limit, z_axis_limit), Vector3(x_axis_limit, y_axis_limit, low_z_axis_limit));
	Renderer::GetRenderer().DrawDebugLine(DEBUGDRAW_PERSPECTIVE, Vector3(low_x_axis_limit, y_axis_limit, low_z_axis_limit), Vector3(x_axis_limit, y_axis_limit, low_z_axis_limit));
	for(unsigned int i = 0; i < children.size(); i++){
		children[i].DrawDebugLines();
	}
}
//add elements to the tree
void OctTreeNode::pushElements(vector <PhysicsNode*> new_elems){
	elements.erase(elements.begin(), elements.end());
	for(unsigned int i = 0; i < new_elems.size(); i ++){
		elements.push_back(new_elems[i]);
	}
}