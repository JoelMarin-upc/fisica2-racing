#pragma once

#include "Globals.h"
#include "Module.h"
#include "Timer.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>

class PhysBody;
class PhysicEntity;
class Car;
class Checkpoint;
class Finishline;

class ModuleGame : public Module
{
public:
	ModuleGame(Application* app, bool start_enabled = true);
	~ModuleGame();

	bool Start();
	update_status Update();
	bool CleanUp();

	void CreateMap();
	void AddCars();
	void PerformCountdown();
	void StartRace();
	void EndRace();
	void GetInput();
	void PerformNitro();

	Timer countdownTimer;
	const double countdownTime = 3.f;
	
	Timer lapTimer;
	double lapTime;
	double raceTime;

	const int totalLaps = 3;
	const int totalCars = 8;

	std::vector<PhysicEntity*> entities;
	std::vector<Car*> cars;
	Car* car;
	std::vector<Checkpoint*> checkpoints;
	Finishline* finishline;

	Vector2* movementInput;
	bool nitro;

	Timer nitroTimer;
	const double maxNitroTime = 2.f;

};
