//Untitled Game
//By Alasdair Hogarth + David Wagner

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <common/Entity.h>
#include <common\Player.h>
#include <common\Mothership.h>
#include <common\Digit.h>
#include <Windows.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\string_cast.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>



/*************************************************************
To Do: (! indicates done, % indicates partial completion)
-!	Add method definitions for creating player;
-%	Add method definitions for each type of enemy 
	(this will allow a simple method call to set speed, 
	movement, etc without having to manually set for 
	each of that enemy type);
-	Add second do loop for second level;
-	Remove superfluous tutorial 7 code
**************************************************************/



/*************************************************************
						global variables
**************************************************************/
#define START "Images/Splash.bmp"
#define LOSE "Images/LoseScreen.bmp"
#define WIN "Images/WinScreen.bmp"
#define STAGE2 "Images/Stage2InstructGreen.bmp"
#define LEVEL_TEXTURES "Images/WorkingTexturesColor.bmp"
#define SCORE "Images/Score.bmp"
#define BULLET_COUNT 3

GLuint Texture;
GLuint TextureID;

GLuint vertexbuffer;
GLuint uvbuffer;

GLuint VertexArrayID;
GLuint programID;
GLuint MatrixID;

std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;

glm::mat4 Projection = getProjectionMatrix();
glm::mat4 View = getViewMatrix();
glm::mat4 ModelMatrix = glm::mat4(1.0);
glm::mat4 MVP = Projection * View * ModelMatrix;

std::vector<Entity*> bullets;
std::vector<Entity*> enemies;
std::vector<Digit*> scoreVector;
Player* player;
Mothership* mother;

bool white = false;
int nextBullet;
int enemyCount;
int level;
int playerLives = 3;
bool continueGame = true;
glm::vec3 tempPoint;
int score = 0;
int stage2PacifistCounter = 0;
///////////////////////////////////////////////////////////////


/*************************************************************
					helper method declerations
**************************************************************/
//fires a projectile if available
void fire();

//creates and adds a player object into all relevant buffers
Player* addPlayer(int numOfLives,
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer);

//adds enemy with spiral design and speed 3
Entity* addSpiral (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location);

//adds enemy with egg design and speed 2
Entity* addEgg (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location);

//adds enemy with egg design and speed 1
Entity* addLotus (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location);

//adds enemy with brain design and speed 1
Entity* addBrain (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location);

//adds enemy with nugget design, speed 12, and 2 hitpoints
Entity* addNugget (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location);

//adds enemy with crystal design, speed 12, and 2 hitpoints
Entity* addCrystal (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location);

//adds enemy with rose design, speed 12, and 2 hitpoints
Entity* addRose (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location);

//adds enemy with clasp design, speed 12, and 2 hitpoints
Entity* addClasp (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location);

//adds enemy mothership, speed 12, and 1000000 hitpoints
Mothership* addMothership (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location);

//adds projectile object
Entity* addBullet (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location);

//creates a pool of projectiles of size "count"
void setupBullets (int count);

//adds score to screen at given position
void setupScore (int scoreValue, int xPos, int yPos);

//refreshes score display to current score
void refreshScore (void);

//moves bullets and resets bullets if they reach top screen
void manageBullets(double deltaTime);

//closes the program and cleans up
int closeProgram();

//displays a splashscreen with the given image url
void showSplashScreen(char * url);

//displays screen with current score
void showScoreScreen();

//prepare buffers
void setupBuffers();

//update buffer information and draw triangles
void drawBuffers();

//compute MVP matrix andsend to bound shader
void computeMVP();

//play explosion sound, update texture and update lives
void destroyPlayer (Player* player);

//clear all the vectors used to store entity/player information
//i.e. vertices, uvs, normals
void clearVertices();

//setup level one and run do loop
void levelOne();

//setup level 2 and run do loop
void levelTwo();

/*************************************************************
					helper method implementations
**************************************************************/

void fire(double currentTime, double &bulletTime, glm::vec3 point)
{
	for (int i = 0; i < BULLET_COUNT; i++){
				if (!bullets[i]->isActive()){
					//available to fire
					if ((currentTime - bulletTime) > 0.3){
						//put bullet above player
						tempPoint = glm::vec3(1.0f);
						player->getLocation(vertices, tempPoint);
						bullets[nextBullet]->move(vertices, glm::vec3(tempPoint.x,tempPoint.y,tempPoint.z));
						bullets[nextBullet]->activate();
						PlaySound("Sounds/pew.wav",NULL,SND_FILENAME|SND_ASYNC);
						nextBullet = (nextBullet + 1) % bullets.size();
						bulletTime = currentTime;
					}
				}
			}
}

Player* addPlayer (int numOfLives,
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer) //textureBuffer
{
	Player* player = new Player(
		playerLives,							//number of lives
		vertexBuffer,				//vertex buffer
		uvBuffer,					//texture buffer
		normalBuffer,				//normal buffer
		"Player2.obj",				//object file
		glm::vec3(0.0f,4.0f,0.0f),	//location on screen
		2,							//texture row
		0,							//texture column
		20.0,
		1);
	return player;
};

Entity* addSpiral (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location)
{
	Entity* spiral = new Entity(
		vertexBuffer,				//vertex buffer
		uvBuffer,					//texture buffer
		normalBuffer,				//normal buffer
		"Player2.obj",				//object file
		location,					//location on screen
		0,							//texture row
		0,
		3.0f,
		1);

	return spiral;
}

Entity* addEgg (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location)
{
	Entity* egg = new Entity(
		vertexBuffer,				//vertex buffer
		uvBuffer,					//texture buffer
		normalBuffer,				//normal buffer
		"Player2.obj",				//object file
		location,					//location on screen
		0,							//texture row
		1,
		2.0f,
		1);

	return egg;
}

Entity* addLotus (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location)
{
	Entity* lotus = new Entity(
		vertexBuffer,				//vertex buffer
		uvBuffer,					//texture buffer
		normalBuffer,				//normal buffer
		"Player2.obj",				//object file
		location,					//location on screen
		0,							//texture row
		2,
		1.0f,
		1);

	return lotus;
}

Entity* addBrain (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location)
{
	Entity* brain = new Entity(
		vertexBuffer,				//vertex buffer
		uvBuffer,					//texture buffer
		normalBuffer,				//normal buffer
		"Player2.obj",				//object file
		location,					//location on screen
		0,							//texture row
		3,
		1.8f,
		1);

	return brain;
}

Entity* addNugget (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location)
{
	Entity* nugget = new Entity(
		vertexBuffer,				//vertex buffer
		uvBuffer,					//texture buffer
		normalBuffer,				//normal buffer
		"Player2.obj",				//object file
		location,					//location on screen
		0,							//texture row
		4,
		15.0f,
		2);
	stage2PacifistCounter++;
	return nugget;
}

Entity* addCrystal (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location)
{
	Entity* crystal = new Entity(
		vertexBuffer,				//vertex buffer
		uvBuffer,					//texture buffer
		normalBuffer,				//normal buffer
		"Player2.obj",				//object file
		location,					//location on screen
		0,							//texture row
		5,
		15.0f,
		2);
	stage2PacifistCounter++;
	return crystal;
}

Entity* addRose (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location)
{
	Entity* rose = new Entity(
		vertexBuffer,				//vertex buffer
		uvBuffer,					//texture buffer
		normalBuffer,				//normal buffer
		"Player2.obj",				//object file
		location,					//location on screen
		0,							//texture row
		6,
		15.0f,
		2);
	stage2PacifistCounter++;
	return rose;
}

Entity* addClasp (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location)
{
	Entity* clasp = new Entity(
		vertexBuffer,				//vertex buffer
		uvBuffer,					//texture buffer
		normalBuffer,				//normal buffer
		"Player2.obj",				//object file
		location,					//location on screen
		0,							//texture row
		7,
		15.0f,
		2);
	stage2PacifistCounter++;
	return clasp;
}

Mothership* addMothership (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location)
{
	Mothership* mothership = new Mothership(
		vertexBuffer,				//vertex buffer
		uvBuffer,					//texture buffer
		normalBuffer,				//normal buffer
		"Player2.obj",				//object file
		location,					//location on screen
		4,							//texture row
		5,
		15.0f,
		1000000);

	return mothership;
}

Entity* addBullet (
		std::vector<glm::vec3> & vertexBuffer,
		std::vector<glm::vec2> & uvBuffer,
		std::vector<glm::vec3> & normalBuffer,
		glm::vec3 location)
{
	Entity* bullet = new Entity(
		vertexBuffer,				//vertex buffer
		uvBuffer,					//texture buffer
		normalBuffer,				//normal buffer
		"Player2.obj",				//object file
		location,					//location on screen
		4,							//texture row
		2,
		30.0f,
		1);

	return bullet;
}

void setupBullets (int count)
{
	//add three bullets to bullet pool
	for (int i = 0; i < count; i++){
		bullets.push_back(addBullet(vertices,uvs,normals,glm::vec3(1.0f)));
	}
	for (int i = 0; i < bullets.size(); i ++)
	{
		//deactivate bullets
		bullets[i]->destroy(vertices, uvs);	
	}
	nextBullet = 0;
}

void setupScore (int scoreValue, int xPos, int yPos)
{
	if (scoreVector.size() > 0) scoreVector.clear();
	//add score digits
	int value , remainder = scoreValue, divisor;
	int x;
	glm::vec3 location;
	for (int i = 0; i < 5; i++)
	{
		x = (2*i) + xPos;
		location = glm::vec3(x,yPos,-1.0f);
		divisor = int(std::pow(10.0,(4-i)));
		value = int(remainder / divisor);
		remainder = remainder % divisor;
		Digit* digit = new Digit(vertices, uvs, normals, "Player2.obj",location);
		digit->setDigit(uvs, value);
		scoreVector.push_back(digit);
	}
}

void refreshScore (void)
{
	if (scoreVector.size() <= 0) return;

	int value , remainder = score, divisor;
	for (int i = 0; i < 5; i++)
	{
		divisor = int(std::pow(10.0,(4-i)));
		value = int(remainder / divisor);
		remainder = remainder % divisor;
		scoreVector[i]->setDigit(uvs, value);
	}
}

void manageBullets(double deltaTime)
{
	for (int i = 0; i < bullets.size(); i++)
		{
			if (bullets[i]->isActive()){
				bullets[i]->moveY(vertices,deltaTime);
				bullets[i]->getLocation(vertices,tempPoint);
				if (tempPoint.y > 29.0f)
				{
					score -= 10;
					bullets[i]->destroy(vertices, uvs);
				}
			}
		}
}

//close program method
int closeProgram()
{

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

//splash screen method
void showSplashScreen(char * url)
{
	int task = 0;
	clearVertices();

	Texture = loadBMP_custom(url);

	// Get a handle for our "myTextureSampler" uniform
	TextureID  = glGetUniformLocation(programID, "myTextureSampler");
	loadOBJ("Splash.obj", vertices, uvs, normals);

	// Load it into a VBO

	setupBuffers();

	float startTime = glfwGetTime();
	float currentTime;
	float deltaTime;

	do
	{

		currentTime = glfwGetTime();
		deltaTime = currentTime - startTime;

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMVP();

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*sizeof(glm::vec3), &vertices[0]) ;
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		if ( glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS && deltaTime > 0.3)
		{
			task = 0;
			break;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS && deltaTime > 0.3)
		{
			//clear buffer vextors
			clearVertices();

			//is url is start
			if (strcmp(url, START) == 0){task = 1;}
			//if url is stage 2
			if (strcmp(url, STAGE2) == 0){task = 2;}
			//if url is lose or win
			if (strcmp(url, WIN) == 0 || strcmp(url, LOSE) == 0){task = 0;}

			break;
		}

	}
	while( true );

	switch (task)
	{
	case 0:
		//exit
		continueGame = false;
		break;
	case 1:
		//start level 1
		levelOne();
		break;
	case 2:
		//start level 2
		levelTwo();
		break;
	default:
		//go to start
		break;
	}
}

void showScoreScreen()
{
	clearVertices();
	setupScore(score, -5, 10);
	loadOBJ("score.obj", vertices, uvs, normals);

	// Load the texture
	Texture = loadBMP_custom(SCORE);
	
	// Get a handle for our "myTextureSampler" uniform
	TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	setupBuffers();

	float startTime = glfwGetTime();
	float currentTime;
	float deltaTime;

	do
	{

		currentTime = glfwGetTime();
		deltaTime = currentTime - startTime;

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMVP();

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		drawBuffers();

		if ( glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS && deltaTime > 0.3)
		{
			break;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS && deltaTime > 0.3)
		{
			//clear buffer vextors
			clearVertices();
			break;
		}

	}
	while( true );

	clearVertices();
}

void setupBuffers()
{
	//GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	//GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
}

void drawBuffers()
{
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*sizeof(glm::vec3), &vertices[0]) ;
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, uvs.size()*sizeof(glm::vec2), &uvs[0]) ;
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void computeMVP()
{
	// Compute the MVP matrix from keyboard and mouse input
	computeMatricesFromInputs();
	Projection = getProjectionMatrix();
	View = getViewMatrix();
	ModelMatrix = glm::mat4(1.0);
	MVP = Projection * View * ModelMatrix;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
}

void destroyPlayer (Player* player){

	/////////////////////////////////
	//change texture to explosion and display
	//////////////////////////////////

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(programID);

	player->setTexture(3,4,uvs);

	computeMVP();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(TextureID, 0);

	drawBuffers();

	//////////////////////////////////////////////////
	//remove life from player and play explosion sound
	//////////////////////////////////////////////////

	player->removeLife();
	PlaySound("Sounds/boom.wav",NULL,SND_FILENAME|SND_SYNC);

	//////////////////////////////////////////////////////////////////
	//check remaining lives and exit to lose screen or update texture
	/////////////////////////////////////////////////////////////////
	int lifeCheck = player->getLives();
	switch (lifeCheck){
	case 3:
		player->setTexture(2,0,uvs);
		break;
	case 2:
		player->setTexture(2,1,uvs);
		break;
	case 1:
		player->setTexture(2,2,uvs);
		break;
	default:
		player->setTexture(3,4,uvs);
		break;
	}

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(programID);

	// Compute the MVP matrix from keyboard and mouse input
	computeMVP();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(TextureID, 0);

	drawBuffers();

}

void clearVertices()
{
	if (vertices.size() > 0) {vertices.clear();}
	if (uvs.size() > 0) {uvs.clear();}
	if (normals.size() > 0) {normals.clear();}
	if (enemies.size() > 0) {enemies.clear();}
}

//level one method
void levelOne()
{
	clearVertices();
	//setup
	level = 1;
	playerLives = 3;
	score = 0;
	setupScore(score, -19, 1);

	char* url;
	// Load the texture
	Texture = loadBMP_custom(LEVEL_TEXTURES);
	
	// Get a handle for our "myTextureSampler" uniform
	TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	//add earth and stretch out
	bool res;
	res = loadOBJ("earth.obj", vertices, uvs, normals);

	//add player model
	player = addPlayer(playerLives, vertices, uvs, normals);
	glm::vec3 point1 = glm::vec3(1.0f);

	//std::vector<Entity*> wave1;
	for (int i = 0; i < 15; i++)
	{
		
		enemies.push_back(addSpiral(vertices,uvs,normals,glm::vec3(-17.5+(2.5*(i)),20.0f,0.0f)));
		enemies.push_back(addEgg(vertices,uvs,normals,glm::vec3(-17.5+(2.5*(i)),22.5f,0.0f)));
		enemies.push_back(addLotus(vertices,uvs,normals,glm::vec3(-17.5+(2.5*(i)),25.0f,0.0f)));
		enemies.push_back(addBrain(vertices,uvs,normals,glm::vec3(-17.5+(2.5*(i)),27.5f,0.0f)));
	}

	enemyCount = enemies.size();
	setupBullets(BULLET_COUNT);

	setupBuffers();

	//do loop
	do{
		// glfwGetTime is called only once, the first time this function is called
		static double lastTime = glfwGetTime();
		static double bulletTime = lastTime;

		// Compute time difference between current and last frame
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime) / 2;

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		//move enemy waves and check for collisions
		for (int i = 0; i < enemies.size(); i++)
		{
			Entity* current = enemies[i];
			bool done = false;
			if (current->isActive())
			{
				current->moveY(vertices, -deltaTime);

				//check for collision
				if (current->collide(vertices, bullets, player, enemyCount, level, uvs, score)){
					//player loses life
					destroyPlayer(player);
					playerLives = player->getLives();
					if(playerLives > 0)
					{
						//move aliens back up screen and reset clock time
						for (int i = 0; i < enemies.size(); i++){
							if (enemies[i]->isActive()){
								enemies[i]->getLocation(vertices,point1);
								enemies[i]->move(vertices,glm::vec3(point1.x,point1.y+enemies[i]->getSpeed(),point1.z));
							}
						}
						currentTime = glfwGetTime();
					}
					done = true;
				}
			}
			if (done) {break;}
		}

		


		//left and right keys
		if (glfwGetKey(window, GLFW_KEY_LEFT ) == GLFW_PRESS)
		{
			point1 = glm::vec3(1.0f);
			player->getLocation(vertices,point1);
			if (point1.x > -19.0f)
			{
				player->moveX(vertices, -deltaTime);
			}
		}

		if (glfwGetKey(window, GLFW_KEY_RIGHT ) == GLFW_PRESS)
		{
			point1 = glm::vec3(1.0f);
			player->getLocation(vertices,point1);
			if (point1.x < 19.0f){
				player->moveX(vertices, deltaTime);
			}
		}

		//move and deactivate bullets
		manageBullets(deltaTime);

		//
		if (glfwGetKey(window, GLFW_KEY_UP ) == GLFW_PRESS)
		{

			fire(currentTime, bulletTime, point1);
			
		}

		refreshScore();

		// Compute the MVP matrix from keyboard and mouse input
		computeMVP();

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		drawBuffers();

		lastTime = currentTime;

		if (playerLives < 1)
		{
			url = LOSE;
			break;
		}

		if (enemyCount < 1)
		{
			url = STAGE2;
			break;
		}

	}
	while( true);


	//cleanup and next splash screen
	//clear buffer vextors
	clearVertices();
	showScoreScreen();
	showSplashScreen(url);
}

//level two method
void levelTwo() {
	clearVertices();
	//setup
	level = 2;
	//playerLives = 3;
	setupScore(score, -19, 1);

	char* url;
	// Load the texture
	Texture = loadBMP_custom(LEVEL_TEXTURES);
	
	// Get a handle for our "myTextureSampler" uniform
	TextureID  = glGetUniformLocation(programID, "myTextureSampler");
	//add player model
	player = addPlayer(4, vertices, uvs, normals);
	glm::vec3 point1 = glm::vec3(1.0f);
	
	float y = 30;

	for (float x = -20; x< 15; y+=1.5, x+=2.5){
			enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(x,y,0.0f)));
			enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(x,y+2,0.0f)));
	}
	y+=4.5;
	for (float x = 20; x>-15; y+=1.5, x-=2.5){
		enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(x,y,0.0f)));
		enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(x,y+2,0.0f)));
		enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(x,y+4,0.0f)));
	}
	y+=10;
	for (float x=0; x<=2.5;x+=2.5){
		enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(x,y,0.0f)));
		enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(x,y+2,0.0f)));
		enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(x,y+4,0.0f)));

		enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(-x,y,0.0f)));
		enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(-x,y+2,0.0f)));
		enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(-x,y+4,0.0f)));
	}

	for(float x= 20; x>5; x-=2.5,y+= 1.75){
		enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(x,y,0.0f)));
		enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(x,y+2,0.0f)));
		enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(x,y+4,0.0f)));


		enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(-x,y,0.0f)));
		enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(-x,y+2,0.0f)));
		enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(-x,y+4,0.0f)));
		
	}
	y+=5;

	for(float x= 1; x<17; x+=2.5,y+= 1.75){
		enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(x,y,0.0f)));
		enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(x,y+2,0.0f)));
		enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(x,y+4,0.0f)));


		enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(-x,y,0.0f)));
		enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(-x,y+2,0.0f)));
		enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(-x,y+4,0.0f)));
		
	}
	
	y+=15;
	for(float x= 19; x>0; x-=2.0){
		if (x != 5){
			enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(x,y,0.0f)));
			enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(x,y+2,0.0f)));
			enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(x,y+4,0.0f)));


			enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(-x,y,0.0f)));
			enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(-x,y+2,0.0f)));
			enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(-x,y+4,0.0f)));
		}
	}

	y+=15;
	for(float x= 19; x>0; x-=2.0){
		if (x != 9){
			enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(x,y,0.0f)));
			enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(x,y+2,0.0f)));
			enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(x,y+4,0.0f)));


			enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(-x,y,0.0f)));
			enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(-x,y+2,0.0f)));
			enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(-x,y+4,0.0f)));
		}
	}

	y+=15;
	for(float x= 19; x>0; x-=2.0){
		if (x != 13){
			enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(x,y,0.0f)));
			enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(x,y+2,0.0f)));
			enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(x,y+4,0.0f)));


			enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(-x,y,0.0f)));
			enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(-x,y+2,0.0f)));
			enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(-x,y+4,0.0f)));
		}
	}

	y+=15;
	for(float x= 19; x>0; x-=2.0){
		if (x != 1){
			enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(x,y,0.0f)));
			enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(x,y+2,0.0f)));
			enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(x,y+4,0.0f)));


			enemies.push_back(addCrystal(vertices,uvs,normals,glm::vec3(-x,y,0.0f)));
			enemies.push_back(addRose(vertices,uvs,normals,glm::vec3(-x,y+2,0.0f)));
			enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(-x,y+4,0.0f)));
		}
	}
	enemies.push_back(addClasp(vertices,uvs,normals,glm::vec3(-1.0,y+4,0.0f)));

	//mothership
	y+=30;
	mother = addMothership(vertices,uvs,normals,glm::vec3(0.0f,y,0.0f));
	mother->scale(vertices,glm::vec3(20.0f,20.0f,1.0f));

	enemyCount = enemies.size();

	bullets.clear();
	setupBullets(BULLET_COUNT);

	// Load it into a VBO

	setupBuffers();

	//do loop
	do{
		// glfwGetTime is called only once, the first time this function is called
		static double lastTime = glfwGetTime();
		static double bulletTime = lastTime;

		// Compute time difference between current and last frame
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime) / 2;

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		//move enemy waves and check for collisions
		for (int i = 0; i < enemies.size()-1; i++)
		{
			Entity* current = enemies[i];
			bool done = false;
			if (current->isActive())
			{
				current->moveY(vertices, -deltaTime);

				//check for collision
				if (current->collide(vertices, bullets, player, enemyCount, level, uvs, score)){
					//player loses life
					destroyPlayer(player);
					playerLives = player->getLives();
					if(playerLives > 0)
					{
						//move aliens back up screen and reset clock time
						for (int i = 0; i < enemies.size(); i++){
							if (enemies[i]->isActive()){
								enemies[i]->getLocation(vertices,point1);
								enemies[i]->move(vertices,glm::vec3(point1.x,point1.y+enemies[i]->getSpeed(),point1.z));
							}
						}
						mother->getLocation(vertices,point1);
						mother->move(vertices,glm::vec3(point1.x,point1.y+mother->getSpeed(),point1.z));
						currentTime = glfwGetTime();
					}
					done = true;
				}
			}
			if (done) {break;}
		}

		//move mothership and check for collision
		mother->moveY(vertices, -deltaTime);

		if (mother->collide(vertices, bullets, player, enemyCount, level, uvs)){
			destroyPlayer(player);
			if (stage2PacifistCounter == enemyCount){
				score += stage2PacifistCounter;
			}
			url=WIN;
			break;
		}

		//left and right keys
		if (glfwGetKey(window, GLFW_KEY_LEFT ) == GLFW_PRESS)
		{
			point1 = glm::vec3(1.0f);
			player->getLocation(vertices,point1);
			if (point1.x > -19.0f)
			{
				player->moveX(vertices, -deltaTime);
			}
		}

		if (glfwGetKey(window, GLFW_KEY_RIGHT ) == GLFW_PRESS)
		{
			point1 = glm::vec3(1.0f);
			player->getLocation(vertices,point1);
			if (point1.x < 19.0f){
				player->moveX(vertices, deltaTime);
			}
		}

		//move and deactivate bullets
		for (int i = 0; i < bullets.size(); i++)
		{
			if (bullets[i]->isActive()){
				bullets[i]->moveY(vertices,deltaTime);
				bullets[i]->getLocation(vertices,point1);
				if (point1.y > 29.0f)
				{
					bullets[i]->destroy(vertices, uvs);
				}
			}
		}

		//
		if (glfwGetKey(window, GLFW_KEY_UP ) == GLFW_PRESS)
		{
			fire(currentTime, bulletTime, point1);
		}

		refreshScore();

		// Compute the MVP matrix from keyboard and mouse input
		computeMVP();

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		drawBuffers();

		lastTime = currentTime;
		

		if (playerLives < 1)
		{
			url = LOSE;
			break;
		}

		if (enemyCount < 1)
		{
			url = WIN;
			break;
		}


	} // Check if the ESC key was pressed or the window was closed
	while( true);


	//cleanup and next splash screen
	//clear buffer vextors
	clearVertices();
	showScoreScreen();
	showSplashScreen(url);
}

///////////////////////////////////////////////////////////////




/*************************************************************
						Main method
**************************************************************/
int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 800, 600, "Game", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	if (white) {glClearColor(20.0f, 20.0f, 20.0f, 0.0f);}
	else {glClearColor(0.0f, 0.0f, 0.0f, 0.0f);}

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	//GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	/*GLuint*/ programID = LoadShaders( "TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	/*GLuint*/ MatrixID = glGetUniformLocation(programID, "MVP");
	
	/*************************************************************
						Start Screen Setup
	**************************************************************/
	
	clearVertices();
	showSplashScreen(START);

	closeProgram();

	return 0;
}