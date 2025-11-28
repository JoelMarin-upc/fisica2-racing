#pragma once

#include "Application.h"
#include "BoxSensor.h"
#include "PhysicEntity.h"
#include "ModulePhysics.h"

class Checkpoint : public BoxSensor
{
public:
	Checkpoint(Application* app, int _x, int _y, int width, int height, float angle, Module* _listener, int _order)
		: BoxSensor(app->physics, _x, _y, width, height, _listener, CHECKPOINT, angle, false), order(_order)
	{

	}

	void OnCollision(PhysicEntity* other) {

	}

	~Checkpoint() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}

	int order;
};