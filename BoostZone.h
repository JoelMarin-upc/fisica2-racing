#pragma once

#include "Application.h"
#include "BoxSensor.h"
#include "PhysicEntity.h"
#include "ModulePhysics.h"
#include <cmath>

class BoostZone : public BoxSensor
{
public:
	BoostZone(Application* app, int _x, int _y, int width, int height, float angle, Module* _listener, float _impulse)
		: BoxSensor(app->physics, app->renderer, _x, _y, width, height, _listener, SLOWZONE, angle, false) // usamos SLOWZONE en el enum para no tocar enums globales
		, boostImpulse(_impulse)
	{

	}

	void OnCollision(PhysicEntity* other) {
		if (!other || !other->body) return;
		// Appliy impulse towards the direction of the player
		if (other->body->body) {
			float angle = other->body->body->GetAngle();
			float ix = cosf(angle) * boostImpulse;
			float iy = sinf(angle) * boostImpulse;
			other->body->ApplyImpulse(ix, iy);
		}
	}

	void OnCollisionEnd(PhysicEntity* other) {

	}

	~BoostZone() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}

	float boostImpulse;
};