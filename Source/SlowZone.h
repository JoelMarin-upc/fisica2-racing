#pragma once

#include "Application.h"
#include "BoxSensor.h"
#include "PhysicEntity.h"
#include "ModulePhysics.h"

class SlowZone : public BoxSensor
{
public:
	SlowZone(Application* app, int _x, int _y, int width, int height, float angle, Module* _listener, double _slowScale)
		: BoxSensor(app->physics, app->renderer, _x, _y, width, height, _listener, SLOWZONE, angle, false), slowScale(_slowScale)
	{

	}

	void OnCollision(PhysicEntity* other, bool isSensor) {

	}

	void OnCollisionEnd(PhysicEntity* other, bool isSensor) {

	}

	~SlowZone() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}

	double slowScale;
};