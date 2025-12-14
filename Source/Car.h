#pragma once

#include "Application.h"
#include "ModuleAudio.h"
#include "Box.h"

class ModuleGame;

class Car : public Box
{
public:
	Car(Application* app, int _x, int _y, float angle, Module* _listener, Texture2D _texture, int carNum, bool isHuman, int _sprintFXId, int _runFXId, int _crashFXId, int _difficulty = 1);

	~Car();

	void Enable();

	void Disable();

	void Update(float dt) override;

	void OnCollision(PhysicEntity* other, bool isSensor);

	void OnCollisionEnd(PhysicEntity* other, bool isSensor);

private:
	void GetInput();

	void AI();

	void GetTargetDirection();

	void Move(float dt);

	void PlayRunAudio();

	void CheckNitro();

	void SetSpeedScale(float scale = 1);

public:
	int carNum;

	int currentPosition;
	int currentLap;
	int currentCheckpointNum;
	int distanceToLastCheckpoint;
	
	bool isHumanControlled;
	
	int availableNitros;

	double engineTemperature;
	double maxEngineTemperature;
	bool isOverHeated;

private:
	const float engineForce = 5.f;
	const float steerStrength = 2.5f;
	const float nitroMultiplier = 2.f;
	
	Vector2* targetDirection;
	bool active;

	ModuleGame* game;
	ModuleAudio* audio;

	Timer nitroTimer;
	const double nitroTime = 2.f;
	const int maxAvailableNitros = 3;
	bool nitroActive;
	bool nitroInput;

	double speedScale = 1;
	int sprintFX;
	int runFX;
	int crashFX;

	Timer runFXTimer;
	const double runFXSeconds = 3.f;

	const float warmingVelocityThreshold = 9;

	int difficulty = 1;
};