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

	//sounds
	countdownFX = App->audio->LoadFx("Assets/Sounds/FX/countdown.wav");
	startFX = App->audio->LoadFx("Assets/Sounds/FX/start.wav");
	looseFX = App->audio->LoadFx("Assets/Sounds/FX/loose.wav");
	winFX = App->audio->LoadFx("Assets/Sounds/FX/win.wav");
	runFX = App->audio->LoadFx("Assets/Sounds/FX/run.wav");
	crashFX = App->audio->LoadFx("Assets/Sounds/FX/bounce.wav");

	App->audio->PlayMusic("Assets/Sounds/Music/GlooGloo.wav");

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
	map = MapLoader::LoadMap(mapNumber, App, this);
}

void ModuleGame::AddCars()
{
	for (int i = 0; i < map->totalCars - 1; i++)
	{
		Transform2D t = map->playerStartPositions[i];
		const std::string tex = map->carsBasePath + "car" + std::to_string(i + 1) + ".png";
		cars.push_back(new Car(App, t.position.x, t.position.y, t.rotation, this, LoadTexture(tex.c_str()), i + 1, false, difficulty));
	}
	Transform2D playerTransform = map->playerStartPositions[map->totalCars-1];
	const std::string playerTex = map->carsBasePath + "carPlayer.png";
	car = new Car(App, playerTransform.position.x, playerTransform.position.y, playerTransform.rotation, this, LoadTexture(playerTex.c_str()), map->totalCars, true);
	cars.push_back(car);
}

void ModuleGame::PerformCountdown()
{
	if (raceActive) return;
	if (!countdownStarted)
	{
		countdownTimer.Start();
		countdownStarted = true;
		App->audio->PlayFx(countdownFX);
	}
	int currentNumber = 3 - floor(countdownTimer.ReadSec());
	std::string countdownText = "";
	if (currentNumber == 0) {
		countdownText = "Start";
		App->audio->PlayFx(startFX);
	}
	else if (currentNumber == -1) StartRace();
	else countdownText = std::to_string(currentNumber);
	App->renderer->rDrawTextCentered(countdownText.c_str(), GetScreenWidth() / 2, GetScreenHeight() / 2, fontTitle, 5, YELLOW);
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
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) movementInput->x = 2;
	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) movementInput->x = -2;
	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) movementInput->y = -1.3;
	if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) movementInput->y = 1;
	if (IsKeyPressed(KEY_SPACE)) nitroInput = true;
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
	//b2Vec2 mouseb2 = { mouse.x - App->renderer->camera.target.x, mouse.y - App->renderer->camera.target.y };
	Vector2 mouseWorld = GetScreenToWorld2D(mouse, App->renderer->camera);
	b2Vec2 mouseb2 = { mouseWorld.x, mouseWorld.y };
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
		/*float x = mouse.x - App->renderer->camera.target.x;
		float y = mouse.y - App->renderer->camera.target.y;
		b2Vec2 mouseb2 = { PIXEL_TO_METERS(x), PIXEL_TO_METERS(y) };*/
		Vector2 mouseWorld = GetScreenToWorld2D(mouse, App->renderer->camera);
		b2Vec2 mouseb2 = { PIXEL_TO_METERS(mouseWorld.x), PIXEL_TO_METERS(mouseWorld.y) };
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

void ModuleGame::GetMenuInput()
{
	if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
	{
		menuOption++;
		if (menuOption > 4) menuOption = 1;
	}
	if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
	{
		menuOption--;
		if (menuOption < 1) menuOption = 4;
	}
	if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
	{
		if (menuOption == 1)
		{
			mapNumber--;
			if (mapNumber < 1) mapNumber = 2;
		}
		if (menuOption == 2)
		{
			difficulty--;
			if (difficulty < 1) difficulty = 3;
		}
		if (menuOption == 4)
		{
			totalLaps--;
			if (totalLaps < 1) totalLaps = 5;
		}
	}
	if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
	{
		if (menuOption == 1)
		{
			mapNumber++;
			if (mapNumber > 2) mapNumber = 1;
		}
		if (menuOption == 2)
		{
			difficulty++;
			if (difficulty > 3) difficulty = 1;
		}
		if (menuOption == 3)
		{
			totalLaps++;
			if (totalLaps > 5) totalLaps = 1;
		}
	}
	if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
	{
		if (menuOption == 4)
		{
			gameStarted = true;
			LoadMap();
			AddCars();
			App->renderer->SetCameraTarget(car);
		}
	}
}

void ModuleGame::PrintMenu()
{
	Color selected = RED;
	Color unselected = BLUE;
	
	std::string mapName;
	switch (mapNumber)
	{
	case 1:
		mapName = "River";
		break;
	case 2:
		mapName = "Road";
		break;
	default:
		mapName = "None";
		break;
	}
	std::string diffName;
	switch (difficulty)
	{
	case 1:
		diffName = "Easy";
		break;
	case 2:
		diffName = "Medium";
		break;
	case 3:
		diffName = "Hard";
		break;
	default:
		diffName = "None";
		break;
	}
	
	int screenCenterX = GetScreenWidth() / 2;
	int screenCenterY = GetScreenHeight() / 2;

	App->renderer->rDrawTextCentered(TextFormat("Map: < %s >", mapName.c_str()), screenCenterX, screenCenterY - 45, fontText, 5, menuOption == 1 ? selected : unselected);
	App->renderer->rDrawTextCentered(TextFormat("Difficulty: < %s >", diffName.c_str()), screenCenterX, screenCenterY - 15, fontText, 5, menuOption == 2 ? selected : unselected);
	App->renderer->rDrawTextCentered(TextFormat("Laps: < %i >", totalLaps), screenCenterX, screenCenterY + 15, fontText, 5, menuOption == 3 ? selected : unselected);
	App->renderer->rDrawTextCentered("Start game", screenCenterX, screenCenterY + 45, fontSubtitle, 5, menuOption == 4 ? selected : unselected);
}

void ModuleGame::PrintInfo()
{
	//App->renderer->DrawText(TextFormat("Last checkpoint: %d", car->currentCheckpointNum), 10, 30, fontText, 5, RED);
	App->renderer->rDrawText(TextFormat("Laps: %d/%d", car->currentLap, totalLaps), 10, 50, fontText, 5, RED);
	App->renderer->rDrawText(TextFormat("Position: %d/%d", car->currentPosition, map->totalCars), 10, 70, fontText, 5, RED);
	App->renderer->rDrawText(TextFormat("Lap time: %02.02f s", lapTime), 10, 90, fontText, 5, RED);
	App->renderer->rDrawText(TextFormat("Total time: %02.02f s", raceTime), 10, 110, fontText, 5, RED);
	App->renderer->rDrawText(TextFormat("Avaliable nitros: %d", car->availableNitros), 10, 130, fontText, 5, RED);
	for (int i = 0; i < cars.size(); i++)
	{
		Car* c = cars[i];
		const char* text;
		if (c->isHumanControlled) text = TextFormat("Position %d -> PLAYER", c->currentPosition, c->carNum);
		else text = TextFormat("Position %d -> CAR %d", c->currentPosition, c->carNum);
		App->renderer->rDrawText(text, 10, 160 + 15 * (i + 1), fontSmall, 5, RED);
	}
}

void ModuleGame::PrintEndScreen()
{
	int centerX = GetScreenWidth() / 2;
	int centerY = GetScreenHeight() / 2;

	if (car->currentPosition == 1) {
		App->renderer->rDrawTextCentered("Well played. Another Game?", centerX, centerY + 50, fontSubtitle, 5, YELLOW);
		App->audio->PlayFx(winFX);
	}
	if (car->currentPosition >= 2) {
		App->renderer->rDrawTextCentered("More luck next time...", centerX, centerY + 50, fontSubtitle, 5, YELLOW);
		App->audio->PlayFx(looseFX);
	}
	
	App->renderer->rDrawTextCentered(TextFormat("Position: %d", car->currentPosition), centerX, centerY - 50, fontTitle, 5, YELLOW);
	App->renderer->rDrawTextCentered(TextFormat("Race time: %02.02f s", raceTime), centerX, centerY, fontSubtitle, 5, YELLOW);
	App->renderer->rDrawTextCentered(TextFormat("Best lap time: %02.02f s", bestLapTime), centerX, centerY + 30, fontSubtitle, 5, YELLOW);
	App->renderer->rDrawTextCentered("Press [R] to restart", centerX, centerY + 100, fontSubtitle, 5, YELLOW);
}

void ModuleGame::Restart()
{
	if (mouseJoint) DestroyMouseJoint();
	
	App->renderer->SetCameraTarget(nullptr);

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

	gameStarted = false;
	raceEnded = false;
	raceActive = false;
	countdownStarted = false;
}

update_status ModuleGame::Update(float dt)
{
	if (gameStarted)
	{
		if (IsKeyPressed(KEY_C)) App->renderer->cameraRotationActive = !App->renderer->cameraRotationActive;
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

		if (gameStarted && !raceActive && !raceEnded) PerformCountdown();
	}
	else {
		GetMenuInput();
		PrintMenu();
	}

	//for (PhysicEntity* entity : entities) entity->Update(dt);

	return UPDATE_CONTINUE;
}

void ModuleGame::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	if (!gameStarted) return;

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
	if (!gameStarted) return;

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
