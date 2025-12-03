#pragma once

#include "Module.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "p2Point.h"

class PhysicEntity
{
protected:

	PhysicEntity(PhysBody* _body, ModulePhysics* _physics, ModuleRender* _render, Module* _listener, EntityType type)
		: body(_body)
		, physics(_physics)
		, render(_render)
		, listener(_listener)
		, type(type)
	{
		body->listener = listener;
	}

public:
	virtual ~PhysicEntity() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}
	virtual void Update(float dt) = 0;
	virtual void OnCollision(PhysicEntity* other) = 0;
	virtual void OnCollisionEnd(PhysicEntity* other) = 0;
	virtual int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
	{
		return 0;
	}
	bool TestPoint(b2Vec2 point)
	{
		return physics->TestPoint(body->body, point);
	}

	PhysBody* body;
	Module* listener;
	EntityType type;
	ModulePhysics* physics;
	ModuleRender* render;

};
