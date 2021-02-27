#include "Skyroads.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>
#include <math.h>

using namespace std;

#define GRAVITY 20
#define BLUE glm::vec3(0, 0, 1)
#define GREEN glm::vec3(0, 1, 0)
#define RED glm::vec3(1, 0, 0)
#define YELLOW glm::vec3(1, 1, 0)
#define ORANGE glm::vec3(1, 0.6f, 0.2f)

/*
*
*	Se apasa tasta ENTER pentru a se incepe jocul.
* 
*	Culorile platformelor vor fi definite ca a 4a coomponenta a unui vec4.
*	Aceasta va fi un numar decimal, ce va fi calculat ca reprezentarea in decimal
*	a codificarii rgb a culorii in binar. Astfel culorile vor fi codificate astfel:
*		-> platforme albastre: (0, 0, 1) => 1
*		-> platforme verzi:  (0, 1, 0) => 2
*		-> platforme galbene: (1, 1, 0) => 3 (aleator ales)
*		-> platforme rosii: (1, 0, 0) => 4
*		-> platforme portocalii: (1, 0.6f, 0.2f) => 5 (ales aleator)
* 
*	Scorul jucatorului este afisat in partea stanga sus a ecranului.
* 
*	Reguli scor:
*		De fiecare data cand jucatorul aterizeaza pe o platforma verde				  -> +10 puncte
*		De fiecare data cand jucatorul aterizeaza pe o platforma galbena			  -> -5 puncte
*	
*	
*/


Skyroads::Skyroads()
{
}

Skyroads::~Skyroads()
{
}

/* 
*	Functie ce va afisa regulile jocului in consola la inceputul acestuia. 
*/
void Skyroads::InitGameInterface() {
	cout << endl << endl << "========================== SKY ROADS==============================" << endl;
	cout << " --> Se apasa tasta ENTER pentru a se incepe jocul. <--" << endl;

	cout << endl << " --> Jucatorul va fi controlat prin tastele WASD si SPACE.";
	cout << " Folosind A si D, va schimba coloanele de mers, tasta SPACE va fi " <<
		"folosita pentru a sari peste spatii, iar W si S pentru a alege viteza de mers." << endl << endl;

	cout << " --> Folosind tasta C, se va oscila intre modurile de joc first si third person camera." << endl << endl;
	cout << "===================================================================" << endl;
}

void Skyroads::Init()
{
	renderCameraTarget = false;

	camera = new Laborator::Camera();
	camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("screen_quad");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "screen_quad.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	//======================= Creare PlayerMesh =============================================
	{
		player = new Player("player", playerCenter, playerFuel, playerLifes);
		meshes[player->getMesh()->GetMeshID()] = player->getMesh();

		// Pozitia initiala a camerei in functie de pozitia player-ului.
		camera->position.z = player->getCenterCoord().z + 3;
	}

	//====================== Creare PlatformMesh==============================================
	{
		platform = new Platform("platform", platformCenter, glm::vec3(1, 0, 0));
		meshes[platform->getMesh()->GetMeshID()] = platform->getMesh();
	}

	//========================== Desenare Configuratie Initiala ===============================
	{
		DrawInitialMap();
	}
	
	//========================= Creare Shader pentru colorare =====================================================
	{
		Shader* shader = new Shader("MyShader");
		shader->AddShader("Source/Laboratoare/Skyroads/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Skyroads/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//========================= Creare Noise Shader =====================================================
	{
		Shader* shader = new Shader("NoiseShader");
		shader->AddShader("Source/Laboratoare/Skyroads/NoiseShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Skyroads/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//========================= Creare Shader pentru combustibil ==========================================
	{
		Shader* shader = new Shader("ShaderFuel");
		shader->AddShader("Source/Laboratoare/Skyroads/FuelShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Skyroads/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// Afisam regulile jocului la inceput.
	InitGameInterface();

	// Populam vectorul cu cele 7 segmente.
	GetSevenSegments();

	// Initializam scorul la inceputul jocului.
	score = 0;

	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
}

void Skyroads::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

/*
*	Functie ce va returna codificarea culorii unei platforme generate aleator.
*	@color: culoarea primita ca parametru sub forma unui numar intre 1 si 100 iar in functie 
*			de intervalul in care se afla se va returna culoarea corespunzatoare.
*/
float Skyroads::GetColor(float color) {

	float colorGen = 0;

	// Platforme albastre: 70% sanse in a se genera
	if (color >= 1 && color <= 70) {
		colorGen = 1;
	}

	// Platforme rosii: 3% sanse in a se genera
	if (color > 70 && color <= 73) {
		colorGen = 4;
	}

	// Platforme portocalii: 7% sanse in a se genera
	if (color > 73 && color <= 80) {
		colorGen = 5;
	}

	// Platforme verzi: 11% sanse in a se genera
	if (color > 80 && color <= 91) {
		colorGen = 2;
	}

	// Platforme galbene: 9% sanse in a se genera
	if (color > 91 && color <= 100) {
		colorGen = 3;
	}

	return colorGen;
}

/*
*	Functie ce va afisa pe ecran vietile sub forma unor dreptunghiuri verzi scalate mai putin pe Ox
*	si mai mult pe Oy. In cazul in care numarul de vieti este zero atunci se va termina jocul.
*/
void Skyroads::RenderLifes() {

	// Jocul se va termina. Jucatorul nu mai are nicio viata.
	if (player->getLifes() <= 0) {
		isGameOver = true;
	}

	for (float i = 0; i < player->getLifes(); i++) {

		// Compunem distanta dintre vieti in momentul randarii
		float dist = (i / 100 + 0.08f) * 2;

		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(0.7f + dist, 0.9f, -0.9f);
		modelMatrix *= Transform3D::Scale(0.01f, 0.1f, 0.5f);
		RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 0));
	}
}

/*
*	Functie ce va detecta coliziunile dintre player si platforme.
*/
bool Skyroads::CollisionDetection(glm::vec4 platformCoord) {

	// Coltul din stanga jos al platformei
	float platformMinX = platformCoord.x - 0.55f;
	float platformMinY = platformCoord.y;
	float platformMinZ = platformCoord.z + 0.55f;

	// Coltul din dreapta sus al platformei
	float platformMaxX = platformMinX + 1;
	float platformMaxY = platformMinY;
	float platformMaxZ = platformMinZ - 1;

	float x = max(platformMinX, min(player->getCenterCoord().x, platformMaxX));
	float y = max(platformMinY, min(player->getCenterCoord().y, platformMaxY));
	float z = max(platformMinZ, min(player->getCenterCoord().z, platformMaxZ));

	float distance = sqrt((x - player->getCenterCoord().x) * (x - player->getCenterCoord().x) +
						  (y - player->getCenterCoord().y) * (y - player->getCenterCoord().y) +
						  (z - player->getCenterCoord().z) * (z - player->getCenterCoord().z));

	return (distance < player->getRadius() + 0.1f);

}

/*
*	Functie ce va afisa o platforma in scena la o anumita valoare pe axa Oz si o anumita
*	valoare pe axa Ox, date ca parametru.
*/
void Skyroads::RenderPlatform(float zPosition, float xPosition, glm::vec3 color) {
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(xPosition, 0, -zPosition);
	modelMatrix *= Transform3D::Scale(xScale, yScale, zScale);
	RenderSimpleMesh(meshes[platform->getName()], shaders["MyShader"], modelMatrix, color);
}

/*
*	Functie ce va afisa o configuratie de platforme initiala.
*/
void Skyroads::DrawInitialMap() {
	glm::vec3 color = glm::vec3(0, 0, 1);

	for (float j = 1.2f; j > -2; j -= 1.2f) {
		for (float i = -1; i < 20; i++) {

			// Primesc pe a 4a coordonata valoarea 1, codificarea culorii albastru.
			platformsMap.push_back(glm::vec4(j, 0, i, 1));
		}
	}
}

/*
*	Functie ce va genera la un moment dat de timp pe pozitia de pe axa Oz data ca parametru,
*	o singura platforma pe un rand.
*/
void Skyroads::GenerateOnePlatform(float z) {

	// Daca nu am mai generat pana acum nicio platforma, daca am generat 2 sau 3
	// nu ne intereseaza pe ce coordonate va fi pusa noua platforma generata deoarece
	// sigur se va putea continua jocul.
	if (platformLine.size() == 0 || platformLine.size() == 2 || platformLine.size() == 3) {
		float randX = rand() % 3 + 1;

		// Nu ne intereseaza ce linie de platforme s-a generat anterior, deci golim vectorul.
		if (platformLine.size() == 2 || platformLine.size() == 3) {
			platformLine.clear();
		}

		// Coloana din stanga
		if (randX == 1) {
			platformLine.push_back(glm::vec4(-1.2f, 0, z, 1));
		}

		// Mijloc
		if (randX == 2) {
			platformLine.push_back(glm::vec4(0, 0, z, 1));
		}

		// Coloana din dreapta
		if (randX == 3) {
			platformLine.push_back(glm::vec4(1.2f, 0, z, 1));
		}
	}
	else if (platformLine.size() == 1) {
		float xPosition = platformLine.at(0).x;
		platformLine.clear();

		// Daca platforma anterior generata era pe coloana din stanga
		if (xPosition == -1.2f) {
			float randX = rand() % 2 + 1;

			// Coloana din stanga
			if (randX == 1) {
				platformLine.push_back(glm::vec4(-1.2f, 0, z, 1));
			}

			// Mijloc
			if (randX == 2) {
				platformLine.push_back(glm::vec4(0, 0, z, 1));
			}	
		}

		// Daca platforma anterior generata era pe coloana din mijloc
		if (xPosition == 0) {
			float randX = rand() % 3 + 1;

			// Coloana din stanga
			if (randX == 1) {
				platformLine.push_back(glm::vec4(-1.2f, 0, z, 1));
			}

			// Mijloc
			if (randX == 2) {
				platformLine.push_back(glm::vec4(0, 0, z, 1));
			}

			// Coloana din dreapta
			if (randX == 3) {
				platformLine.push_back(glm::vec4(1.2f, 0, z, 1));
			}
		}

		// Daca platforma anterior generata era pe coloana din dreapta
		if (xPosition == 1.2f) {
			float randX = rand() % 2 + 1;

			// Mijloc
			if (randX == 1) {
				platformLine.push_back(glm::vec4(0, 0, z, 1));
			}

			// Coloana din dreapta
			if (randX == 2) {
				platformLine.push_back(glm::vec4(1.2f, 0, z, 1));
			}
		}
	}

	// Adaugam platforma nou generata in vectorul de coordonate
	platformsMap.insert(platformsMap.end(), platformLine.begin(), platformLine.end());
}

/*
*	Functie ce va genera la un moment dat de timp pe o pozitie de pe axa Oz data ca parametru,
*	doua platforme pe un singur rand.
*/
void Skyroads::GenerateTwoPlatforms(float z) {

	// Generam random culoarea fiecarei platforme.
	float randColor = rand() % 100 + 1;
	float color1 = GetColor(randColor);

	randColor = rand() % 100 + 1;
	float color2 = GetColor(randColor);

	// Daca ambele culori ale platformelor sunt rosii, atunci generam alta culoare.
	// Orice alta culoare din cele 4 disponibile.
	if (color1 == color2 && color1 == 4) {
		color2 = GetColor(rand() % 100 + 1);

		if (color2 == 4) {
			color2 = 1;
		}
	}

	// Generam random distanta dintre platforme (0 spatii libere/ 1 spatiu liber).
	float randDist = rand() % 1 + 0;

	if (randDist == 0) {

		// Alegem coordonata pe X de unde vom incepe generarea. (2 = x la mijloc, 1 = x in stanga).
		float randX = rand() % 2 + 1;
			
		// Daca generam doar pe coloana din mijloc si pe cea din dreapta
		if (randX == 2) {
			platformLine.clear();
			platformLine.push_back(glm::vec4(0, 0, z, color1));
			platformLine.push_back(glm::vec4(1.2f, 0, z, color2));
		}
		else if (randX == 1) {
			platformLine.clear();
			platformLine.push_back(glm::vec4(-1.2f, 0, z, color1));
			platformLine.push_back(glm::vec4(0, 0, z, color2));
		}	
	}
	else if (randDist == 1) {
		platformLine.clear();
		platformLine.push_back(glm::vec4(-1.2f, 0, z, color1));
		platformLine.push_back(glm::vec4(1.2f, 0, z, color2));
	}

	// Adaugam in vectorul de coordonate noile platforme generate
	platformsMap.insert(platformsMap.end(), platformLine.begin(), platformLine.end());
}

/*
*	Functie ce va genera la un moment dat de timp pe o pozitie de pe axa Oz data ca parametru,
*	trei platforme pe un singur rand.
*/
void Skyroads::GenerateThreePlatforms(float z) {

	// Generam random culoarea fiecarei platforme.
	float randColor = rand() % 100 + 1;
	float color1 = GetColor(randColor);

	randColor = rand() % 100 + 1;
	float color2 = GetColor(randColor);

	randColor = rand() % 100 + 1;
	float color3 = GetColor(randColor);

	// Daca toate trei platformele sunt rosii atunci schimbam culoarea unei platforme
	// sau schimbam culorile pentru 2 platforme.
	if (color1 == color2 && color1 == color3 && color1 == 4) {
		float colorsChange = rand() % 2 + 1;

		// Schimbam culoarea pentru o platforma
		if (colorsChange == 1) {
			float index = rand() % 3 + 1;

			// Schimbam culoarea platformei 1
			if (index == 1) {
				color1 = GetColor(rand() % 100 + 1);
				
				if (color1 == 4) {
					color1 = 1;
				}
			}

			// Schimbam culoarea platformei 2
			if (index == 2) {
				color2 = GetColor(rand() % 100 + 1);

				if (color2 == 4) {
					color2 = 1;
				}
			}

			// Schimbam culoarea platformei 3
			if (index == 3) {
				color3 = GetColor(rand() % 100 + 1);

				if (color3 == 4) {
					color3 = 1;
				}
			}
		}
		else if (colorsChange == 2) {
			float index = rand() % 3 + 1;

			// Schimbam culoarea platformelor 1 si 2
			if (index == 1) {
				color1 = GetColor(rand() % 100 + 1);
				color2 = GetColor(rand() % 100 + 1);

				if (color1 == 4) {
					color1 = 1;
				}

				if (color2 == 4) {
					color2 = 1;
				}
			}

			// Schimbam culoarea platformelor 2 si 3
			if (index == 2) {
				color2 = GetColor(rand() % 100 + 1);
				color3 = GetColor(rand() % 100 + 1);

				if (color2 == 4) {
					color2 = 1;
				}

				if (color3 == 4) {
					color3 = 1;
				}
			}

			// Schimbam culoarea platformelor 1 si 3
			if (index == 3) {
				color3 = GetColor(rand() % 100 + 1);
				color1 = GetColor(rand() % 100 + 1);

				if (color3 == 4) {
					color3 = 1;
				}

				if (color1 == 4) {
					color3 = 1;
				}
			}
		}
	}

	// Nu ne mai intereseaza ce s-a generat anterior.
	// Stergem toate elementele din vectorul liniei generate.
	platformLine.clear();

	// Actualizam linia generata
	platformLine.push_back(glm::vec4(-1.2f, 0, z, color1));
	platformLine.push_back(glm::vec4(0, 0, z, color2));
	platformLine.push_back(glm::vec4(1.2f, 0, z, color3));

	// Adaugam noile platforme in vectorul de coordonate total
	platformsMap.insert(platformsMap.end(), platformLine.begin(), platformLine.end());
}

/*
*	Functie ce va genera o noua linie formata din 3 platforme.
*/
void Skyroads::GeneratePlatformLine() {

	// Generam cate platforme vom pune pe linia noua 
	float lineSize = rand() % 3 + 1;

	// Coordonata pe axa Oz la care se va genera noua linie de platforme
	float zPlatform = player->getInitialZ() + 19;


	// Daca vom genera o linie cu o platforma.
	if (lineSize == 1) {
		GenerateOnePlatform(zPlatform);
	}

	// Daca vom genera o linie cu 2 platforme.
	if (lineSize == 2) {
		GenerateTwoPlatforms(zPlatform);
	}

	// Daca vom genera o linie cu 3 platforme
	if (lineSize == 3) {
		GenerateThreePlatforms(zPlatform);
	}
}

/*
*	Functie ce va desena un rand de platforme in momentul in care player-ul
*	va parcurge un rand de platforme din configuratia din scena. Astfel asiguram
*	faptul ca se vor genera la infinit platformele din scena.
*/
void Skyroads::DrawPlatforms() {
	
	// Verificam daca player-ul a trecut de un rand de platforme.
	if (player->getCenterCoord().z - player->getInitialZ() > 1) {
		player->setInitialZ(player->getInitialZ() + 1);
		GeneratePlatformLine();
	}
}

/*
*	Functie ce returneaza sub forma unui vector rgb culoarea, in functie de 
*	codificarea facuta.
*/
glm::vec3 Skyroads::ColorHandler(float color) {

	if (color == 1) {
		return BLUE;
	}

	if (color == 2) {
		return GREEN;
	}

	if (color == 3) {
		return YELLOW;
	}

	if (color == 4) {
		return RED;
	}

	return ORANGE;
}

/*
*	Functie ce va afisa in scena configuratia de platforme so va gestiona si momentul in care
*	player-ul se va lovi de platforme iar culoarea platformei va fi schimbata in mov.
*/
void Skyroads::RenderMap() {
	DrawPlatforms();

	glm::vec3 color = glm::vec3(0, 0, 1);
	glm::vec3 resetColor = glm::vec3(0, 0, 1);

	for (int i = 0; i < platformsMap.size(); i++) {
		bool isCollision = false;

		if (isStartGame) {
			isCollision = CollisionDetection(platformsMap.at(i));
		}

		// Daca este coliziune atunci platforma isi va schimba culoarea in mov.
		if (isCollision == true) {
			color = glm::vec3(0.5f, 0, 1);
		}
		else {
			color = ColorHandler(platformsMap.at(i).w);
		}
		RenderPlatform(platformsMap.at(i).z, platformsMap.at(i).x, color);
	}
}

/*
*	Functie ce va trata cazurile in care player-ul se deplaseaza cu o viteza mai mare
*	sau mai mica sau va sari.
*/
void Skyroads::PlayerActionsHandler(float deltaTimeSeconds) {
	if (isStartGame) {
		
		camera->MoveForward(deltaTimeSeconds * playerSpeed);

		// Crestem viteza cu care se deplaseaza
		if (isWPressed == true) {
			isWPressed = false;
			player->setZPosition(deltaTimeSeconds * playerSpeed * 2.5f);
		} 
		else 
		// Micsoram viteza de deplasare
		if (isSPressed == true) {
			isSPressed = false;
			player->setZPosition(deltaTimeSeconds * playerSpeed * 2.5f);
		} 
		else 
		// Player-ul se deplaseaza normal pe platforme.
		if (isWPressed == false && isSPressed == false) {
			player->setZPosition(deltaTimeSeconds * 3.5f);
		
		}

		// Player-ul va sari
		if (isSpacePressed == true) {

			if (isInAir) {
				player->incYPosition(jumpPlayer * deltaTimeSeconds - (GRAVITY * deltaTimeSeconds * deltaTimeSeconds / 2));
				jumpPlayer -= deltaTimeSeconds * GRAVITY;
			}

			// Verificam cand a terminat de facut saritura sau daca a cazut si va trebui sa iasa din joc.
			for (int i = 0; i < platformsMap.size(); i++) {
				if (CollisionDetection(platformsMap.at(i)) == true && player->getCenterCoord().y <= 0.28f) {
					isInAir = false;
				}
			}

			// Daca nu mai este in aer verifican daca a cazut de pe platforma sau este pe platforma.
			if (isInAir == false) {

				// Verificam daca este cazul in care acesta iese din joc.
				if (player->getCenterCoord().y < 0.28f) {
					player->setYPosition(0.28f);
				}
				isSpacePressed = false;
				jumpPlayer = 7;
				player->setYPosition(0.28f);
			}
		}
	}
}

/*
*	Functie ce va trata cazurile in care jucatorul schimba coloana de platforme.
*/
void Skyroads::ChangingPlatformsHandler(float deltaTimeSeconds) {

	// Se va schimba platforma cu cea din stanga.
	if (isChangingPlatformToLeft == true) {
		isChangingPlatformToLeft = false;

		player->setXPosition(-deltaTimeSeconds * 3);
		player->setZPosition(deltaTimeSeconds * 0.5f);
	}

	// Se va schimba platforma cu cea din dreapta.
	if (isChangingPlatformToRight == true) {
		isChangingPlatformToRight = false;

		player->setXPosition(deltaTimeSeconds * 3);
		player->setZPosition(deltaTimeSeconds * 0.5f);
	}
}

/*
*	Functie ce va face tranzitia modul de vizualizare a jocului: din Third Person (se vede si jucatorul) in 
*	First Person (se vede tot jocul din perspectiva jucatorului) si invers.
*/
bool Skyroads::SetCameraMode() {

	if (isCpressed) {
		camera->Set(glm::vec3(player->getCenterCoord().x, player->getCenterCoord().y, -player->getCenterCoord().z),
			glm::vec3(player->getCenterCoord().x, player->getCenterCoord().y, -player->getCenterCoord().z - 1),
			OyAxis);
		return true;
	}
	else {
		camera->Set(glm::vec3(0, 2, -player->getCenterCoord().z + 5),
			glm::vec3(0, 1, -player->getCenterCoord().z),
			OyAxis);
		return false;
	}
}

/*
*	Functie ce va verifica daca jucatorul pierde o viata.
*/
void Skyroads::GetOutPlayer(float deltaTimeSeconds, glm::vec4 lastPosition) {

	player->incYPosition(-deltaTimeSeconds * 5);

	if (player->getCenterCoord().y >= -4) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix *= Transform3D::Translate(player->getCenterCoord().x, player->getCenterCoord().y, -player->getCenterCoord().z);
		modelMatrix *= Transform3D::Scale(scalingFactor, scalingFactor, scalingFactor);
		RenderSimpleMesh(meshes[player->getName()], shaders["MyShader"], modelMatrix, glm::vec3(0, 1, 1));
	}

	// Jocul s-a terminat, deci ii mai scadem o viata.
	if (player->getCenterCoord().y < -4) {
		player->setLifes(1);
		player->setCenterCoord(initialCoord);
		fuelScale = 0;
		score = 0;
		player->setInitialZ(0);
		
		RenderPlayer(deltaTimeSeconds);
	}
	
}

/*
*	Functie ce va determina cu ce tip special de platforma este facuta coliziunea player-ului.
*/
void Skyroads::DetectCollisionsWithSpecialEffects(float color, glm::vec4 position) {
	glm::vec3 platformColor = ColorHandler(color);

	if (platformColor == RED) {
		isCollisionRedPlatform = true;
	}

	if (platformColor == GREEN) {
		isCollisionGreenPlatform = true;
	}
	
	if (platformColor == YELLOW) {
		isCollisionYellowPlatform = true;
	}

	if (platformColor == ORANGE) {
		isCollisionOrangePlatform = true;
	}
}

/*
*	Functie ce va gestiona coliziunea cu fiecare tip de platforma speciala.
*/
bool Skyroads::SpecialCollisionsHandler(float deltaTimeSeconds) {

	// Daca este o coliziune cu o platforma rosie, atunci jucatorul va pierde imediat.
	if (isCollisionRedPlatform) {
		isCollisionRedPlatform = false;
		isGameOver = true;

		/*if (!player->getIsLosingLife()) {
			player->setLifes(1);
			player->setIsLosingLife(true);
			score = 0;
		}

		player->setCenterCoord(initialCoord);
		fuelScale = 0;
		player->setInitialZ(0);*/

		return true;
	}

	// Daca este o coliziune cu o platforma verde, atunci creste cantitatea de combustibil
	if (isCollisionGreenPlatform) {
		isCollisionGreenPlatform = false;
		
		if (!player->getIsLosingFuel()) {
			fuelScale -= 0.003f;
			player->setIsLosingFuel(true);
		}

		if (!isGettingScore) {
			isGettingScore = true;
			score += 30;
		}
	}

	// Daca este o coliziune cu o platforma galbena, atunci scade cantitatea de combustibil
	if (isCollisionYellowPlatform) {
		isCollisionYellowPlatform = false;

		if (!player->getIsReceivingFuel()) {
			fuelScale += 0.001f;
			player->setIsReceivingFuel(true);
		}

		if (!isLosingPoints) {
			isLosingPoints = true;
			score -= 5;
		}
	}

	// Daca este o coliziune cu o platforma portocalie, jucatorul este blocat un anumit numar de secunde 
	// la o viteza foarte mare (tastele W si S nu au niciun efect in aceasta perioada)
	if (isCollisionOrangePlatform) {

		seconds -= 0.1f;
		if (seconds >= 0) {
			playerSpeed += deltaTimeSeconds * 10;
			player->setZPosition(playerSpeed * deltaTimeSeconds);
		}
		else {
			isCollisionOrangePlatform = false;
		}
	}

	return false;
}

/*
*	Functie ce va reseta variabilele de tip boolean folosite pentru coliziunile cu platformele
*	speciale.
*/
void Skyroads::ResetPlayerAttributes() {
	if (!isCollisionRedPlatform) {
		player->setIsLosingLife(false);
	}

	if (!isCollisionYellowPlatform) {
		player->setIsLosingFuel(false);
		isLosingPoints = false;
		
	}

	if (!isCollisionGreenPlatform) {
		isGettingScore = false;
		player->setIsReceivingFuel(false);
	}
}

/*
*	Functie ce va afisa in scena player-ul si va gestiona toate actiunile sale in joc.
*/
void Skyroads::RenderPlayer(float deltaTimeSeconds) {
	
	glm::mat4 modelMatrix = glm::mat4(1);

	// Miscarile jucatorului
	PlayerActionsHandler(deltaTimeSeconds);

	// Functia ce va afisa scorul in scena.
	RenderScore(score);

	// Verificam daca jucatorul vrea sa schimbe platforma pe care se afla.
	ChangingPlatformsHandler(deltaTimeSeconds);

	// Vedem daca este cazul sa generam o noua platforma.
	DrawPlatforms();

	bool isDead = true;

	// Ultima pozitie a player-ului inainte sa moara
	glm::vec4 lastPosition;

	// Retinem culoarea platformei cu care se efecteaza coliziunii
	float color = 0;

	if (isStartGame) {
		for (int i = 0; i < platformsMap.size(); i++) {

			bool isCollision = CollisionDetection(platformsMap.at(i));

			if (isCollision) {
				isDead = false;

				color = platformsMap.at(i).w;
	
				lastPosition.x = player->getCenterCoord().x;
				lastPosition.y = player->getCenterCoord().y;
				lastPosition.z = player->getCenterCoord().z;

				DetectCollisionsWithSpecialEffects(color, lastPosition);
			}
		}
	}

	// Daca nu sunt coliziuni cu platformele atunci resetam atributele player-ului
	// corespunzatoare.
	ResetPlayerAttributes();

	// Verificam cu care dintre platforme a fost facuta coliziunea si daca 
	// coliziunea a fost facuta cu o platforma rosie functia va returna adevarat.
	bool redCollision = SpecialCollisionsHandler(deltaTimeSeconds);

	if (isCollisionOrangePlatform) {
		isCollisionOrangePlatform = false;
	}

	// Verificam daca player-ul pierde jocul sau nu si gestionam si daca player-ul aterizeaza pe o platforma speciala 
	// pentru a-l afisa deformat folosing shader-ul corespunzator.
	if (!redCollision) {
		if ((!player->getIsLosingLife())  && 
			((isDead == true && isStartGame && player->getCenterCoord().y <= 0.28f) ||(isDead == false && player->getCenterCoord().y < 0))) {
			GetOutPlayer(deltaTimeSeconds, lastPosition);
		}
		else {
			if (SetCameraMode() == false) {

				modelMatrix *= Transform3D::Translate(player->getCenterCoord().x, player->getCenterCoord().y, -player->getCenterCoord().z);
				modelMatrix *= Transform3D::Scale(scalingFactor, scalingFactor, scalingFactor);

				if (player->getCenterCoord().y == 0.28f && isStartGame == true && ColorHandler(color) != BLUE) {
					RenderSimpleMesh(meshes[player->getName()], shaders["NoiseShader"], modelMatrix, glm::vec3(0, 1, 1));
				}
				else {
					RenderSimpleMesh(meshes[player->getName()], shaders["MyShader"], modelMatrix, glm::vec3(0, 1, 1));
				}
			}
		}
	}
}

/*
*	Functie ce va afisa in scena combustibilul.
*/
void Skyroads::RenderFuel(float deltaTimeSeconds) {

	if (isStartGame) {
		fuelScale += deltaTimeSeconds * 0.03f;
	}

	// Stabilim nivelul de combustibil curent.
	float fuelLevel = 0.9f - fuelScale;

	glm::vec4 lastPosition;
	lastPosition.x = player->getCenterCoord().x;
	lastPosition.y = player->getCenterCoord().y;
	lastPosition.z = player->getCenterCoord().z;

	// Player-ul nu mai are combustibil.
	if (fuelLevel <= -0.0093f) {

		// Scadem numarul vietilor cu 1 si resetam combustibilul.
		player->setLifes(1);
		fuelScale = 0;
		player->setCenterCoord(initialCoord);
		player->setInitialZ(0);
		score = 0;

		RenderPlayer(deltaTimeSeconds);	
	}
	
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(0.9f, 0.05f, -0.5);
	modelMatrix *= Transform3D::Translate(0, -0.5f, 0);
	modelMatrix *= Transform3D::Scale(0.05f, 0.9f - fuelScale, 0.5f);
	modelMatrix *= Transform3D::Translate(0, 0.5f, 0);
	RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 0));

	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(0.9f, 0, -0.5);
	modelMatrix *= Transform3D::Scale(0.08f, 0.93f, 0.5f);
	RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(1, 1, 1));
}

/*
*	Functie ce va configura vectorul ce va contine coordonatele dreptunghiurilor pentru o 
*	cifra pentru scor si in plus de asta va retine o codificare pe a 4a coordonata pentru 
*	a sti daca dreptunghiul este orizontal sau vertical.
*/
void Skyroads::GetSevenSegments() {
	sevenSegmentsCoord.push_back(glm::vec4(-0.7f, 0.95f, -0.5f, 1)); //1
	sevenSegmentsCoord.push_back(glm::vec4(-0.68f, 0.91f, -0.5f, 0)); //2
	sevenSegmentsCoord.push_back(glm::vec4(-0.68f, 0.85f, -0.5f, 0)); //3
	sevenSegmentsCoord.push_back(glm::vec4(-0.7f, 0.82f, -0.5f, 1));//4
	sevenSegmentsCoord.push_back(glm::vec4(-0.72f, 0.85f, -0.5f, 0));//5
	sevenSegmentsCoord.push_back(glm::vec4(-0.72f, 0.91f, -0.5f, 0));//6
	sevenSegmentsCoord.push_back(glm::vec4(-0.7f, 0.88f, -0.5f, 1));//7
}

/*
*	Functie ce va afisa pentru fiecare cifra segmentele corespunzatoare.
*/
void Skyroads::GetDigit(int digit, float offset) {
	glm::mat4 modelMatrix;

	switch (digit) {
		case 0:
			for (int i = 0; i < 6; i++) {
				modelMatrix = glm::mat4(1);
				modelMatrix *= Transform3D::Translate(sevenSegmentsCoord.at(i).x + offset, sevenSegmentsCoord.at(i).y, sevenSegmentsCoord.at(i).z);

				if (sevenSegmentsCoord.at(i).w == 1) {
					modelMatrix *= Transform3D::Scale(0.03f, 0.01f, 0.3f);
				}
				else if (sevenSegmentsCoord.at(i).w == 0) {
					modelMatrix *= Transform3D::Scale(0.005f, 0.05f, 0.3f);
				}

				RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 0));
			}
			break;
		case 1:
			for (int i = 1; i < 3; i++) {
				modelMatrix = glm::mat4(1);
				modelMatrix *= Transform3D::Translate(sevenSegmentsCoord.at(i).x + offset, sevenSegmentsCoord.at(i).y, sevenSegmentsCoord.at(i).z);
				modelMatrix *= Transform3D::Scale(0.005f, 0.05f, 0.3f);
				RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 0));
			}
			break;
		case 2:
			for (int i = 0; i < 7; i++) {
				if (i != 2 && i != 5) {
					modelMatrix = glm::mat4(1);
					modelMatrix *= Transform3D::Translate(sevenSegmentsCoord.at(i).x + offset, sevenSegmentsCoord.at(i).y, sevenSegmentsCoord.at(i).z);

					if (sevenSegmentsCoord.at(i).w == 1) {
						modelMatrix *= Transform3D::Scale(0.03f, 0.01f, 0.3f);
					}
					else if (sevenSegmentsCoord.at(i).w == 0) {
						modelMatrix *= Transform3D::Scale(0.005f, 0.05f, 0.3f);
					}

					RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 0));
				}
			}
			break;
		case 3:
			for (int i = 0; i < 7; i++) {
				if (i != 4 && i != 5) {
					modelMatrix = glm::mat4(1);
					modelMatrix *= Transform3D::Translate(sevenSegmentsCoord.at(i).x + offset, sevenSegmentsCoord.at(i).y, sevenSegmentsCoord.at(i).z);

					if (sevenSegmentsCoord.at(i).w == 1) {
						modelMatrix *= Transform3D::Scale(0.03f, 0.01f, 0.3f);
					}
					else if (sevenSegmentsCoord.at(i).w == 0) {
						modelMatrix *= Transform3D::Scale(0.005f, 0.05f, 0.3f);
					}

					RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 0));
				}
			}
			break;
		case 4:
			for (int i = 0; i < 7; i++) {
				if (i == 6 || i == 5 || i == 2 || i == 1) {
					modelMatrix = glm::mat4(1);
					modelMatrix *= Transform3D::Translate(sevenSegmentsCoord.at(i).x + offset, sevenSegmentsCoord.at(i).y, sevenSegmentsCoord.at(i).z);

					if (sevenSegmentsCoord.at(i).w == 1) {
						modelMatrix *= Transform3D::Scale(0.03f, 0.01f, 0.3f);
					}
					else if (sevenSegmentsCoord.at(i).w == 0) {
						modelMatrix *= Transform3D::Scale(0.005f, 0.05f, 0.3f);
					}

					RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 0));
				}
			}
			break;
		case 5:
			for (int i = 0; i < 7; i++) {
				if (i != 4 && i != 1) {
					modelMatrix = glm::mat4(1);
					modelMatrix *= Transform3D::Translate(sevenSegmentsCoord.at(i).x + offset, sevenSegmentsCoord.at(i).y, sevenSegmentsCoord.at(i).z);

					if (sevenSegmentsCoord.at(i).w == 1) {
						modelMatrix *= Transform3D::Scale(0.03f, 0.01f, 0.3f);
					}
					else if (sevenSegmentsCoord.at(i).w == 0) {
						modelMatrix *= Transform3D::Scale(0.005f, 0.05f, 0.3f);
					}

					RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 0));
				}
			}
			break;
		case 6:
			for (int i = 0; i < 7; i++) {
				if (i != 1) {
					modelMatrix = glm::mat4(1);
					modelMatrix *= Transform3D::Translate(sevenSegmentsCoord.at(i).x + offset, sevenSegmentsCoord.at(i).y, sevenSegmentsCoord.at(i).z);

					if (sevenSegmentsCoord.at(i).w == 1) {
						modelMatrix *= Transform3D::Scale(0.03f, 0.01f, 0.3f);
					}
					else if (sevenSegmentsCoord.at(i).w == 0) {
						modelMatrix *= Transform3D::Scale(0.005f, 0.05f, 0.3f);
					}

					RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 0));
				}
			}
			break;
		case 7:
			for (int i = 0; i < 7; i++) {
				if (i == 0 || i == 1 || i == 2) {
					modelMatrix = glm::mat4(1);
					modelMatrix *= Transform3D::Translate(sevenSegmentsCoord.at(i).x + offset, sevenSegmentsCoord.at(i).y, sevenSegmentsCoord.at(i).z);

					if (sevenSegmentsCoord.at(i).w == 1) {
						modelMatrix *= Transform3D::Scale(0.03f, 0.01f, 0.3f);
					}
					else if (sevenSegmentsCoord.at(i).w == 0) {
						modelMatrix *= Transform3D::Scale(0.005f, 0.05f, 0.3f);
					}

					RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 0));
				}
			}
			break;
		case 8:
			for (int i = 0; i < 7; i++) {
				modelMatrix = glm::mat4(1);
				modelMatrix *= Transform3D::Translate(sevenSegmentsCoord.at(i).x + offset, sevenSegmentsCoord.at(i).y, sevenSegmentsCoord.at(i).z);

				if (sevenSegmentsCoord.at(i).w == 1) {
					modelMatrix *= Transform3D::Scale(0.03f, 0.01f, 0.3f);
				}
				else if (sevenSegmentsCoord.at(i).w == 0) {
					modelMatrix *= Transform3D::Scale(0.005f, 0.05f, 0.3f);
				}

				RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 0));
				
			}
			break;
		case 9:
			for (int i = 0; i < 7; i++) {
				if (i != 4) {
					modelMatrix = glm::mat4(1);
					modelMatrix *= Transform3D::Translate(sevenSegmentsCoord.at(i).x + offset, sevenSegmentsCoord.at(i).y, sevenSegmentsCoord.at(i).z);

					if (sevenSegmentsCoord.at(i).w == 1) {
						modelMatrix *= Transform3D::Scale(0.03f, 0.01f, 0.3f);
					}
					else if (sevenSegmentsCoord.at(i).w == 0) {
						modelMatrix *= Transform3D::Scale(0.005f, 0.05f, 0.3f);
					}

					RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 0));
				}
			}
			break;
		default:
			return;
	}
}

/*
*	Functie ce va afisa un "-" (minus) in cazul in care scorul este negativ.
*	(in cazul in care un jucator aterizeaza pe o platforma galbena si are inca scorul 0).
*/
void Skyroads::RenderMinus(float offset) {
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(-0.7f + offset, 0.87f, -0.5f);
	modelMatrix *= Transform3D::Scale(0.02f, 0.01f, 0.3f);
	RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 0));
}

/*
*	Functie ce va afisa scorul in scena.
*/
void Skyroads::RenderScore(int score) {
	int count = 0;

	if (score == 0) {
		GetDigit(0, 0);
	}

	int originalScore = score;

	if (score < 0) {
		score = -score;
	}

	// Afisam in scena fiecare cifra a scorului.
	while (score > 0) {
		int digit = score % 10;
		GetDigit(digit, -0.07f * (float)count);
		count += 1;
		score /= 10;
	}

	// Tratam cazul in care scorul este negativ.
	if (originalScore < 0) {
		RenderMinus(-0.07f * (float)count);
	}
}

void Skyroads::Update(float deltaTimeSeconds)
{
	// Verificam daca jocul s-a terminat sau este in modul de pauza.
	if (!isGameOver && !isPause && !isRestartGame) {
		RenderPlayer(deltaTimeSeconds);
		RenderMap();
		RenderFuel(deltaTimeSeconds);
		RenderLifes();	
	}
	else {
		if (isPause) {
			glClearColor(1, 1, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix *= Transform3D::Translate(0.1f, 0, -0.5);
			modelMatrix *= Transform3D::Scale(0.06f, 0.93f, 0.5f);
			RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 1));

			modelMatrix = glm::mat4(1);
			modelMatrix *= Transform3D::Translate(-0.06f, 0, -0.5);
			modelMatrix *= Transform3D::Scale(0.06f, 0.93f, 0.5f);
			RenderSimpleMesh(meshes["box"], shaders["ShaderFuel"], modelMatrix, glm::vec3(0, 1, 1));
		}
		else if (isGameOver) {
			cout << "GAME OVER" << endl;
			return;
		}	
	}

	// Render the camera target. Useful for understanding where is the rotation point in Third-person camera movement
	if (renderCameraTarget)
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
	}
}

void Skyroads::FrameEnd()
{
	//DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
}

/*
*	Functie de randare in care folosim shader-ul pentru culoare creat in care vom da ca parametru si culoarea
*	cu care dorim sa coloram obiectul.
*/
void Skyroads::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	GLint location = glGetUniformLocation(shader->program, "object_color");
	glUniform3fv(location, 1, glm::value_ptr(color));

	mesh->Render();
}

void Skyroads::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

/*
*	Functie ce va gestiona in OnInputUpdate() miscarile player-ului si va ajusta viteza
*	de mers si schimbarea coloanei de platforme pe care se deplaseaza.
*/
void Skyroads::playerMoveHandler(float deltaTime) {

	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {

		// Tastele W/S au efect doar daca nu exista coliziune cu o platforma portocalie
		if (!isCollisionOrangePlatform) {

			// Creste viteza de mers
			if (window->KeyHold(GLFW_KEY_W)) {
				isWPressed = true;
				playerSpeed += deltaTime * 5;
				fuelScale += deltaTime * 0.05f;

				if (playerSpeed > 10) {
					playerSpeed = 10;
				}
			}

			// Scade viteza de mers
			if (window->KeyHold(GLFW_KEY_S)) {
				isSPressed = true;
				playerSpeed -= (deltaTime * 0.5f);
				fuelScale += deltaTime * 0.01f;

				if (playerSpeed < 0) {
					playerSpeed = 0;
					isStaying = true;
				}
			}
		}

		// Schimba platforma cu cea din stanga.
		if (window->KeyHold(GLFW_KEY_A)) {
			isChangingPlatformToLeft = true;
		}

		// Schimba platforma cu cea din dreapta.
		if (window->KeyHold(GLFW_KEY_D)) {
			isChangingPlatformToRight = true;
		}
	}
}

void Skyroads::OnInputUpdate(float deltaTime, int mods)
{
	// Determinam cum se misca player-ul pe platforme.
	playerMoveHandler(deltaTime);
}

/*
*	Functie ce va gestiona modurile jocului: Start, Pauza, GameOver.
*	Pentru Start se va apasa ENTER, pentru pauza se va apasa P si pentru
*	a inchide jocul fortat se va apasa ESC.
*/
void Skyroads::GameModesHandler(int key) {

	// Daca apasam tasta ENTER, se va incepe jocul.
	if (key == GLFW_KEY_ENTER) {
		isStartGame = true;
	}

	// Daca apasam tasta P, trecem jocul in modul de pauza.
	if (key == GLFW_KEY_P) {
		isPause = !isPause;
	}

	// Daca apasam tasta ESC, se termina jocul fortat.
	if (key == GLFW_KEY_ESCAPE) {
		isGameOver = true;
	}
}

void Skyroads::OnKeyPress(int key, int mods)
{
	// Verificam modul curent al jocului.
	GameModesHandler(key);

	if (isInAir == false) {
		// Vrem ca jucatorul sa sara.
		if (key == GLFW_KEY_SPACE) {
			
			isSpacePressed = true;
			isInAir = true;
		}
	}

	// Variabila ce va stabili daca vedem sfera (ThirdPerson) sau daca nu vedem sfera (FirstPerson).
	if (key == GLFW_KEY_C) {
		isCpressed = !isCpressed;
	}
}

void Skyroads::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (window->GetSpecialKeyState() == 0) {
			renderCameraTarget = false;
			camera->RotateFirstPerson_OX(deltaY * sensivityOX);
			camera->RotateFirstPerson_OY(deltaX * sensivityOY);
		}

		if (window->GetSpecialKeyState() && GLFW_MOD_CONTROL) {
			renderCameraTarget = true;
			camera->RotateThirdPerson_OX(deltaY * sensivityOX);
			camera->RotateThirdPerson_OY(deltaX * sensivityOY);
		}

	}
}

void Skyroads::OnKeyRelease(int key, int mods)
{
	// Daca nu mai este apasata tasta W, atunci se revine la viteza de deplasare initiala.
	if (key == GLFW_KEY_W) {
		playerSpeed = initialSpeed;
	}

	// Daca nu mai este apasata tasta S, atunci se revine la viteza de deplasare initiala.
	if (key == GLFW_KEY_S) {
		playerSpeed = initialSpeed;
	}

	// Daca nu mai e apasat SPACE, atunci revenim la viteza de lansare initiala.
	if (key == GLFW_KEY_SPACE && !isInAir) {
		jumpPlayer = 7;
	}
}