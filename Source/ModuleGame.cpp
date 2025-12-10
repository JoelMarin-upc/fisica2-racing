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

bool ModuleGame::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	fontTitle = LoadFontEx(fontPath, 60, nullptr, 0);
	fontSubtitle = LoadFontEx(fontPath, 50, nullptr, 0);
	fontText = LoadFontEx(fontPath, 30, nullptr, 0);
	fontSmall = LoadFontEx(fontPath, 20, nullptr, 0);

	LoadMap();
	AddCars();
	/*car = new Car(App, 30, 500, 0, this, LoadTexture("Assets/car1.png"), 1, true);
	cars.push_back(car);
	cars.push_back(new Car(App, 100, 500, 0, this, LoadTexture("Assets/car1.png"), 2, false));*/

	return ret;
}

bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

void ModuleGame::LoadMap()
{
	// adds circuit, checkpoints and finishline
	map = MapLoader::LoadMap(1, App, this);
}

void ModuleGame::AddCars()
{
	for (int i = 0; i < totalCars - 1; i++)
	{
		Transform2D t = map->playerStartPositions[i];
		const std::string tex = "Assets/Cars/car" + std::to_string(i + 1) + ".png";
		cars.push_back(new Car(App, t.position.x, t.position.y, t.rotation, this, LoadTexture(tex.c_str()), i + 1, false));
	}
	Transform2D playerTransform = map->playerStartPositions[totalCars-1];
	car = new Car(App, playerTransform.position.x, playerTransform.position.y, playerTransform.rotation, this, LoadTexture("Assets/Cars/carPlayer.png"), totalCars, true);
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
	App->renderer->DrawTextCentered(countdownText.c_str(), GetScreenWidth() / 2, GetScreenHeight() / 2, fontTitle, 5, YELLOW);
}

void ModuleGame::StartRace()
{
	raceActive = true;
	for (Car* c : cars) c->Enable();
}

void ModuleGame::EndRace()
{
	raceActive = false;
	raceEnded = true;
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
	int x, y;
	car->body->GetPhysicPosition(x, y);
	App->renderer->camera.x = -x + GetScreenWidth() / 2.f;
	App->renderer->camera.y = -y + GetScreenHeight() / 2.f;
	//car->body->GetRotation();
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
			return c1->distanceToLastCheckpoint > c2->distanceToLastCheckpoint;
		}
	);

	for (int i = 0; i < cars.size(); i++) cars[i]->currentPosition = i + 1;
}

void ModuleGame::CreateMouseJoint()
{
	bool jointDone = false;
	Vector2 mouse = GetMousePosition();
	b2Vec2 mouseb2 = { mouse.x - App->renderer->camera.x, mouse.y - App->renderer->camera.y };
	for (auto& c : cars) {
		if (c->TestPoint(mouseb2)) {
			mouseJoint = App->physics->CreateMouseJoint(c->body->body, mouseb2);
			jointDone = true;
		}
	}
	//if (!jointDone) mouseJoint = App->physics->CreateMouseJoint(car->body->body, mouseb2);
}

void ModuleGame::DestroyMouseJoint()
{
	App->physics->DestroyJoint(mouseJoint);
	mouseJoint = nullptr;
}

void ModuleGame::UpdateMouseJoint()
{
	if (mouseJoint) {
		Vector2 mouse = GetMousePosition();
		float x = mouse.x - App->renderer->camera.x;
		float y = mouse.y - App->renderer->camera.y;
		b2Vec2 mouseb2 = { PIXEL_TO_METERS(x), PIXEL_TO_METERS(y) };
		mouseJoint->SetTarget(mouseb2);
		App->renderer->rDrawLine(METERS_TO_PIXELS(mouseb2.x),
			METERS_TO_PIXELS(mouseb2.y),
			METERS_TO_PIXELS(mouseJoint->GetBodyB()->GetPosition().x),
			METERS_TO_PIXELS(mouseJoint->GetBodyB()->GetPosition().y),
			RED);
	}
}

void ModuleGame::MarkLap(int doneLap)
{
	if (doneLap == 0) {
		lapTimer = Timer();
		bestLapTime = INT64_MAX;
	}
	else {
		if (bestLapTime > lapTime)
		{
			bestLap = doneLap;
			bestLapTime = lapTime;
		}
		raceTime += lapTime;
	}
	lapTimer.Start();
}

void ModuleGame::RunTimer()
{
	if (car->currentLap > 0) lapTime = lapTimer.ReadSec();
}

void ModuleGame::PrintInfo()
{
	App->renderer->DrawText(TextFormat("Last checkpoint: %d", car->currentCheckpointNum), 10, 30, fontText, 5, RED);
	App->renderer->DrawText(TextFormat("Laps: %d", car->currentLap), 10, 50, fontText, 5, RED);
	App->renderer->DrawText(TextFormat("Position: %d", car->currentPosition), 10, 70, fontText, 5, RED);
	App->renderer->DrawText(TextFormat("Lap time: %02.02f s", lapTime), 10, 90, fontText, 5, RED);
	App->renderer->DrawText(TextFormat("Total time: %02.02f s", raceTime), 10, 110, fontText, 5, RED);
	App->renderer->DrawText(TextFormat("Avaliable nitros: %d", car->availableNitros), 10, 130, fontText, 5, RED);
	for (int i = 0; i < cars.size(); i++)
	{
		Car* c = cars[i];
		const char* text;
		if (c->isHumanControlled) text = TextFormat("Position %d -> PLAYER", c->currentPosition, c->carNum);
		else text = TextFormat("Position %d -> CAR %d", c->currentPosition, c->carNum);
		App->renderer->DrawText(text, 10, 160 + 15 * (i + 1), fontSmall, 5, RED);
	}
}

void ModuleGame::PrintEndScreen()
{
	int centerX = GetScreenWidth() / 2;
	int centerY = GetScreenHeight() / 2;
	App->renderer->DrawTextCentered(TextFormat("Position: %d", car->currentPosition), centerX, centerY - 50, fontTitle, 5, YELLOW);
	App->renderer->DrawTextCentered(TextFormat("Race time: %02.02f s", raceTime), centerX, centerY, fontSubtitle, 5, YELLOW);
	App->renderer->DrawTextCentered(TextFormat("Best lap time: %02.02f s", bestLapTime), centerX, centerY + 30, fontSubtitle, 5, YELLOW);
	App->renderer->DrawTextCentered("Press [R] to restart", centerX, centerY + 100, fontSubtitle, 5, YELLOW);
}

void ModuleGame::Restart()
{
	for (auto& c : cars) {
		delete c;
		c = nullptr;
	}
	cars.clear();
	delete map;
	map = nullptr;

	lapTime = 0;
	bestLapTime = 0;
	bestLap = 0;
	raceTime = 0;

	raceEnded = false;
	raceActive = false;
	countdownStarted = false;

	LoadMap();
	AddCars();
}

update_status ModuleGame::Update(float dt)
{
	if (IsKeyPressed(KEY_F1)) {
		if (mouseJoint) DestroyMouseJoint();
		else CreateMouseJoint();
	}

	if (raceActive && !raceEnded)
	{
		GetInput();
		RunTimer();
	}

	map->Update(dt);

	for (Car* c : cars) c->Update(dt);

	AdjustCamera();
	UpdateMouseJoint();
	
	if (raceEnded) {
		PrintEndScreen();
		if (IsKeyPressed(KEY_R)) Restart();
	}
	else
	{
		CalculatePositions();
		PrintInfo();
	}

	if (!raceActive && !raceEnded) PerformCountdown();

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

	if (map->boundsIn->body == bodyA) entityA = map;
	if (map->boundsIn->body == bodyB) entityB = map;

	if (map->boundsOut->body == bodyA) entityA = map;
	if (map->boundsOut->body == bodyB) entityB = map;

	if (map->navigationLayerIn->body == bodyA) entityA = map;
	if (map->navigationLayerIn->body == bodyB) entityB = map;

	if (map->navigationLayerOut->body == bodyA) entityA = map;
	if (map->navigationLayerOut->body == bodyB) entityB = map;

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

	for (auto& z : map->slowZones) {
		if (z->body == bodyA) entityA = z;
		if (z->body == bodyB) entityB = z;
	}

	if (entityA/* && entityB*/) {
		entityA->OnCollision(entityB);
		//entityB->OnCollision(entityA);
	}
}

void ModuleGame::OnCollisionEnd(PhysBody* bodyA, PhysBody* bodyB)
{
	PhysicEntity* entityA = nullptr;
	PhysicEntity* entityB = nullptr;

	for (auto& c : cars) {
		if (c->body == bodyA) entityA = c;
		if (c->body == bodyB) entityB = c;
	}

	if (map->body == bodyA) entityA = map;
	if (map->body == bodyB) entityB = map;

	if (map->boundsIn->body == bodyA) entityA = map;
	if (map->boundsIn->body == bodyB) entityB = map;

	if (map->boundsOut->body == bodyA) entityA = map;
	if (map->boundsOut->body == bodyB) entityB = map;

	if (map->navigationLayerIn->body == bodyA) entityA = map;
	if (map->navigationLayerIn->body == bodyB) entityB = map;

	if (map->navigationLayerOut->body == bodyA) entityA = map;
	if (map->navigationLayerOut->body == bodyB) entityB = map;

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

	for (auto& z : map->slowZones) {
		if (z->body == bodyA) entityA = z;
		if (z->body == bodyB) entityB = z;
	}

	if (entityA/* && entityB*/) {
		entityA->OnCollisionEnd(entityB);
		//entityB->OnCollisionEnd(entityA);
	}
}
