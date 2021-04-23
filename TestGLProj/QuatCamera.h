#pragma once
#include "camera.h"

class QuatCamera	
{
public:
    QuatCamera(int WindowWidth, int WindowHeight);

	QuatCamera(int WindowWidth, int WindowHeight, const glm::vec3& Pos, const glm::vec3& Target, const glm::vec3& Up);
	
	 bool OnKeyboard(int Key);

	 void OnMouse(int x, int y);

	 void OnRender();

	glm::vec3 GetPos(); 
    
    glm::vec3 GetLookAtPoint(); 

    glm::vec3 GetUp(); 
   
	void setFlying(bool isflying);

private:
	glm::vec3 m_pos;    
	glm::vec3 m_lookAtDir;
    glm::vec3 m_up;
    void Init();
    void Update();

	int m_windowWidth;
    int m_windowHeight;

    float m_AngleH;
    float m_AngleV;
/*
    bool m_OnUpperEdge;
    bool m_OnLowerEdge;
    bool m_OnLeftEdge;
    bool m_OnRightEdge;
*/
	bool isFlying;
//i32vec2? - jpq
	glm::vec2 m_mousePos;
	glm::vec3 initialLookat;
};
