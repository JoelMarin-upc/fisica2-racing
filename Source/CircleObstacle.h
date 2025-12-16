#pragma once

#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "Circle.h"
#include "Animation.h"

class CircleObstacle : public Circle
{
public:
	CircleObstacle(ModulePhysics* physics, ModuleRender* render, int _x, int _y, Module* _listener, Texture2D _texture, EntityType type, float angle = 0.f, bool dynamic = true, float restitution = 0.f, std::string animationsPath = "", double colliderRadius = 0.f)
		: Circle(physics, render, _x, _y, listener, _texture, type, angle, dynamic, restitution, colliderRadius)
	{
		hasAnims = animationsPath != "";

		if (hasAnims)
		{
			std::unordered_map<int, std::string> aliases = { {0,"idle"} };
			anims.LoadFromTSX(animationsPath.c_str(), aliases);
			anims.SetCurrent("idle");
		}
	}

	~CircleObstacle() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}

	void Update(float dt) override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		if (hasAnims)
		{
			anims.Update(dt);
			const Rectangle& animFrame = anims.GetCurrentFrame();

			Rectangle source = animFrame;

			Rectangle dest = {
				x,
				y,
				animFrame.width,
				animFrame.height
			};

			render->rDrawTexturePro(texture, source, dest, { animFrame.width / 2.f, animFrame.height / 2.f }, body->GetRotation() * RAD2DEG, WHITE);
		}
		else
		{
			render->rDrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
				Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
				Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
		}
	}

	void OnCollision(PhysicEntity* other, bool isSensor) {

	}

	void OnCollisionEnd(PhysicEntity* other, bool isSensor) {

	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

	AnimationSet anims;
	bool hasAnims;

};