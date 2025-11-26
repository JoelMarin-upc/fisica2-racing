#include "Application.h"
#include "Chain.cpp"
#include "Checkpoint.cpp"
#include "Finishline.cpp"

class Map : public Chain
{
public:
	Map(Application* app, int _x, int _y, float angle, Module* _listener, Texture2D _texture)
		: Chain(app->physics, _x, _y, points, sizeof(points) / sizeof(points[0]), _listener, _texture, FINISHLINE, angle, false)
	{

	}

	void Update(float dt) override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
		
		for (PhysicEntity* obstacle : obstacles) obstacle->Update(dt);
	}

	~Map() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}

	std::vector<b2Vec2> playerStartPositions;
	std::vector<PhysicEntity*> obstacles;
	std::vector<Checkpoint*> checkpoints;
	Finishline* finishline;

	const int points[1] = { 1 };
};