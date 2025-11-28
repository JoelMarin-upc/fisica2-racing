#pragma once

#include "Application.h"
#include "ModuleAudio.h"
#include "Box.h"

class ModuleGame;

class Car : public Box
{
public:
	Car(Application* app, int _x, int _y, float angle, Module* _listener, Texture2D _texture, int carNum, bool isHuman);

	~Car();

	void GetInput();

	void AI();

	void GetTargetDirection();

	void Move(float dt);

	void CheckNitro();

	void Enable();

	void Disable();

	void Update(float dt) override;

	void OnCollision(PhysicEntity* other);

	const float engineForce = 10.f;
	const float steerStrength = 12.f;
	const float nitroMultiplier = 2.f;
	
	Vector2* targetDirection;
	bool active;
	bool isHumanControlled;
	int carNum;
	int currentPosition;
	int currentLap;
	int currentCheckpointNum;

	ModuleGame* game;
	ModuleAudio* audio;

	Timer nitroTimer;
	const double nitroTime = 2.f;
	const int maxAvailableNitros = 3;
	int availableNitros = maxAvailableNitros;
	bool nitroActive;
	bool nitroInput;

};