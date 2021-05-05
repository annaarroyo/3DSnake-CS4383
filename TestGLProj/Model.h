#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "tiny_obj_loader.h"
#include <string>
#include "Shader.h"

/*This class represents a mesh - a collection of triangles. It stores a list of vertices, which are interpreted by openGL as every 3 vertices makes a triangle.*/
class Model{

public:

	/*This struct represents a vertex in 3D space*/
	struct Vertex 
	{
		Vertex() : Position(0.0f,0.0f,0.0f), Normal(0.0f,0.0f,1.0f){} // default values
		glm::vec3 Position; /// x,y,z 
		glm::vec3 Normal; // determines which way the vertex is 'facing'

	};
	struct Textures {
		GLuint ambient;
		GLuint diffuse;
		GLuint specular;
		GLuint normal;
	};

	/*
	@param shader - a pointer to the shader program to use
	@param filename - the name of the file with the relative directory included
	@param materialPath - [optional] the relative path to the mtl files. NOTE: if your obj has a mtl file associated with it, this is not optional!
	*/
	Model(Shader *shader,  const char* filename, const char* materialPath = NULL); 
	~Model(void){} // default destructor
	void render(glm::mat4 ModelView, glm::mat4 Projection, bool useObjMaterial); // render the model
	void setOverrideDiffuseMaterial(glm::vec4 color);
	void setOverrideSpecularMaterial(glm::vec4 color);
	void setOverrideSpecularShininessMaterial(float shine);
	void setOverrideAmbientMaterial(glm::vec4 color);
	void setOverrideEmissiveMaterial(glm::vec4 color);
	
private:
	static std::map<std::string, GLuint> textureManager;
	glm::vec4 diffuseOverride;
	glm::vec4 specularOverride;
	float shininessOverride;
	glm::vec4 ambientOverride;
	glm::vec4 emissiveOverride;
	Shader *m_shader; // shader program
	std::vector<tinyobj::shape_t> shapes; //a list of meshes and their respective materials
	std::vector<struct Textures> textures;
	std::vector<GLuint> m_VBO;// vertex buffer IDs, each corresponding to a shape
	std::vector<GLuint> m_NBO;// normal buffer IDs, each corresponding to a shape
	std::vector<GLuint> m_TCBO;// texture coord buffer IDs, each corresponding to a shape

	std::vector<GLuint> m_IBO;// index buffer IDs, each corresponding to a shape

	void updateBuffers(); //initialize your VBO and update when triangles are added
	GLuint LoadTexture(const char* filename);

	




	


};

