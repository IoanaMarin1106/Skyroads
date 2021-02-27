#pragma once

#include <Component\SimpleScene.h>
#include <Core/Engine.h>
#include <iostream>

#include "Methods.h"

class Player {
private:
	Mesh* mesh;
	std::string name;
	glm::vec4 centerCoord;
	int fuel;
	int lifes;
	float initialZ;
	float radius;
	bool isLosingLife;
	bool isLosingFuel;
	bool isReceivingFuel;
public:
	Player();
	~Player();
	Player(std::string name, glm::vec4 centerCoord, int fuel, int lifes);

	bool getIsReceivingFuel();

	void setIsReceivingFuel(bool val);

	bool getIsLosingFuel();

	void setIsLosingFuel(bool val);

	bool getIsLosingLife();
	void setIsLosingLife(bool val);

	float getRadius();
	void setRadius(float radius);

	Mesh* getMesh();

	int getFuel();
	void setFuel(int fuel);

	std::string getName();

	int getLifes();

	void setLifes(int lifes);

	glm::vec4 getCenterCoord();
	void setCenterCoord(glm::vec4 centerCoord);

	void setZPosition(float velZ);
	void incYPosition(float incY);
	void setYPosition(float velY);
	void setXPosition(float velX);

	float getInitialZ();
	void setInitialZ(float newZ);
};
