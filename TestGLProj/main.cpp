#include <GL/glew.h>
#include <GL/freeglut.h>

//glm library
#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"
#include "Shader.h"
#include "QuatCamera.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <cmath>
#include <ctgmath>

#define START_X 0.0f
#define START_Y -1.5f
#define START_Z 0.0f

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

#define SNAKE_LENGTH 4


short direction = RIGHT;

void controlMotion(void);
void resetFactors(short dir);
void alignVertical(void);
bool checkIfVertical(void);
void moveSnakeUp(void);

glm::mat4 snakeModel;
glm::mat4 snakeHeadModel;
glm::mat4 snakeTranslateModel;
glm::mat4 rotateCube;
glm::mat4 motion;

Shader shader;
Model *plane;

glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;

glm::vec4 lightPosition = glm::vec4(0.0f,3.0f,0.0f,1.0f); 
glm::vec3 position(0.0f, 65.0f, 25.0f);
glm::vec3 focus(0.0f, 0.0f, 0.0f);

float rightFactor = 0.0f;
float leftFactor = 0.0f;
float upFactor = 0.0f;
float downFactor = 0.0f;

bool startMotion = false;
bool turning = false;

QuatCamera * camera;

using namespace std;
// snakeObj will contain the model object and positions for each part of the snake
struct snakeObj {
	Model* model;
	float xPos;
	float yPos;
	float zPos;
} snakeObject;

// snake contains a vector of all the snakeObjs
struct snake {
	std::vector<snakeObj> snakeModels;
} game;


/* report GL errors, if any, to stderr */
void checkError(const char *functionName)
{
	GLenum error;
	while (( error = glGetError() ) != GL_NO_ERROR) {
	  std::cerr << "GL error " << error << " detected in " << functionName << std::endl;
	}
}

void initShader(void)
{
	shader.InitializeFromFile("shaders/phong3.vert", "shaders/phong3.frag");
	//shader.AddAttribute("vertexPosition");
	//shader.AddAttribute("vertexNormal");

	checkError ("initShader");
}

void initRendering(void)
{
	glClearColor (0.117f, 0.565f, 1.0f, 0.0f); // Dodger Blue
	checkError ("initRendering");
}

void init(void) 
{	
	// View  positioned back -5 on the z axis, looking into the screen.
	glm::vec3 initpos = glm::vec3(0.0f, 0.0f, -20.0f);
	glm::vec3 initlookatpnt = glm::vec3(.0f, .0f, -1.0f);
	camera = new QuatCamera(800,600,initpos, initlookatpnt, glm::vec3(0.0f, 1.0f, 0.0f));
	// Perspective projection matrix.
	projection = glm::perspective(45.0f, 800.0f/600.0f, 1.0f, 1000.0f);

	// Load identity matrix into model matrix.
	//model = glm::mat4();

	initShader ();
	initRendering ();
}

void dumpInfo(void)
{
	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
	checkError ("dumpInfo");
}

void display(void)
{

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	camera->OnRender();
	//view = glm::lookAt(camera->GetPos(), camera->GetLookAtPoint(), camera->GetUp()); // first person cam
		
	view = glm::lookAt(position, focus, glm::vec3(0.0f, 1.0f, 0.0f));
	bool useMat = false;
	shader.Activate(); // Bind shader.
		 
	// point light
	shader.SetUniform("lightPosition", view * lightPosition);
	shader.SetUniform("lightDiffuse", glm::vec4(1.5, 1.0, 1.0, 1.0));
	shader.SetUniform("lightSpecular", glm::vec4(1.5, 1.0, 1.0, 1.0));
	shader.SetUniform("lightAmbient", glm::vec4(1.5, 1.0, 1.0, 1.0));

	// plane model
	plane->setOverrideSpecularMaterial( glm::vec4(.70, 0.70, 0.70, 1.0));
	plane->setOverrideDiffuseMaterial( glm::vec4(1.0, 0.0, 0.0, 1.0));
	plane->setOverrideAmbientMaterial(  glm::vec4(0.2 , 0.0, 0.0, 1.0));
	plane->setOverrideSpecularMaterial( glm::vec4(1.0, 1.0, 1.0, 1.0));
	plane->setOverrideSpecularShininessMaterial( 90.0f);
	plane->setOverrideEmissiveMaterial(  glm::vec4(0.0, 0.0, 0.0, 1.0));
	plane->render(view*glm::translate(0.0f,-2.0f,0.0f)*glm::scale(20.0f,20.0f,20.0f), projection, useMat);


	for (int i = 0; i < SNAKE_LENGTH; i++) {
		snakeObj* snakePart = &game.snakeModels.at(i);
		snakePart->model->setOverrideSpecularMaterial(glm::vec4(.70, 0.70, 0.70, 1.0));
		snakePart->model->setOverrideDiffuseMaterial(glm::vec4(1.0, 0.0, 1.0, 1.0));
		snakePart->model->setOverrideAmbientMaterial(glm::vec4(0.2, 0.0, 0.0, 1.0));
		snakePart->model->setOverrideSpecularMaterial(glm::vec4(1.0, 1.0, 1.0, 1.0));
		snakePart->model->setOverrideSpecularShininessMaterial(90.0f);
		snakePart->model->setOverrideEmissiveMaterial(glm::vec4(0.0, 0.0, 0.0, 1.0));

	}
	// set rotate matrix for cubes to be aligned
	rotateCube = glm::rotate(125.0f, 1.0f, 1.0f, 0.0f) * glm::rotate(16.0f, 0.0f, 0.0f, 1.0f); // rotate cube to be level with plane

	if (startMotion) {
			// call function that will control the snake's motion
			controlMotion();
		}
	else {
		for (int i = 0; i < SNAKE_LENGTH; i++) {
			snakeObj* snakePart = &game.snakeModels.at(i);
			snakeModel = glm::translate(snakePart->xPos, snakePart->yPos, snakePart->zPos) * rotateCube * glm::scale(0.5f, 0.5f, 0.5f);
			snakePart->model->render(view * snakeModel, projection, useMat);
		}
	}	

	//starts game by pressing 'd'
	
	/*
	game.snakeModels.at(2).model->setOverrideSpecularMaterial(glm::vec4(.70, 0.70, 0.70, 1.0));
	game.snakeModels.at(2).model->setOverrideDiffuseMaterial(glm::vec4(1.0, 0.0, 1.0, 1.0));
	game.snakeModels.at(2).model->setOverrideAmbientMaterial(glm::vec4(0.2, 0.0, 0.0, 1.0));
	game.snakeModels.at(2).model->setOverrideSpecularMaterial(glm::vec4(1.0, 1.0, 1.0, 1.0));
	game.snakeModels.at(2).model->setOverrideSpecularShininessMaterial(90.0f);
	game.snakeModels.at(2).model->setOverrideEmissiveMaterial(glm::vec4(0.0, 0.0, 0.0, 1.0));
	snakeBody2Model = glm::translate(1.7f, 0.0f, 0.0f) * snakeHeadModel;

	// render body2 at third position in vector
	game.snakeModels.at(2).model->render(view * snakeBody2Model, projection, useMat);

	game.snakeModels.at(3).model->setOverrideSpecularMaterial(glm::vec4(.70, 0.70, 0.70, 1.0));
	game.snakeModels.at(3).model->setOverrideDiffuseMaterial(glm::vec4(1.0, 0.0, 1.0, 1.0));
	game.snakeModels.at(3).model->setOverrideAmbientMaterial(glm::vec4(0.2, 0.0, 0.0, 1.0));
	game.snakeModels.at(3).model->setOverrideSpecularMaterial(glm::vec4(1.0, 1.0, 1.0, 1.0));
	game.snakeModels.at(3).model->setOverrideSpecularShininessMaterial(90.0f);
	game.snakeModels.at(3).model->setOverrideEmissiveMaterial(glm::vec4(0.0, 0.0, 0.0, 1.0));
	snakeBody3Model = glm::translate(2.55f, 0.0f, 0.0f) * snakeHeadModel;

	// render body3 at fourth position in vector
	game.snakeModels.at(3).model->render(view * snakeBody3Model, projection, useMat);

	game.snakeModels.at(4).model->setOverrideSpecularMaterial(glm::vec4(.70, 0.70, 0.70, 1.0));
	game.snakeModels.at(4).model->setOverrideDiffuseMaterial(glm::vec4(1.0, 0.0, 1.0, 1.0));
	game.snakeModels.at(4).model->setOverrideAmbientMaterial(glm::vec4(0.2, 0.0, 0.0, 1.0));
	game.snakeModels.at(4).model->setOverrideSpecularMaterial(glm::vec4(1.0, 1.0, 1.0, 1.0));
	game.snakeModels.at(4).model->setOverrideSpecularShininessMaterial(90.0f);
	game.snakeModels.at(4).model->setOverrideEmissiveMaterial(glm::vec4(0.0, 0.0, 0.0, 1.0));
	snakeTailModel = glm::translate(3.4f, 0.0f, 0.0f) * snakeHeadModel;

	// render tail at fifth position in vector
	game.snakeModels.at(4).model->render(view * snakeTailModel, projection, useMat);*/
	
	glutSwapBuffers(); // Swap the buffers.

	checkError ("display");
}

 void controlMotion() {
	snakeObj *snakeHead = &game.snakeModels.at(0);
	float prevZ;
	float prevX;

	switch (direction) {
	case UP: 
		
		if(!checkIfVertical()) { // checks if the cubes are all vertical
			alignVertical(); // move snake to be vertical
		}
		else {
			moveSnakeUp(); // start motion upwards if all cubes are vertical
			resetFactors(UP); // resets speed factors of down, left, and down to 0.0f
		}
		break;

	case RIGHT:
		// TODO: implement same code structure as case UP
		rightFactor += 0.01f;
		for (int i = SNAKE_LENGTH - 1; i > 0; i--) {
			snakeObj* snakeBack = &game.snakeModels.at(i);
			snakeObj* snakeFront = &game.snakeModels.at(i - 1);
			if (snakeBack->xPos == snakeHead->xPos && snakeBack->zPos != snakeHead->zPos) {
				prevZ = snakeFront->zPos;
				snakeBack->zPos = prevZ;
			}
			else {
				prevX = snakeFront->xPos;
				snakeBack->xPos = prevX - 0.85f;

			}
			//printf("RIGHT i: %d x: %f y: %f z: %f\n", i, snakeBack->xPos, snakeBack->yPos, snakeBack->zPos);

			snakeModel = glm::translate(snakeBack->xPos, snakeBack->yPos, snakeBack->zPos) * rotateCube * glm::scale(0.5f, 0.5f, 0.5f);
			snakeBack->model->render(view * snakeModel, projection, false);
		}

		snakeHead->xPos += rightFactor;
		snakeHeadModel = glm::translate(snakeHead->xPos, snakeHead->yPos, snakeHead->zPos) * rotateCube * glm::scale(0.5f, 0.5f, 0.5f);
		snakeHead->model->render(view * snakeHeadModel, projection, false);
		
		resetFactors(RIGHT);
		break;
	case DOWN:
		downFactor += 0.00001f;
		// TODO: implement same code structure as case UP

		resetFactors(DOWN);
		break;
	case LEFT:
		leftFactor -= 0.00001f;
		// TODO: implement same code structure as case UP

		resetFactors(LEFT);
		break;
	}
}


 bool checkIfVertical(void) 
{
	 float x;
	 bool isVertical = true;

	 // get x value of the head of snake
	 snakeObj* snakeHead = &game.snakeModels.at(0);
	 x = snakeHead->xPos;

	 // check is all the cubes are aligned vertically 
	 for (int i = 1; i < SNAKE_LENGTH; i++) {
		 snakeObj *snakePart = &game.snakeModels.at(i);
		 if (snakePart->xPos != x)
		 {
			 return false;
		 }
	 }

	 // check to see if cubes are done rendering
	 for (int i = 0; i < SNAKE_LENGTH - 1; i++) {
		 snakeObj* snakePart = &game.snakeModels.at(i);
		 snakeObj* snakeFront = &game.snakeModels.at(i + 1);
		 if (snakePart->zPos == snakeFront->zPos) {
			 return false;
		 }
	 }
	 return isVertical;
}
void alignVertical(void)
{
	snakeObj* snakeHead = &game.snakeModels.at(0);
	float prevZ;
	float prevX;

	for (int i = SNAKE_LENGTH - 1; i > 0; i--) {
		snakeObj* snakeBack = &game.snakeModels.at(i);
		snakeObj* snakeFront = &game.snakeModels.at(i - 1);

		if (snakeBack->xPos != snakeHead->xPos && snakeBack->zPos == snakeFront->zPos) {
			prevX = snakeFront->xPos;
			snakeBack->xPos = prevX;
		}
		else {
			prevZ = snakeFront->zPos;
			snakeBack->zPos = prevZ;
		}
		snakeModel = glm::translate(snakeBack->xPos, snakeBack->yPos, snakeBack->zPos) * rotateCube * glm::scale(0.5f, 0.5f, 0.5f);
		snakeBack->model->render(view * snakeModel, projection, false);
		//printf("UP i: %d x: %f y: %f z: %f\n", i, snakeBack->xPos, snakeBack->yPos, snakeBack->zPos);

	}

	// offset head of snake
	snakeHead->zPos -= 0.85f;
	snakeHeadModel = glm::translate(snakeHead->xPos, snakeHead->yPos, snakeHead->zPos) * rotateCube * glm::scale(0.5f, 0.5f, 0.5f);

	snakeHead->model->render(view * snakeHeadModel, projection, false);
	//printf("HEAD x: %f y: %f z: %f\n", snakeHead->xPos, snakeHead->yPos, snakeHead->zPos);

}

void moveSnakeUp(void)
{
	snakeObj* snakeHead = &game.snakeModels.at(0);
	float prevZ;
	//upFactor -= 0.01f;


	for (int i = SNAKE_LENGTH - 1; i > 0; i--) {
		snakeObj* snakeBack = &game.snakeModels.at(i);
		snakeObj* snakeFront = &game.snakeModels.at(i - 1);
		prevZ = snakeFront->zPos;
		snakeBack->zPos = prevZ - .85f;

		snakeModel = glm::translate(snakeBack->xPos, snakeBack->yPos, snakeBack->zPos) * rotateCube * glm::scale(0.5f, 0.5f, 0.5f);
		snakeBack->model->render(view * snakeModel, projection, false);
		//printf("UP i: %d x: %f y: %f z: %f\n", i, snakeBack->xPos, snakeBack->yPos, snakeBack->zPos);

	}

	// start up motion once everything is aligned or vertical
	//printf("UP HEAD x: %f y: %f z: %f\n", snakeHead->xPos, snakeHead->yPos, snakeHead->zPos);
	snakeHead->zPos += -0.85f;
	snakeHeadModel = glm::translate(snakeHead->xPos, snakeHead->yPos, snakeHead->zPos) * rotateCube * glm::scale(0.5f, 0.5f, 0.5f);
	snakeHead->model->render(view * snakeHeadModel, projection, false);
	//printf("UP HEAD x: %f y: %f z: %f\n", snakeHead->xPos, snakeHead->yPos, snakeHead->zPos);

}

void resetFactors(short dir) {
	switch (dir) {
	case UP:
		rightFactor = 0.0f;
		leftFactor = 0.0f;
		downFactor = 0.0f;
		break;
	case DOWN:
		rightFactor = 0.0f;
		leftFactor = 0.0f;
		upFactor = 0.0f;
		break;
	case LEFT:
		rightFactor = 0.0f;
		downFactor = 0.0f;
		upFactor = 0.0f;
		break;
	case RIGHT:
		leftFactor = 0.0f;
		downFactor = 0.0f;
		upFactor = 0.0f;
		break;
	}
}

void idle()
{
	glutPostRedisplay();
}


void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	checkError ("reshape");
}

void specialKeyboard(int Key, int x, int y)
{
    camera->OnKeyboard(Key);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'w':
		if (startMotion == false || direction == DOWN) {
			break;
		}
		direction = UP;
		break;
	case 'a':
		if (startMotion == false || direction == RIGHT) {
			break;
		}
		direction = LEFT;
		break;
	case 's':
		if (startMotion == false || direction == UP) {
			break;
		}
		direction = DOWN;
		break;
	case 'd':
		if (startMotion == false || direction == LEFT) {
			break;
		}
		direction = RIGHT;
	case 32: // press space to start game
		startMotion = true;
		break;
	}
}

static void passiveMouse(int x, int y)
{
 //  camera->OnMouse(x, y);
}

void timer(int)
{
	/* update animation */
	glutPostRedisplay();
	glutTimerFunc(1000.0 / 4.0, timer, 0);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE| GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (800, 600); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Lighting and Quaternion Camera Demo");

	glewInit();
	dumpInfo ();
	init ();

	glutDisplayFunc(display); 
	//glutIdleFunc(idle); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc (keyboard);
	glutSpecialFunc(specialKeyboard);
    glutPassiveMotionFunc(passiveMouse);
	glutTimerFunc(1000.0 / 4.0, timer, 0);
	
	glEnable(GL_DEPTH_TEST);

	plane = new Model(&shader, "models/plane.obj", "models/");
	
	// starter snake models
	snakeObj sHead;
	snakeObj sBody1;
	snakeObj sBody2;
	snakeObj sBody3;
	snakeObj sTail;

	// manually set up the starter models fof the snake
	sHead.model = new Model(&shader, "models/cube.obj", "models/");
	sHead.xPos = START_X;
	sHead.yPos = START_Y;
	sHead.zPos = START_Z;


	sBody1.model = new Model(&shader, "models/cube.obj", "models/");
	sBody1.xPos = -.85f;
	sBody1.yPos = -1.5f;
	sBody1.zPos = 0.0f;

	sBody2.model = new Model(&shader, "models/cube.obj", "models/");
	sBody2.xPos = -1.7f;
	sBody2.yPos = -1.5f;
	sBody2.zPos = 0.0f;

	sBody3.model = new Model(&shader, "models/cube.obj", "models/");
	sBody3.xPos = -2.55f;
	sBody3.yPos = -1.5f;
	sBody3.zPos = 0.0f;

	sTail.model = new Model(&shader, "models/cube.obj", "models/");

	game.snakeModels.push_back(sHead);
	game.snakeModels.push_back(sBody1);
	game.snakeModels.push_back(sBody2);
	game.snakeModels.push_back(sBody3);
	//game.snakeModels.push_back(sTail);


	// TODO: automate this model loading process for when it eats the food, to increase size by 1 and add a model


	glutMainLoop();

   return 0;
}