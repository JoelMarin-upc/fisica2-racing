#pragma once

#include "Application.h"
#include "Chain.h"
#include "Checkpoint.h"
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
		render->DrawTexturePRO(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x + texture.width / 2, (float)y + texture.height / 2, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
		
		for (PhysicEntity* obstacle : obstacles) obstacle->Update(dt);
	}

	void OnCollision(PhysicEntity* other) {

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
		delete finishline;
		finishline = nullptr;
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}

	std::vector<Transform2D> playerStartPositions;
	std::vector<PhysicEntity*> obstacles;
	std::vector<Checkpoint*> checkpoints;
	Finishline* finishline;

	
};

class MapLoader
{
public:
	static Map* LoadMap(int mapNumber, Application* app, Module* listener) {
		if (mapNumber == 1)
		{
			int points[3] = { 1, 2, 3 };
			Map* map = new Map(app, 0, 0, 0, points, 8, listener, LoadTexture("Assets/road.png"));
			map->addCheckPoint(new Checkpoint(app, 500, 100, 50, 200, 0, listener, 1));
			map->addCheckPoint(new Checkpoint(app, 1000, 350, 50, 200, 90, listener, 2));
			map->addCheckPoint(new Checkpoint(app, 500, 600, 50, 200, 0, listener, 3));
			map->addFinishLine(new Finishline(app, 50, 350, 50, 200, 90, listener));
			map->addStartPosition(Transform2D(30, 500, 0));
			map->addStartPosition(Transform2D(100, 500, 0));
			map->addStartPosition(Transform2D(30, 700, 0));
			map->addStartPosition(Transform2D(100, 700, 0));
			//map->playerStartPositions ...
			//map->obstacles ...
			return map;
		}
		else if (mapNumber == 2) { }
		else if (mapNumber == 3) { }
	}
};