#pragma once

#include "Application.h"
#include "Chain.h"
#include "Checkpoint.h"
#include "SlowZone.h"
#include "Finishline.h"
#include "PhysicEntity.h"
#include "ModulePhysics.h"
#include "Transform2D.h"

class Map : public Chain
{
public:
	Map(Application* app, int _x, int _y, float _angle, int* _points, unsigned int _size, Module* _listener, Texture2D _texture)
		: Chain(app->physics, app->renderer, _x, _y, _points, _size, _listener, _texture, CIRCUIT, _angle, false)
	{

	}

	void Update(float dt) override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		render->rDrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x + texture.width / 2, (float)y + texture.height / 2, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
		
		for (PhysicEntity* obstacle : obstacles) obstacle->Update(dt);
	}

	void OnCollision(PhysicEntity* other) {

	}

	void OnCollisionEnd(PhysicEntity* other) {

	}

	int getLastCheckpointOrder() {
		int max = -1;
		for (auto& c : checkpoints) if (c->order > max) max = c->order;
		return max;
	}

	void addStartPosition(Transform2D t) {
		playerStartPositions.push_back(t);
	}

	void addObstacle(PhysicEntity* o) {
		obstacles.push_back(o);
	}
	
	void addCheckPoint(Checkpoint* c) {
		checkpoints.push_back(c);
	}

	void addSlowZone(BoxSensor* z) {
		slowZones.push_back(z);
	}

	void addFinishLine(Finishline* f) {
		finishline = f;
		f->requiredCheckpoint = getLastCheckpointOrder();
	}

	Checkpoint* getCheckPoint(int checkpointNumber) {
		for (auto& c : checkpoints) if (c->order == checkpointNumber) return c;
		return nullptr;
	}

	~Map() {
		for (auto& c : checkpoints)
		{
			delete c;
			c = nullptr;
		}
		checkpoints.clear();
		for (auto& o : obstacles)
		{
			delete o;
			o = nullptr;
		}
		obstacles.clear();
		for (auto& z : slowZones)
		{
			delete z;
			z = nullptr;
		}
		slowZones.clear();
		delete finishline;
		finishline = nullptr;
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}

	std::vector<Transform2D> playerStartPositions;
	std::vector<PhysicEntity*> obstacles;
	std::vector<Checkpoint*> checkpoints;
	std::vector<BoxSensor*> slowZones;
	Finishline* finishline;
	
};
