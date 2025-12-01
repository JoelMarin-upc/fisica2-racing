#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

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
	//AddCars();
	car = new Car(App, 30, 500, 0, this, LoadTexture("Assets/car1.png"), 1, true);
	cars.push_back(car);
	cars.push_back(new Car(App, 100, 500, 0, this, LoadTexture("Assets/car1.png"), 2, false));

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
	map = MapLoader::LoadMap(1, App, this);
	map->addCheckPoint(new Checkpoint(App, 500, 100, 50, 200, 0, this, 1));
	map->addCheckPoint(new Checkpoint(App, 1000, 350, 50, 200, 90, this, 2));
	map->addCheckPoint(new Checkpoint(App, 500, 600, 50, 200, 0, this, 3));
	map->addFinishLine(new Finishline(App, 50, 350, 50, 200, 90, this));
	//map->playerStartPositions ...
	//map->obstacles ...
}

void ModuleGame::AddCars()
{
	for (int i = 0; i < totalCars - 1; i++)
	{
		Vector2 pos = map->playerStartPositions[i];
		const std::string tex = "Assets/car" + std::to_string(i + 1) + ".png";
		cars.push_back(new Car(App, pos.x, pos.y, 0, this, LoadTexture(tex.c_str()), i++, false));
	}
	Vector2 playerPos = map->playerStartPositions[totalCars-1];
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

void ModuleGame::CalculatePositions()
{
	for (auto& c : cars) {
		PhysBody* b;
		if (c->currentCheckpointNum == 0) b = map->finishline->body;
		else b = map->getCheckPoint(c->currentCheckpointNum)->body;

		c->distanceToLastCheckpoint = std::fabs(App->physics->GetDistance(c->body->body, b->body));
	}

	std::sort(cars.begin(), cars.end(),
		[](const Car* c1, const Car* c2) {
			if (c1->currentLap != c2->currentLap) return c1->currentLap > c2->currentLap;
			if (c1->currentCheckpointNum != c2->currentCheckpointNum) return c1->currentCheckpointNum > c2->currentCheckpointNum;
			return c1->distanceToLastCheckpoint < c2->distanceToLastCheckpoint;
		}
	);

	for (int i = 0; i < cars.size(); i++) cars[i]->currentPosition = i + 1;
}

// Update: draw background
update_status ModuleGame::Update(float dt)
{
	map->Update(dt);
	
	if (raceActive) GetInput();
	else PerformCountdown();

	for (Car* c : cars) c->Update(dt);

	AdjustCamera();
	App->renderer->DrawText(TextFormat("Last checkpoint: %d", car->currentCheckpointNum), 10, 30, {20}, 5, {255, 0, 0, 255});
	App->renderer->DrawText(TextFormat("Laps: %d", car->currentLap), 10, 50, {20}, 5, {255, 0, 0, 255});

	CalculatePositions();
	App->renderer->DrawText(TextFormat("Position: %d", car->currentPosition), 10, 70, { 20 }, 5, { 255, 0, 0, 255 });

	//for (PhysicEntity* entity : entities) entity->Update(dt);

	return UPDATE_CONTINUE;
}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	PhysicEntity* entityA = nullptr;
	PhysicEntity* entityB = nullptr;

	for (auto& c : cars) {
		if (c->body == bodyA) entityA = c;
		if (c->body == bodyB) entityB = c;
	}

	if (map->body == bodyA) entityA = map;
	if (map->body == bodyB) entityB = map;

	if (map->finishline->body == bodyA) entityA = map->finishline;
	if (map->finishline->body == bodyB) entityB = map->finishline;

	for (auto& c : map->checkpoints) {
		if (c->body == bodyA) entityA = c;
		if (c->body == bodyB) entityB = c;
	}

	for (auto& o : map->obstacles) {
		if (o->body == bodyA) entityA = o;
		if (o->body == bodyB) entityB = o;
	}

	if (entityA/* && entityB*/) {
		entityA->OnCollision(entityB);
		//entityB->OnCollision(entityA);
	}
}
