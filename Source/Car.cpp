#include "ModuleGame.h"
#include "ModuleRender.h"

Car::Car(Application* app, int _x, int _y, float angle, Module* _listener, Texture2D _texture, int carNum, bool isHuman)
	: Box(app->physics, app->renderer, _x, _y, _listener, _texture, CAR, angle),
	carNum(carNum), isHumanControlled(isHuman), currentPosition(carNum), targetDirection(new Vector2{ 0, 0 }), currentLap(0), nitroInput(false), nitroActive(false), active(false), game(app->scene_intro), audio(app->audio), currentCheckpointNum(0)
{
	availableNitros = maxAvailableNitros;
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
	/*int x, y;
	body->GetPhysicPosition(x, y);
	int targetX, targetY;
	Map* map = game->map;
	if (currentCheckpointNum >= map->getLastCheckpointOrder()) map->finishline->body->GetPhysicPosition(targetX, targetY);
	else map->getCheckPoint(currentCheckpointNum + 1)->body->GetPhysicPosition(targetX, targetY);
	
	targetDirection = new Vector2();
	targetDirection->x = 0;
	targetDirection->y = 0;
	if (targetX > x) targetDirection->x = 1;
	if (targetX < x) targetDirection->x = -1;
	if (targetY > y) targetDirection->y = 1;
	if (targetY < y) targetDirection->y = -1;*/

	int x, y;
	body->GetPhysicPosition(x, y);

	int targetX, targetY;
	Map* map = game->map;

	// Get the target checkpoint's position
	if (currentCheckpointNum >= map->getLastCheckpointOrder()) {
		auto finishline = map->finishline->body;
		finishline->GetPhysicPosition(targetX, targetY);
		targetX -= finishline->width / 2;
		targetY -= finishline->height / 2;
	}
	else {
		auto checkpoint = map->getCheckPoint(currentCheckpointNum + 1)->body;
		checkpoint->GetPhysicPosition(targetX, targetY);
		targetX -= checkpoint->width / 2;
		targetY -= checkpoint->height / 2;
	}

	// Calculate the difference in position
	int dx = targetX - x;
	int dy = targetY - y;

	// Calculate the angle to the target (in radians)
	float targetAngle = atan2(dy, dx);  // Angle to the target point

	// Round the target angle to the nearest cardinal direction
	const float halfPi = PI / 2.0f;

	// Determine the primary direction
	int directionX = 0, directionY = 0;

	// Calculate X direction (left or right)
	if (targetAngle > -halfPi && targetAngle < halfPi) {
		// Target is generally to the right
		directionX = 1;
	}
	else if (targetAngle > halfPi && targetAngle < 3 * halfPi) {
		// Target is generally to the left
		directionX = -1;
	}

	// Calculate Y direction (up or down)
	if (targetAngle > -PI && targetAngle < 0) {
		// Target is generally down (move backward)
		directionY = -1;
	}
	else if (targetAngle > 0 && targetAngle < PI) {
		// Target is generally up (move forward)
		directionY = 1;
	}

	// Create the targetDirection as unitary vector with values -1, 0, or 1
	targetDirection = new Vector2();
	targetDirection->x = directionX;
	targetDirection->y = directionY;
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
	force *= speedScale;
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

void Car::SetSpeedScale(float scale)
{
	speedScale = scale;
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
	//render->DrawText(TextFormat("V=%f", body->GetLinearVelocity().Length()), GetScreenWidth() / 2, GetScreenHeight() / 2, {20}, 5, {255, 0, 0, 255});

	int x, y;
	body->GetPhysicPosition(x, y);
	render->rDrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
		Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
		Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
}

void Car::OnCollision(PhysicEntity* other)
{
	if (other->type == CHECKPOINT) {
		currentCheckpointNum = dynamic_cast<Checkpoint*>(other)->order;
	}
	if (other->type == FINISHLINE) {
		if (currentLap != 0 && dynamic_cast<Finishline*>(other)->requiredCheckpoint != currentCheckpointNum) return;
		if (currentLap + 1 > game->totalLaps) game->EndRace();
		game->MarkLap(currentLap);
		currentLap++;
		currentCheckpointNum = 0;
	}
	if (other->type == SLOWZONE) {
		double scale = dynamic_cast<SlowZone*>(other)->slowScale;
		SetSpeedScale(scale);
	}
}

void Car::OnCollisionEnd(PhysicEntity* other)
{
	if (other->type == SLOWZONE) {
		SetSpeedScale();
	}
}
