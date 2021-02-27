#include "Platform.h"

#include <iostream>
#include <Component/SimpleScene.h>

using namespace std;

Platform::Platform() {
}

Platform::~Platform() {
}

Platform::Platform(std::string name, glm::vec4 center, glm::vec3 color)
{
	this->name = name;
	this->centerCoord = center;
	this->mesh = Methods::createPlatformMesh(name);
	this->color = color;
}

Mesh* Platform::getMesh()
{
	return this->mesh;
}

void Platform::setColor(glm::vec3 color)
{
	this->color = color;
}

glm::vec3 Platform::getColor()
{
	return this->color;
}

std::string Platform::getName()
{
	return this->name;
}

glm::vec4 Platform::getCenterCoord()
{
	return this->centerCoord;
}

void Platform::setCenterCoord(glm::vec4 centerCoord)
{
	this->centerCoord.x += centerCoord.x;
	this->centerCoord.y += centerCoord.y;
	this->centerCoord.z += centerCoord.z;
}
