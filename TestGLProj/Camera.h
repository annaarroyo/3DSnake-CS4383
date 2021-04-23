#pragma once
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(){};

	virtual bool OnKeyboard(int Key){return false;};

	virtual void OnMouse(int x, int y){};

	virtual void OnRender(){};

    glm::vec3& GetPos() 
    {
        return m_pos;
    }

    glm::vec3& GetTarget() 
    {
        return m_pos+m_lookat;
    }

    glm::vec3& GetUp() 
    {
        return m_up;
    }

protected:
	glm::vec3 m_pos;    
	glm::vec3 m_lookat;
    glm::vec3 m_up;

};
