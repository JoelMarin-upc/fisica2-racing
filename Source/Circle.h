#pragma once

#include "PhysicEntity.h"
#include "ModulePhysics.h"

class Circle : public PhysicEntity
{
public:
	Circle(ModulePhysics* physics, ModuleRender* render, int _x, int _y, Module* _listener, Texture2D _texture, EntityType type, float angle = 0.f, bool dynamic = true, float restitution = 0.f)
		: PhysicEntity(physics->CreateCircle(_x, _y, _texture.height / 2, angle, dynamic, restitution), physics, render, _listener, type)
		, texture(_texture)
	{
		body->type = type;
	}

	void Update(float dt) override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		Vector2 position{ (float)x, (float)y };
		float scale = 1.0f;
		Rectangle source = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
		Rectangle dest = { position.x, position.y, (float)texture.width * scale, (float)texture.height * scale };
		Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };
		float rotation = body->GetRotation() * RAD2DEG;
		render->rDrawTexturePro(texture, source, dest, origin, rotation, WHITE);
	}

	void OnCollision(PhysicEntity* other) {

	}

	void OnCollisionEnd(PhysicEntity* other) {

	}

	Texture2D texture;

};