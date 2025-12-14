#pragma once

#include "PhysicEntity.h"
#include "ModulePhysics.h"

class BoxSensor : public PhysicEntity
{
public:
	BoxSensor(ModulePhysics* physics, ModuleRender* render, int _x, int _y, int width, int height, Module* _listener, EntityType type, float angle = 0.f, bool dynamic = true)
		: PhysicEntity(physics->CreateRectangleSensor(_x, _y, width, height, angle, dynamic), physics, render, _listener, type)
	{
		body->type = type;
	}

	~BoxSensor() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}

	void Update(float dt) override
	{

	}

	void OnCollision(PhysicEntity* other, bool isSensor) {

	}

	void OnCollisionEnd(PhysicEntity* other, bool isSensor) {

	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

};