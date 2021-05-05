#include <GL/glew.h>
#include <GL/freeglut.h>

//glm library
#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

//gli library
#include <gli/gli.hpp>
#include <gli/gtx/gl_texture2d.hpp>

#include "Model.h"
#include "Shader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
Model *sphere;
Shader shader;
glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;
bool drawTorus = true;
bool spin = false;
float rotation = 0.0f;
GLuint metalTexName,bulletholeTexName, glossTexName,rustyTexName;
GLuint vertexBufferName;
glm::vec4 lightPosition = glm::vec4(0.0f,0.0f,3.0f,1.0f);

/* vertex array data  */
const GLfloat varray[] = {


	0.0f, 0.0f, 1.0f, /* normal */
	0.0f, 0.0f, 0.0f,1.0f,      /* lower left vertex*/
	0.0f,0.0f, //texture coordinate


	0.0f, 0.0f, 1.0f, /* normal */
	2.0f, 0.0f,0.0f,1.0f,         /* lower right vertex */
	1.0f,0.0f, //texture coordinate


	0.0f, 0.0f, 1.0f, /* normal */
	0.0f, 2.0f, 0.0f,1.0f ,       /* upper leftvertex */
	0.0f, 1.0f, //texture coordinate

	0.0f, 0.0f, 1.0f, /* normal */
	0.0f, 2.0f, 0.0f,1.0f ,       /* upper leftvertex */
	0.0f, 1.0f, //texture coordinate

	0.0f, 0.0f, 1.0f, /* normal */
	2.0f, 0.0f,0.0f,1.0f,         /* lower right vertex */
	1.0f,0.0f, //texture coordinate

	0.0f, 0.0f, 1.0f, /* normal */
	2.0f, 2.0f,0.0f,1.0f,         /* upper right vertex */
	1.0f,1.0f //texture coordinate
};

/* compile-time constants */
enum {
	numTextureComponents = 2, // there are 2 floats to define a texture coordinate
	numNormalComponents = 3, // there are 3 floats to define a normal
	numVertexComponents = 4, // there are 4 floats to define the position of a vertex
	stride = sizeof(GLfloat) * (numTextureComponents+numNormalComponents + numVertexComponents), // the space between all the information for a vertex
	numElements = sizeof(varray) / stride // number of vertices in the array
};
/* report GL errors, if any, to stderr */
void checkError(const char *functionName)
{
	GLenum error;
	while (( error = glGetError() ) != GL_NO_ERROR) {
		std::cerr << "GL error " << error << " detected in " << functionName << std::endl;
	}
}
void initTexture(){
// the three textures you need are already loaded and ready to go
	glActiveTexture(GL_TEXTURE0);
	metalTexName = gli::createTexture2D("textures/texturedMetal.tga"); 
	glBindTexture(GL_TEXTURE_2D, metalTexName);
	
	glActiveTexture(GL_TEXTURE0+1);
	bulletholeTexName = gli::createTexture2D("textures/bulletholesmall.dds");
	glBindTexture(GL_TEXTURE_2D, bulletholeTexName);

	glActiveTexture(GL_TEXTURE0+2);
	glossTexName = gli::createTexture2D("textures/gloss.tga");
	glBindTexture(GL_TEXTURE_2D, glossTexName);

	
	glActiveTexture(GL_TEXTURE0+3);
	rustyTexName = gli::createTexture2D("textures/rustyMetal.dds");
	glBindTexture(GL_TEXTURE_2D, rustyTexName);
}

void initShader(void)
{
	shader.InitializeFromFile("shaders/phong3.vert", "shaders/phong3.frag");
	//shader.AddAttribute("vertexPosition");
	//shader.AddAttribute("vertexNormal");
	//shader.AddAttribute("texcoord");
	checkError ("initShader");
}
void initBuffer(){
	glGenBuffers (1, &vertexBufferName);
	glBindBuffer (GL_ARRAY_BUFFER, vertexBufferName);
	glBufferData (GL_ARRAY_BUFFER, sizeof(varray), varray, GL_STATIC_DRAW);
}

void initRendering(void)
{
	glClearColor (0.117f, 0.565f, 1.0f, 0.0f); // Dodger Blue
	checkError ("initRendering");
}

void init(void) 
{	
	// Load identity matrix into model matrix.
	model = glm::mat4();
	initBuffer();
	initTexture();
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

const GLvoid *bufferObjectPtr (GLsizei index)
{
	;
	return (const GLvoid *) (((char *) NULL) + index);
}
void drawQuad(){
	glEnableVertexAttribArray(shader["vertexPosition"]); // Enable vertex attribute.
	glEnableVertexAttribArray(shader["vertexNormal"]); // Enable normal attribute.
	glEnableVertexAttribArray(shader["vertexTextureCoordinates"]); // Enable texture coordiante attribute.
		
	glBindBuffer (GL_ARRAY_BUFFER, vertexBufferName);
	glVertexAttribPointer (shader["vertexNormal"], numNormalComponents, GL_FLOAT, GL_FALSE,
		stride, bufferObjectPtr (0));
	glVertexAttribPointer (shader["vertexPosition"], numVertexComponents, GL_FLOAT, GL_FALSE,
		stride, bufferObjectPtr (sizeof(GLfloat) * (numNormalComponents) ));
	glVertexAttribPointer (shader["vertexTextureCoordinates"], numTextureComponents, GL_FLOAT, GL_FALSE,
		stride, bufferObjectPtr (sizeof(GLfloat)*(numNormalComponents+numVertexComponents)));
	glDrawArrays(GL_TRIANGLES, 0, numElements);
	
	glDisableVertexAttribArray(shader["vertexPosition"]); // Disable vertex attribute.
	glDisableVertexAttribArray(shader["vertexNormal"]); // Disable normal attribute.
	glDisableVertexAttribArray(shader["vertexTextureCoordinates"]); 


}
void display(void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Perspective projection matrix.
	projection = glm::perspective(45.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 1000.0f);

	// View matrix positioned back 5 on the z axis, looking into the screen.
	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	//a moving light may help with testing your gloss map
	lightPosition = glm::rotate(.1f,0.0f, 1.0f, 0.0f) * lightPosition;

	//blending currently on to work with transparency (you can turn it off if you arent using the blending func in your solution)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader.Activate(); // Bind shader.
	shader.SetUniform("Projection", projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, metalTexName);

//a white light
	shader.SetUniform("lightPosition", view*lightPosition);
	shader.SetUniform("lightDiffuse", glm::vec4(1.0, 1.0, 1.0, 1.0));
	shader.SetUniform("lightSpecular", glm::vec4(1.0, 1.0, 1.0, 1.0));
	shader.SetUniform("lightAmbient", glm::vec4(0.0, 0.0, 0.0, 1.0));

//surface reflectance properties
	shader.SetUniform("surfaceDiffuse", glm::vec4(1.0, 1.0, 1.0, 1.0));
	shader.SetUniform("surfaceSpecular", glm::vec4(1.0, 1.0, 1.0, 1.0));
	shader.SetUniform("surfaceAmbient", glm::vec4(1.0, 1.0, 1.0, 1.0));
	shader.SetUniform("shininess", 120.0f);
	shader.SetUniform("surfaceEmissive", glm::vec4(0.0, 0.0, 0.0, 1.0));

// the texture variables in the shader are bound to the appropriate texture units
	//shader.SetUniform("bulletholetex", 1);
	//shader.SetUniform("glosstex", 3);
//	shader.SetUniform("metaltex",2);	
	model = glm::translate(-0.5f, -.5f,-2.0f);
	shader.SetUniform("ModelView", view * model );
	//drawQuad(); // a rusty looking quad, when you shoot through the other quad in the lower left corner, you should be able to see through to this one

	shader.SetUniform("ModelView", view);
	shader.SetUniform("diffuseTexture", 0);
	//drawQuad(); // the metal quad you are going to shoot through
		
sphere->render(view * model * glm::rotate(90.0f,1.0f,0.0f,0.0f),projection, true); // Render current active model.
	
	shader.DeActivate(); // Unbind shader.
	glutSwapBuffers(); // Swap the buffers.

	checkError ("display");
}

void idle()
{
	glFlush ();
	glutPostRedisplay();
}


void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	
	checkError ("reshape");

}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE| GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize (800, 600); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	glewInit();
	dumpInfo ();
	init ();
	glutDisplayFunc(display); 
	glutIdleFunc(idle); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc (keyboard);
	glEnable(GL_DEPTH_TEST);
sphere = new Model(&shader,"models/texcube.obj", "models/");
	glutMainLoop();

	return 0;
}