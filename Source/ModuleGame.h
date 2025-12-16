#pragma once

#include "Globals.h"
#include "Module.h"
#include "Timer.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>
#include <map>

#include "Car.h"
#include "Finishline.h"
#include "Checkpoint.h"
#include "SlowZone.h"
#include "Map.h"
#include "MapLoader.h"
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

	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void OnCollisionEnd(PhysBody* bodyA, PhysBody* bodyB);
	
	void MarkLap(int doneLap);
	void EndRace();

private:
	void LoadMap();
	void AddCars();
	void PerformCountdown();
	void StartRace();
	void GetInput();
	void CalculatePositions();
	void CreateMouseJoint();
	void DestroyMouseJoint();
	void UpdateMouseJoint();
	void RunTimer();
	void GetMenuInput();
	void PrintMenu();
	void PrintInfo();
	void PrintEndScreen();
	void Restart();

public:
	int totalLaps = 3;
	Vector2* movementInput;
	bool nitroInput;

	Map* map;

private:
	Timer countdownTimer;
	const double countdownTime = 3.f;
	
	Timer lapTimer;
	double lapTime = 0;
	double bestLapTime = 0;
	int bestLap = 0;
	double raceTime = 0;

	bool gameStarted = false;
	bool raceEnded = false;
	bool raceActive = false;
	bool countdownStarted = false;
	bool endFxPlayed = false;

	std::vector<Car*> cars;
	Car* car; // should be in cars list

	b2MouseJoint* mouseJoint = nullptr;

	std::map<int, Texture2D> positionTex;
	Texture2D menuBack;

	Font fontMainTitle;
	Font fontTitle;
	Font fontSubtitle;
	Font fontText;
	Font fontSmall;
	const char* fontPath = "Assets/ScienceGothic_Condensed-Black.ttf";

	int menuOption = 1;
	int mapNumber = 1;
	int difficulty = 3;

	int startFX;
	int countdownFX;
	int winFX;
	int looseFX;
	int runFX;
	int crashFX;
	int carCrashFX;

	int sprintFX;

	int music;

	
};
