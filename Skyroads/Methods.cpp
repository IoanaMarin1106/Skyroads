#include "Methods.h"

#include <Core/Engine.h>
#include <iostream>

/*
*	Functie ce va crea mesh-ul pentru player.
*/
Mesh* Methods::playerInitMesh(std::string name) {

	Mesh* playerMesh = new Mesh(name);
	playerMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
	return playerMesh;
}

/*
*	Functie ce va crea mesh-ul pentru o platforma.
*/
Mesh* Methods::createPlatformMesh(std::string name) {

	Mesh* platformMesh = new Mesh(name);
	platformMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
	return platformMesh;
}
