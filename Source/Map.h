#pragma once

#include "Application.h"
#include "Chain.h"
#include "Checkpoint.h"
#include "Finishline.h"
#include "PhysicEntity.h"
#include "ModulePhysics.h"

class Map : public Chain
{
public:
	Map(Application* app, int _x, int _y, float _angle, int* _points, unsigned int _size, Module* _listener, Texture2D _texture)
		: Chain(app->physics, _x, _y, _points, _size, _listener, _texture, CIRCUIT, _angle, false)
	{

	}

	void Update(float dt) override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
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

	void AddStartPosition(Vector2 p) {
		playerStartPositions.push_back(p);
	}

	void AddObstacle(PhysicEntity* o) {
		obstacles.push_back(o);
	}
	
	void AddCheckPoint(Checkpoint* c) {
		checkpoints.push_back(c);
	}

	void AddFinishLine(Finishline* f) {
		finishline = f;
		f->requiredCheckpoint = getLastCheckpointOrder();
	}

	~Map() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}

	std::vector<Vector2> playerStartPositions;
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
			int points[8] = {
				12, 65,
				847, 65,
				846, 775,
				11, 775
			};
			return new Map(app, 0, 0, 0, points, 8, listener, LoadTexture("Assets/road.png"));
		}
	}
};