#pragma once

#include "Application.h"
#include "BoxSensor.h"
#include "PhysicEntity.h"
#include "ModulePhysics.h"

class Finishline : public BoxSensor
{
public:
	Finishline(Application* app, int _x, int _y, int width, int height, float angle, Module* _listener)
		: BoxSensor(app->physics, _x, _y, width, height, _listener, FINISHLINE, angle, false)
	{

	}

	void OnCollision(PhysicEntity* other) {

	}

	~Finishline() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}

	int requiredCheckpoint;
};