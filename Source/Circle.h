#pragma once

#include "PhysicEntity.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"

class Circle : public PhysicEntity
{
public:
	Circle(ModulePhysics* physics, ModuleRender* render, int _x, int _y, Module* _listener, Texture2D _texture, EntityType type, float angle = 0.f, bool dynamic = true, float restitution = 0.f, double colliderRadius = 0.f)
		: PhysicEntity(physics->CreateCircle(_x, _y, colliderRadius < EPSILON ? _texture.height / 2 : colliderRadius, angle, dynamic, restitution), physics, render, _listener, type)
		, texture(_texture)
	{
		body->type = type;
	}

	~Circle() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
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

	void OnCollision(PhysicEntity* other, bool isSensor) {

	}

	void OnCollisionEnd(PhysicEntity* other, bool isSensor) {

	}

	Texture2D texture;

};