#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include <iostream>
#include <string>

ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

ModuleGame::~ModuleGame()
{}

// Load assets
bool ModuleGame::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	CreateMap();
	AddCars();

	return ret;
}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

void ModuleGame::CreateMap()
{
	// add circuit, checkpoints and finishline
	map = new Map(App, 0, 0, 0, this, LoadTexture("Assets/map.png"));
	//map->checkpoints ...
	//map->finishline ...
	//map->playerStartPositions ...
}

void ModuleGame::AddCars()
{
	for (int i = 0; i < totalCars; i++)
	{
		b2Vec2 pos = map->playerStartPositions[i];
		const std::string tex = "Assets/car" + std::to_string(i + 1) + ".png";
		cars.push_back(new Car(App, pos.x, pos.y, 0, this, LoadTexture(tex.c_str()), i++, false));
	}
	b2Vec2 playerPos = map->playerStartPositions[totalCars];
	car = new Car(App, playerPos.x, playerPos.y, 0, this, LoadTexture("Assets/carPlayer.png"), totalCars, true);
	cars.push_back(car);
}

void ModuleGame::PerformCountdown()
{
	if (raceActive) return;
	if (!countdownStarted)
	{
		countdownTimer.Start();
		countdownStarted = true;
	}
	int currentNumber = 3 - floor(countdownTimer.ReadSec());
	std::string countdownText = "";
	if (currentNumber == 0) countdownText = "Start";
	else if (currentNumber == -1) StartRace();
	else countdownText = std::to_string(currentNumber);
	App->renderer->DrawText(countdownText.c_str(), GetScreenWidth() / 2, GetScreenHeight() / 2, { 20 }, 5, { 255, 0, 0, 255 });
}

void ModuleGame::StartRace()
{
	raceActive = true;
	for (Car* c : cars) c->Enable();
}

void ModuleGame::EndRace()
{
	raceActive = false;
	for (Car* c : cars) c->Disable();
}

void ModuleGame::GetInput()
{
	movementInput = new Vector2();
	nitroInput = false;
	if (IsKeyDown(KEY_RIGHT)) movementInput->x = 1;
	if (IsKeyDown(KEY_LEFT)) movementInput->x = -1;
	if (IsKeyDown(KEY_UP)) movementInput->y = -1;
	if (IsKeyDown(KEY_DOWN)) movementInput->y = 1;
	if (IsKeyPressed(KEY_SPACE)) nitroInput = true;
}

void ModuleGame::AdjustCamera()
{
	// adjust position and rotation of the camera to have the players car in the center
	// App->renderer->camera
}

// Update: draw background
update_status ModuleGame::Update(float dt)
{
	
	if (raceActive) GetInput();
	else PerformCountdown();

	map->Update(dt);
	for (Car* c : cars) c->Update(dt);

	AdjustCamera();

	//for (PhysicEntity* entity : entities) entity->Update(dt);

	return UPDATE_CONTINUE;
}
