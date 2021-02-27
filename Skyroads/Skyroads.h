
#pragma once
#include <Component/SimpleScene.h>
#include "CameraSkyroads.h"
#include "Player.h"
#include "Transform3D.h"
#include "Platform.h"
#include "Math.h"

#include <iostream> 
#include <iterator> 
#include <map> 

class Skyroads : public SimpleScene
{
	public:
		Skyroads();
		~Skyroads();

		void InitGameInterface();

		void Init() override;

		float left = -2.5f;
		float right = 2.5f;
		float bottom = -1.5f;
		float top = 1.5f;
		float zNear = 2;
		float zFar = 25;

		bool isOrthoProiection = false;

		float FoV_angle = 45.0f;
		float aspect = 1;

	private:
		void FrameStart() override;

		float GetColor(float color);

		void RenderLifes();
		
		void GetSevenSegments();

		void GetDigit(int digit, float offset);

		void RenderMinus(float offset);

		void RenderScore(int score);

		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);
		void RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix) override;

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;

		void OnKeyRelease(int key, int mods) override;

		float scalingFactor = 0.5f;
		float initialRadius = 1;

		// Metoda folosita pentru a detecta coliziunile
		bool CollisionDetection(glm::vec4 platformCoord);
		
		// Metode pentru modurile jocului: PAUZA/JOC TERMINAT
		void GameModesHandler(int key);

		// Metode pentru player
		void playerMoveHandler(float deltaTime);
		void RenderPlayer(float deltaTimeSeconds);

		// Metoda ce va afisa nivelul de combustibil
		void RenderFuel(float deltaTimeSeconds);

		// Metode pentru platforme
		void RenderMap();
		void PlayerActionsHandler(float deltaTimeSeconds);
		void ChangingPlatformsHandler(float deltaTimeSeconds);
		bool SetCameraMode();
		void GetOutPlayer(float deltaTimeSeconds, glm::vec4 lastPosition);
		void DetectCollisionsWithSpecialEffects(float platformColor, glm::vec4 playerPosition);
		bool SpecialCollisionsHandler(float deltaTimeSeconds);
		void ResetPlayerAttributes();
		void RenderPlatform(float zPosition, float xPosition, glm::vec3 color);
		void DrawInitialMap();
		void GenerateOnePlatform(float z);
		void GenerateTwoPlatforms(float z);
		void GenerateThreePlatforms(float z);
		void GeneratePlatformLine();
		void DrawPlatforms();

		glm::vec3 ColorHandler(float color);


	protected:
		Laborator::Camera *camera;
		glm::mat4 projectionMatrix;
		bool renderCameraTarget;

		// Mesh pentru combustibil
		Mesh* whitePolygon;
		Mesh* fuel;

		glm::vec3 leftBottomCorner = glm::vec3(1.95f, -0.1f, 0);
		glm::vec3 leftCornerFuel = glm::vec3(2, 0, 0);
		float width = 0.25f;
		float height = 2;

		// Instanta player-ului.
		Player* player;
		
		// Instanta a unei platforme.
		Platform* platform;

		// Modurile gameOver si Pause
		bool isGameOver = false;
		bool isPause = false;
		bool isChangingPlatformToLeft = false;
		bool isChangingPlatformToRight = false;
		bool isStartGame = false;
		bool isRestartGame = false;

		// Centrul initial al jucatorului. Il plasam la (0, 0, 0).
		glm::vec4 playerCenter = glm::vec4(0, 0, 0, 0);
		glm::vec4 platformCenter = playerCenter;

		// Numarul de vieti initial al jucatorului si cantitatea initiala de combustibil.
		int playerFuel = 100;
		int playerLifes = 3;

		// Viteza initiala de mers a player-ului.
		float playerSpeed = 0.5f;
		float initialSpeed = 1;
		float maxSpeed = 10;

		// Coordonatele cu care se va deplasa player-ul.
		float velZ = 0;
		float velY = 0;
		float velX = 0;

		float jumpPlayer = 7;

		// Transferul dintre FirstPerson -> ThirdPerson.
		bool isCpressed = false; //se vede sfera
	
		// Dimensiunile la care va fi scalata o platforma
		float xScale = 1;
		float yScale = 0.1f;
		float zScale = 1;

		// Retinem intr-un vector de coordonate de platforme, platformele randate sub forma de coordonate
		// Daca imi trebuie si culorile pot sa pun pe a 4a coordonata o codificare pentru fiecare culoare
		// codificarea din culoarea o trec din binar in deci
		std::vector<glm::vec4> platformsMap;

		// Variabila ce va fi folosita pentru a se misca platformele catre player.
		float zMovePlatforms = 0;

		//Variabile boolene pentru a seta daca tastele W/S/SPACE sunt apasate.
		bool isWPressed = false;
		bool isSPressed = false;
		bool isSpacePressed = false;

		// Distanta dintre jucator si ultima platforma va fi mereu de 19.
		float DISTANCE = 19;

		// Axa OY
		glm::vec3 OyAxis = glm::vec3(0, 1, 0);

		// Vector de 4 platforme generate la un moment de timp
		std::vector<glm::vec4> platformLine;

		// Pentru a scala cantitatea de combustibil
		float fuelScale = 0;
		bool isStaying = false;

		// Coliziuni cu platforme
		bool isCollisionRedPlatform = false;
		bool isCollisionGreenPlatform = false;
		bool isCollisionYellowPlatform = false;
		bool isCollisionOrangePlatform = false;

		glm::vec4 initialCoord = glm::vec4(0, 0.28f, 0, 0);

		// Timpul in care se va misca cu o viteza mare player-ul
		float seconds = 50;

		// Vectorul ce va pastra coordonatele celor 7 segmente pentru o cifra.
		std::vector<glm::vec4> sevenSegmentsCoord;

		int score = 0;
		bool isGettingScore = false;
		bool isLosingPoints = false;

		// Player-ul este in aer sau nu, in momentul sariturii.
		bool isInAir = false;
};
