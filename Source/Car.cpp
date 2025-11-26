#include "Application.h"
#include "ModuleAudio.h"
#include "Box.cpp"

class Car : public Box
{
public:
	Car(Application* app, int _x, int _y, float angle, Module* _listener, Texture2D _texture, int carNum, bool isHuman)
		: Box(app->physics, _x, _y, _listener, _texture, CAR, angle),
		carNum(carNum), isHumanControlled(isHuman), currentPosition(carNum), targetDirection(new Vector2{ 0, 0 }), currentLap(0), nitro(false), active(false), game(app->scene_intro), audio(app->audio)
	{
		body->SetLinearDamping(1.0f);
		body->SetAngularDamping(2.0f);
	}

	~Car() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}

	void GetInput() {
		nitroInput = game->nitroInput;
		targetDirection = game->movementInput;
	}

	void AI() {
		// find direction with AI
		// targetDirection = direction from AI;
		// nitro ?
	}

	void GetTargetDirection() {
		if (isHumanControlled) GetInput();
		else AI();
	}

	void Move(float dt) {
		float maxSteerAngle = 3.0f;
		float speedFactor = body->GetLinearVelocity().Length() / engineForce;
		speedFactor = std::min(speedFactor, 1.0f);

		float targetAngularVelocity = targetDirection->x * maxSteerAngle * speedFactor;

		float currentW = body->GetAngularVelocity();

		float newW = currentW + (targetAngularVelocity - currentW) * (dt * steerStrength);
		body->SetAngularVelocity(newW);

		b2Vec2 right = body->GetWorldVector(b2Vec2(1, 0));
		float lateralVel = b2Dot(right, body->GetLinearVelocity());

		float sideFriction = 8.0f;
		b2Vec2 killLateral = -lateralVel * sideFriction * right;

		body->ApplyForce(killLateral.x, killLateral.y);

		b2Vec2 forward = body->GetWorldVector(b2Vec2(0, targetDirection->y));
		auto force = engineForce * forward;
		if (nitroActive) force *= nitroMultiplier;
		body->ApplyForce(force.x, force.y);
	}

	void CheckNitro()
	{
		if (nitroInput && availableNitros > 0 && !nitroActive) {
			nitroActive = true;
			availableNitros--;
			nitroTimer.Start();
		}
		if (nitroActive && nitroTimer.ReadSec() > nitroTime) {
			nitroActive = false;
		}
	}

	void Enable() {
		active = true;
	}

	void Disable() {
		active = false;
	}

	void Update(float dt) override
	{
		if (active)
		{
			GetTargetDirection();
			Move(dt);
		}

		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
	}

	const float engineForce = 10.f;
	const float steerStrength = 12.f;
	const float nitroMultiplier = 2.f;
	
	Vector2* targetDirection;
	bool active;
	bool isHumanControlled;
	int carNum;
	int currentPosition;
	int currentLap;
	int maxCheckpointNum;

	ModuleGame* game;
	ModuleAudio* audio;

	Timer nitroTimer;
	const double nitroTime = 2.f;
	const int maxAvailableNitros = 3;
	int availableNitros = maxAvailableNitros;
	bool nitroActive;
	bool nitroInput;

};