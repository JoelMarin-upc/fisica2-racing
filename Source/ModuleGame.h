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

	void CreateMap();
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

	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

	Timer countdownTimer;
	const double countdownTime = 3.f;
	
	Timer lapTimer;
	double lapTime;
	double raceTime;

	bool raceActive = false;
	bool countdownStarted = false;

	const int totalLaps = 3;
	const int totalCars = 8;

	//std::vector<PhysicEntity*> entities;
	std::vector<Car*> cars;
	Car* car; // should be in cars list
	Map* map;

	Vector2* movementInput;
	bool nitroInput;

	b2MouseJoint* mouseJoint = nullptr;

};
