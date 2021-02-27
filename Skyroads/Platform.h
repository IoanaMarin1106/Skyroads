#pragma once

#include <Component\SimpleScene.h>
#include <Core/Engine.h>
#include <iostream>

#include "Methods.h"

class Platform {
private:
	Mesh* mesh;
	std::string name;
	glm::vec4 centerCoord;
	glm::vec3 color;
	
public:
	Platform();
	~Platform();

	Platform(std::string name, glm::vec4 center, glm::vec3 color);

	Mesh* getMesh();

	void setColor(glm::vec3 color);
	glm::vec3 getColor();

	std::string getName();

	glm::vec4 getCenterCoord();
	void setCenterCoord(glm::vec4 centerCoord);
};
