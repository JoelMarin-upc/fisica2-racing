#pragma once

#include "Globals.h"
#include "Module.h"
#include "Timer.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>

#include "Car.h"
#include "Finishline.h"
#include "Checkpoint.h"
#include "SlowZone.h"
#include "Map.h"
#include "PhysicEntity.h"

class Car;

class ModuleGame : public Module
{
public:
	ModuleGame(Application* app, bool start_enabled = true);
	~ModuleGame();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void LoadMap();
	void AddCars();
	void PerformCountdown();
	void StartRace();
	void EndRace();
	void GetInput();
	void AdjustCamera();
	void CalculatePositions();
	void CreateMouseJoint();
	void DestroyMouseJoint();
	void UpdateMouseJoint();
	void MarkLap(int doneLap);
	void RunTimer();
	void PrintInfo();
	void PrintEndScreen();
	void Restart();

	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void OnCollisionEnd(PhysBody* bodyA, PhysBody* bodyB);

	Timer countdownTimer;
	const double countdownTime = 3.f;
	
	Timer lapTimer;
	double lapTime = 0;
	double bestLapTime = 0;
	double bestLap = 0;
	double raceTime = 0;

	bool raceEnded = false;
	bool raceActive = false;
	bool countdownStarted = false;

	const int totalLaps = 1;
	const int totalCars = 4;

	//std::vector<PhysicEntity*> entities;
	std::vector<Car*> cars;
	Car* car; // should be in cars list
	Map* map;

	Vector2* movementInput;
	bool nitroInput;

	b2MouseJoint* mouseJoint = nullptr;

	Font fontTitle;
	Font fontSubtitle;
	Font fontText;
	Font fontSmall;
	const char* fontPath = "Assets/static/ScienceGothic_Condensed-Black.ttf";

};
