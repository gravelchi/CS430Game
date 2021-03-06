#include "Entity.h"

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <cstring>
#include <common/objloader.hpp>
#include <algorithm>
#include <stdlib.h>

#include <iostream>
#include <glm\gtx\string_cast.hpp>
//using namespace glm;

int position;
int vertexCount;
int health;
int mySpeed;
int currentTextureRow, currentTextureCol;
double creationTime;
bool active;

Entity::Entity(
	std::vector<glm::vec3> & vertexBuffer, 
	std::vector<glm::vec2> & uvBuffer,
	std::vector<glm::vec3> & normalBuffer,
	char * objPath, 
	glm::vec3 location,
	int textureRow,
	int textureColumn,
	float speed,
	int hitpoints)
{
	//save references to buffers
	//save index of first vertex
	position = int(vertexBuffer.size());
	//read object file
	loadOBJ(objPath, vertexBuffer, uvBuffer, normalBuffer);

	//save number of vertices added to buffer
	vertexCount = int(vertexBuffer.size()) - position;

	//set location of object
	glm::mat4 translate = glm::translate(glm::mat4(1.0f),location);
	for (int i = position; i < position + vertexCount; i++){
		glm::vec4 point = glm::vec4(vertexBuffer[i],1.0f);
		point = translate * point;
		vertexBuffer[i] = glm::vec3(point.x, point.y, point.z);
	}

	//set hitpoints
	health = hitpoints;

	//set speed
	mySpeed = speed;

	//set texture
	currentTextureRow = textureRow;
	currentTextureCol = textureColumn;
	setTexture(currentTextureRow,currentTextureCol, uvBuffer);

	//set creationtime
	creationTime = glfwGetTime();
	active = true;
}


Entity::~Entity(void)
{

}

//load the object from an object file
bool Entity::loadObject(const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals)
{	
	return loadOBJ(path, out_vertices, out_uvs, out_normals);
}

bool Entity::collide(std::vector<glm::vec3> & vertexBuffer, std::vector<Entity*> & bullets, Entity * player, int &enemyCount, int level, std::vector<glm::vec2> & textureBuffer, int &score)
{
	if (active){
		//get center of this item
		glm::vec3 self = glm::vec3(1.0f);
		getLocation(vertexBuffer, self);

		glm::vec3 other = glm::vec3(1.0f);

		float dx, dy, distance;

		//check bullet collisions
		for (int i = 0; i < bullets.size(); i++)
		{
			//get center of bullet
			bullets[i]->getLocation(vertexBuffer, other);
			
			dx = self.x - other.x;
			dy = self.y - other.y;

			distance = sqrt(dx * dx + dy * dy);

			if (distance < 1.5)
			{
				//collision
				health -= 1;
				if (health < 1)
				{
					destroy(vertexBuffer, textureBuffer);
					enemyCount--;
					if (level == 1) {score += int(self.y);}
					else {score += 5;}
				}
				bullets[i]->destroy(vertexBuffer, textureBuffer);
				return false;
			}
		}

		//check player collisions
		player->getLocation(vertexBuffer,other);

		dx = self.x - other.x;
		dy = self.y - other.y;

		distance = sqrt(dx * dx + dy * dy);

		if (distance < 1.5)
		{
			//collision
			destroy(vertexBuffer, textureBuffer);
			enemyCount--;
			if (score > 100) {
				score -= 100; 
				
					
			}
			else
			{
				score = 0;
			}
			return true;
		}

		//check ground collision
		if (level == 1)
		{
			if(self.y < 4.0)
			{
				if (score > 100) {
					score -= 100; 
				}
				else
				{
					score = 0;
				}
				return true;
			}
		}
		if (level == 2)
		{
			if (self.y < 0.0){
				score += 7;
				deactivate();
				move(vertexBuffer, glm::vec3(40.0f,10.0f,0.0f));
				deactivate();
			}
			
		}

	}
	return false;
}

void Entity::move(std::vector<glm::vec3> & vertexBuffer, glm::vec3 location)
{
	glm::vec4 point;
	//get center of object
	glm::vec3 center = glm::vec3(1.0f);
	getLocation(vertexBuffer,center);

	//create translation to move to origin
	glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f),-center);

	//create translation to destination
	glm::mat4 toDest = glm::translate(glm::mat4(1.0f),location);
	
	for (int i = position; i < (position + vertexCount); i++)
	{
		point = glm::vec4(vertexBuffer[i], 1.0f);
		point = toDest * toOrigin * point;
		//std::cout<< "point "<<glm::to_string(point)<< std::endl;
		vertexBuffer[i] = glm::vec3(point.x,point.y+2,point.z);
		//std::cout<< "buffer " <<glm::to_string(vertexBuffer[i])<< std::endl;
	}
}

int Entity::getBufferPosition()
{
	return position;
}

int Entity::getLengthInBuffer()
{
	return vertexCount;
}

void Entity::moveX(std::vector<glm::vec3> & vertexBuffer, float time)
{

	glm::vec4 point;
	glm::mat4 trans = glm::translate(glm::mat4(1.0f),glm::vec3((mySpeed*time),0.0f,0.0f));
	for (int i = position; i < (position + vertexCount); i++)
	{

		point = glm::vec4(vertexBuffer[i], 1.0f);
		float currentX = point.x;
		point = trans * point;

		

		//std::cout<< "point "<<glm::to_string(point)<< std::endl;
		vertexBuffer[i] = glm::vec3(point.x,point.y,point.z);
		//std::cout<< "buffer " <<glm::to_string(vertexBuffer[i])<< std::endl;
	}
}

void Entity::moveY(std::vector<glm::vec3> & vertexBuffer, float time)
{
	glm::vec4 point;
	glm::mat4 trans = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,(mySpeed*time),0.0f));
	for (int i = position; i < (position + vertexCount); i++)
	{
		point = glm::vec4(vertexBuffer[i], 1.0f);
		point = trans * point;
		//std::cout<< "point "<<glm::to_string(point)<< std::endl;
		vertexBuffer[i] = glm::vec3(point.x,point.y,point.z);
		//std::cout<< "buffer " <<glm::to_string(vertexBuffer[i])<< std::endl;
	}

}

void Entity::setTexture(
	int row, 
	int col, 
	std::vector<glm::vec2> & textureBuffer)
{
	float x = float(col) / 8;
	float y = float(row) / 8;

	glm::vec2 topLeft = glm::vec2(x,y+0.125);
	glm::vec2 topRight = glm::vec2(x+0.125,y+0.125);
	glm::vec2 bottomLeft = glm::vec2(x,y);
	glm::vec2 bottomRight = glm::vec2(x+0.125,y);

	textureBuffer[position+0] = topLeft;//top left
	textureBuffer[position+1] = bottomLeft;//bottom left
	textureBuffer[position+2] = topRight;//top right

	textureBuffer[position+3] = topRight;//top right
	textureBuffer[position+4] = bottomLeft;//bottom left
	textureBuffer[position+5] = bottomRight;//bottom right
	
}

void Entity::getLocation(std::vector<glm::vec3> & vertexBuffer, glm::vec3 & point)
{
	//calculate center of entity and return the center point
	
	//calculate average x
	float x, minX, maxX;
	minX = maxX = vertexBuffer[position].x;

	//calculate average y
	float y, minY, maxY;
	minY = maxY = vertexBuffer[position].y;
	for (int i = position; i < (position + vertexCount); i++)
	{
		
		//x values
		float value = vertexBuffer[i].x;
		if (value < minX)
		{
			minX = value;
		}
		else if (value > maxX)
		{
			maxX = value;
		}
		x = (minX + maxX)/2;

		//y values
		value = vertexBuffer[i].y;
		if (value < minY)
		{
			minY = value;
		}
		else if (value > maxY)
		{
			maxY = value;
		}

		y = (minY + maxY)/2;
		

		
	}

	point.x = x;
	point.y = y;
	point.z = 0.0f;

}

double Entity::getLifeSpan(void)
{
	return glfwGetTime() - creationTime;
}

void Entity::destroy(std::vector<glm::vec3> & vertexBuffer, std::vector<glm::vec2> & textureBuffer)
{
	active = false;

	//move to inactive area (0,-10,0)
	move(vertexBuffer,glm::vec3(40.0f,-10.0f,0.0f));
}

void Entity::scale(std::vector<glm::vec3> & vertexBuffer, glm::vec3 & scale)
{
	glm::vec4 point;

	//create scale matrix
	glm::mat4 s = glm::scale(glm::mat4(1.0f),scale);
	
	//get center of object
	glm::vec3 center = glm::vec3(1.0f);
	getLocation(vertexBuffer,center);

	//create translation to move to origin
	glm::mat4 toOrigin = glm::translate(glm::mat4(1.0f),-center);
	glm::mat4 reset = glm::translate(glm::mat4(1.0f),center);
	
	for (int i = position; i < (position + vertexCount); i++)
	{
		point = glm::vec4(vertexBuffer[i], 1.0f);
		point = reset * s * toOrigin * point;
		vertexBuffer[i] = glm::vec3(point.x,point.y+2,point.z);
	}
}

void Entity::activate()
{
	active = true;
}

void Entity::deactivate()
{
	active = false;
}

bool Entity::isActive(void){ return active; }

float Entity::getSpeed(){return mySpeed;}

