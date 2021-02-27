#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

namespace Methods
{
	// Metoda ce creeaza mesh-ul player-ului.	
	Mesh* playerInitMesh(std::string name);

	// Metoda ce creeaza mesh-ul unei platforme.
	Mesh* createPlatformMesh(std::string name);
}

