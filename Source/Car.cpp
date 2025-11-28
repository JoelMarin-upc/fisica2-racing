#include "ModuleGame.h"
#include "ModuleRender.h"

Car::Car(Application* app, int _x, int _y, float angle, Module* _listener, Texture2D _texture, int carNum, bool isHuman)
	: Box(app->physics, _x, _y, _listener, _texture, CAR, angle),
	carNum(carNum), isHumanControlled(isHuman), currentPosition(carNum), targetDirection(new Vector2{ 0, 0 }), currentLap(0), nitroInput(false), nitroActive(false), active(false), game(app->scene_intro), audio(app->audio), maxCheckpointNum(0)
{
	body->SetLinearDamping(1.0f);
	body->SetAngularDamping(2.0f);
}

Car::~Car()
{
	auto pbody = body->body;
	physics->DestroyBody(pbody);
}

void Car::GetInput()
{
	nitroInput = game->nitroInput;
	if (game->movementInput) targetDirection = game->movementInput;
}

void Car::AI()
{
	// find direction with AI
	// targetDirection = direction from AI;
	// nitro ?
}

void Car::GetTargetDirection()
{
	if (isHumanControlled) GetInput();
	else AI();
}

void Car::Move(float dt)
{
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

void Car::CheckNitro()
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

void Car::Enable()
{
	active = true;
}

void Car::Disable()
{
	active = false;
}

void Car::Update(float dt)
{
	if (active)
	{
		GetTargetDirection();
		CheckNitro();
		Move(dt);
	}
	if (nitroActive) game->App->renderer->DrawText("Nitro", GetScreenWidth() / 2, GetScreenHeight() / 2, {20}, 5, {255, 0, 0, 255});

	int x, y;
	body->GetPhysicPosition(x, y);
	DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
		Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
		Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
}

void Car::OnCollision(PhysicEntity* other)
{

}
