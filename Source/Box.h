#pragma once

#include "PhysicEntity.h"
#include "ModulePhysics.h"

class Box : public PhysicEntity
{
public:
	Box(ModulePhysics* physics, ModuleRender* render, int _x, int _y, Module* _listener, Texture2D _texture, EntityType type, float angle = 0.f, bool dynamic = true, float restitution = 0.f)
		: PhysicEntity(physics->CreateRectangle(_x, _y, _texture.width, _texture.height, angle, dynamic, restitution), physics, render, _listener, type)
		, texture(_texture)
	{
		body->type = type;
	}

	void Update(float dt) override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		render->rDrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
	}

	void OnCollision(PhysicEntity* other) {

	}

	void OnCollisionEnd(PhysicEntity* other) {

	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

	Texture2D texture;

};