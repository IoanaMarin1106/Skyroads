#include <iostream>
#include <Laboratoare\Tema2\Player.h>
#include <Component/SimpleScene.h>

#include "Player.h"

using namespace std;

Player::Player() {
}

Player::~Player() {
}

Player::Player(std::string name, glm::vec4 centerCoord, int fuel, int lifes) {
	this->name = name;
	this->centerCoord = centerCoord;
	this->centerCoord.y = 0.28f;
	this->fuel = fuel;
	this->lifes = lifes;
	this->mesh = Methods::playerInitMesh(name);
	this->initialZ = 0;
	this->radius = 0.5f;
	this->isLosingLife = false;
	this->isLosingFuel = false;
	this->isReceivingFuel = false;
}

bool Player::getIsReceivingFuel() {
	return this->isReceivingFuel;
}

void Player::setIsReceivingFuel(bool val) {
	this->isReceivingFuel = val;
}


bool Player::getIsLosingFuel() {
	return this->isLosingFuel;
}

void Player::setIsLosingFuel(bool val) {
	this->isLosingFuel = val;
}


bool Player::getIsLosingLife() {
	return this->isLosingLife;
}

void Player::setIsLosingLife(bool val) {
	this->isLosingLife = val;
}

float Player::getRadius() {
	return this->radius;
}

void Player::setRadius(float radius) {
	this->radius = radius;
}

Mesh* Player::getMesh()
{
	return this->mesh;
}

int Player::getFuel()
{
	return this->fuel;
}

void Player::setFuel(int fuel)
{
	this->fuel = fuel;
}

std::string Player::getName()
{
	return this->name;
}

int Player::getLifes()
{
	return this->lifes;
}

void Player::setLifes(int dec) {
	this->lifes -= dec;
}

glm::vec4 Player::getCenterCoord()
{
	return this->centerCoord;
}

void Player::setCenterCoord(glm::vec4 centerCoord)
{
	this->centerCoord = centerCoord;
}

void Player::setZPosition(float velZ) {
	this->centerCoord.z += velZ;
}

void Player::incYPosition(float incY) {
	this->centerCoord.y += incY;
}

void Player::setYPosition(float velY)
{
	this->centerCoord.y = velY;
}

void Player::setXPosition(float velX)
{
	this->centerCoord.x += velX;
}

float Player::getInitialZ()
{
	return this->initialZ;
}

void Player::setInitialZ(float newZ)
{
	this->initialZ = newZ;
}

