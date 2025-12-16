#pragma once

#include "Application.h"
#include "BoxSensor.h"
#include "PhysicEntity.h"
#include "ModulePhysics.h"

class Booster : public BoxSensor
{
public:
	Booster(Application* app, int _x, int _y, int width, int height, float angle, Module* _listener, double _boostScale, double _boostTime)
		: BoxSensor(app->physics, app->renderer, _x, _y, width, height, _listener, BOOSTER, angle, false), boostScale(_boostScale), boostTime(_boostTime)
	{

	}

	void OnCollision(PhysicEntity* other, bool isSensor) {

	}

	void OnCollisionEnd(PhysicEntity* other, bool isSensor) {

	}

	~Booster() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}

	double boostScale;
	double boostTime;
};