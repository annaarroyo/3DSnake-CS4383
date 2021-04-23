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

Shader shader;
Model *plane;
Model *snakeHead;
Model* snakeBody1;
Model* snakeBody2;
Model* snakeBody3;
Model* snakeTail;


glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;
glm::mat4 snakeHeadModel;
glm::mat4 snakeBody1Model;
glm::mat4 snakeBody2Model;
glm::mat4 snakeBody3Model;
glm::mat4 snakeTailModel;
glm::mat4 rotateCube;
glm::mat4 motion;

glm::vec4 lightPosition = glm::vec4(0.0f,3.0f,0.0f,1.0f);
glm::vec3 position(0.0f, 15.0f, 55.0f);
glm::vec3 focus(0.0f, 0.0f, 0.0f);

float motionFactor = 0.0f;

bool startMotion = false;

QuatCamera * camera;

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
	//view = glm::lookAt(camera->GetPos(), camera->GetLookAtPoint(), camera->GetUp());
		
	view = glm::lookAt(position, focus, glm::vec3(0.0f, 1.0f, 0.0f));
	bool useMat = false;
	shader.Activate(); // Bind shader.
		 
	// point light
	shader.SetUniform("lightPosition", view * lightPosition);
	shader.SetUniform("lightDiffuse", glm::vec4(1.5, 1.0, 1.0, 1.0));
	shader.SetUniform("lightSpecular", glm::vec4(1.5, 1.0, 1.0, 1.0));
	shader.SetUniform("lightAmbient", glm::vec4(1.5, 1.0, 1.0, 1.0));

	plane->setOverrideSpecularMaterial( glm::vec4(.70, 0.70, 0.70, 1.0));
	plane->setOverrideDiffuseMaterial( glm::vec4(1.0, 0.0, 0.0, 1.0));
	plane->setOverrideAmbientMaterial(  glm::vec4(0.2 , 0.0, 0.0, 1.0));
	plane->setOverrideSpecularMaterial( glm::vec4(1.0, 1.0, 1.0, 1.0));
	plane->setOverrideSpecularShininessMaterial( 90.0f);
	plane->setOverrideEmissiveMaterial(  glm::vec4(0.0, 0.0, 0.0, 1.0));
	plane->render(view*glm::translate(0.0f,-2.0f,0.0f)*glm::scale(20.0f,20.0f,20.0f), projection, useMat);

	
	snakeHead->setOverrideSpecularMaterial(glm::vec4(.70, 0.70, 0.70, 1.0));
	snakeHead->setOverrideDiffuseMaterial(glm::vec4(1.0, 0.0, 1.0, 1.0));
	snakeHead->setOverrideAmbientMaterial(glm::vec4(0.2, 0.0, 0.0, 1.0));
	snakeHead->setOverrideSpecularMaterial(glm::vec4(1.0, 1.0, 1.0, 1.0));
	snakeHead->setOverrideSpecularShininessMaterial(90.0f);
	snakeHead->setOverrideEmissiveMaterial(glm::vec4(0.0, 0.0, 0.0, 1.0));
	rotateCube = glm::rotate(125.0f, 1.0f, 1.0f, 0.0f) * glm::rotate(16.0f, 0.0f, 0.0f, 1.0f);

	//starts game by pressing 'w'
	if (startMotion) {
		motionFactor += 0.005f;
		snakeHeadModel = glm::translate(motionFactor, -1.5f, 0.0f) * rotateCube * glm::scale(0.5f, 0.5f, 0.5f);
	}
	else {
		snakeHeadModel = glm::translate(0.0f, -1.5f, 0.0f) * rotateCube * glm::scale(0.5f, 0.5f, 0.5f);
	}
	snakeHead->render(view * snakeHeadModel, projection, useMat);

	snakeBody1->setOverrideSpecularMaterial(glm::vec4(.70, 0.70, 0.70, 1.0));
	snakeBody1->setOverrideDiffuseMaterial(glm::vec4(1.0, 0.0, 1.0, 1.0));
	snakeBody1->setOverrideAmbientMaterial(glm::vec4(0.2, 0.0, 0.0, 1.0));
	snakeBody1->setOverrideSpecularMaterial(glm::vec4(1.0, 1.0, 1.0, 1.0));
	snakeBody1->setOverrideSpecularShininessMaterial(90.0f);
	snakeBody1->setOverrideEmissiveMaterial(glm::vec4(0.0, 0.0, 0.0, 1.0));
	snakeBody1Model = glm::translate(0.85f, 0.0f, 0.0f) * snakeHeadModel;
	snakeBody1->render(view * snakeBody1Model, projection, useMat);

	snakeBody2->setOverrideSpecularMaterial(glm::vec4(.70, 0.70, 0.70, 1.0));
	snakeBody2->setOverrideDiffuseMaterial(glm::vec4(1.0, 0.0, 1.0, 1.0));
	snakeBody2->setOverrideAmbientMaterial(glm::vec4(0.2, 0.0, 0.0, 1.0));
	snakeBody2->setOverrideSpecularMaterial(glm::vec4(1.0, 1.0, 1.0, 1.0));
	snakeBody2->setOverrideSpecularShininessMaterial(90.0f);
	snakeBody2->setOverrideEmissiveMaterial(glm::vec4(0.0, 0.0, 0.0, 1.0));
	snakeBody2Model = glm::translate(1.7f, 0.0f, 0.0f) * snakeHeadModel;
	snakeBody2->render(view * snakeBody2Model, projection, useMat);

	snakeBody3->setOverrideSpecularMaterial(glm::vec4(.70, 0.70, 0.70, 1.0));
	snakeBody3->setOverrideDiffuseMaterial(glm::vec4(1.0, 0.0, 1.0, 1.0));
	snakeBody3->setOverrideAmbientMaterial(glm::vec4(0.2, 0.0, 0.0, 1.0));
	snakeBody3->setOverrideSpecularMaterial(glm::vec4(1.0, 1.0, 1.0, 1.0));
	snakeBody3->setOverrideSpecularShininessMaterial(90.0f);
	snakeBody3->setOverrideEmissiveMaterial(glm::vec4(0.0, 0.0, 0.0, 1.0));
	snakeBody3Model = glm::translate(2.55f, 0.0f, 0.0f) * snakeHeadModel;
	snakeBody3->render(view * snakeBody3Model, projection, useMat);

	snakeTail->setOverrideSpecularMaterial(glm::vec4(.70, 0.70, 0.70, 1.0));
	snakeTail->setOverrideDiffuseMaterial(glm::vec4(1.0, 0.0, 1.0, 1.0));
	snakeTail->setOverrideAmbientMaterial(glm::vec4(0.2, 0.0, 0.0, 1.0));
	snakeTail->setOverrideSpecularMaterial(glm::vec4(1.0, 1.0, 1.0, 1.0));
	snakeTail->setOverrideSpecularShininessMaterial(90.0f);
	snakeTail->setOverrideEmissiveMaterial(glm::vec4(0.0, 0.0, 0.0, 1.0));
	snakeTailModel = glm::translate(3.4f, 0.0f, 0.0f) * snakeHeadModel;
	snakeTail->render(view * snakeTailModel, projection, useMat);
	
	glutSwapBuffers(); // Swap the buffers.

	checkError ("display");
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
		startMotion = true;
		break;
   }
}

static void passiveMouse(int x, int y)
{
 //  camera->OnMouse(x, y);
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
	glutIdleFunc(idle); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc (keyboard);
	glutSpecialFunc(specialKeyboard);
    glutPassiveMotionFunc(passiveMouse);
	
	glEnable(GL_DEPTH_TEST);

	plane = new Model(&shader,"models/plane.obj",  "models/");
	snakeHead = new Model(&shader, "models/cube.obj", "models/");
	snakeBody1 = new Model(&shader, "models/cube.obj", "models/");
	snakeBody2 = new Model(&shader, "models/cube.obj", "models/");
	snakeBody3 = new Model(&shader, "models/cube.obj", "models/");
	snakeTail = new Model(&shader, "models/cube.obj", "models/");

	glutMainLoop();

   return 0;
}