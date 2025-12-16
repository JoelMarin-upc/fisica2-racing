#include "ModuleGame.h"
#include "ModuleRender.h"

Car::Car(Application* app, int _x, int _y, float angle, Module* _listener, Texture2D _texture, int carNum, bool isHuman, int _sprintFXId, int _runFXId, int _crashFXId, int _carCrashFX, std::string animationsPath, Vector2 colliderSize, int _difficulty)
	: Box(app->physics, app->renderer, _x, _y, _listener, _texture, CAR, angle, true, 0.f, colliderSize),
	carNum(carNum), isHumanControlled(isHuman), currentPosition(carNum), targetDirection(new Vector2{ 0, 0 }), currentLap(0), nitroInput(false), nitroActive(false), active(false), game(app->scene_intro), audio(app->audio), currentCheckpointNum(0), difficulty(_difficulty)
{
	availableNitros = maxAvailableNitros;
	body->SetLinearDamping(1.0f);
	body->SetAngularDamping(2.0f);

	//sounds
    sprintFX = _sprintFXId;
    runFX = _runFXId;
    crashFX = _crashFXId;
    carCrashFX = _carCrashFX;

    runFXTimer = Timer();

    std::unordered_map<int, std::string> aliases = { {0,"move"} };
    anims.LoadFromTSX(animationsPath.c_str(), aliases);
    anims.SetCurrent("move");
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
    int x, y;
    body->GetPhysicPosition(x, y);

    int targetX, targetY;
    Map* map = game->map;

    if (currentCheckpointNum >= map->getLastCheckpointOrder()) {
        auto finishline = map->finishline->body;
        finishline->GetPhysicPosition(targetX, targetY);
    }
    else {
        auto checkpoint = map->getCheckPoint(currentCheckpointNum + 1)->body;
        checkpoint->GetPhysicPosition(targetX, targetY);
    }

    b2Vec2 right = body->GetWorldVector(b2Vec2(1, 0));
    b2Vec2 left = body->GetWorldVector(b2Vec2(-1, 0));
    b2Vec2 forward = body->GetWorldVector(b2Vec2(0, 1));
    b2Vec2 back = body->GetWorldVector(b2Vec2(0, -1));

    b2Vec2 rightPix = { (float)METERS_TO_PIXELS(right.x),   (float)METERS_TO_PIXELS(right.y) };
    b2Vec2 leftPix = { (float)METERS_TO_PIXELS(left.x),    (float)METERS_TO_PIXELS(left.y) };
    b2Vec2 frontPix = { (float)METERS_TO_PIXELS(back.x), (float)METERS_TO_PIXELS(back.y) };

    int rx1 = x;
    int ry1 = y;
    int rx2 = x + rightPix.x * 5;
    int ry2 = y + rightPix.y * 5;

    int lx1 = x;
    int ly1 = y;
    int lx2 = x + leftPix.x * 5;
    int ly2 = y + leftPix.y * 5;

    int fx1 = x;
    int fy1 = y;
    int fx2 = x + frontPix.x * 4;
    int fy2 = y + frontPix.y * 4;

    float normalx, normaly;

    int distRight = body->RayCast(rx1, ry1, rx2, ry2, normalx, normaly);
    int distLeft = body->RayCast(lx1, ly1, lx2, ly2, normalx, normaly);
    int distFront = body->RayCast(fx1, fy1, fx2, fy2, normalx, normaly);

    // following checkpoints

    float dx = targetX - x;
    float dy = targetY - y;
    b2Vec2 toTarget(dx, dy);

    float forwardDot = b2Dot(toTarget, forward);
    float rightDot = b2Dot(toTarget, right);

    const float eps = 0.2f;

    int directionY = (forwardDot > eps) ? 1 :
        (forwardDot < -eps) ? -1 : 0;

    int directionX = (rightDot > eps) ? 1 :
        (rightDot < -eps) ? -1 : 0;

    // avoiding obstacles

    /*float obstacleSteerX = 0.0f;

    if (distLeft != -1)
        obstacleSteerX += 1.0f;

    if (distRight != -1)
        obstacleSteerX -= 1.0f;

    if (distFront != -1)
    {
        if (distLeft != -1 && distRight == -1)
            obstacleSteerX -= 1.0f;
        else if (distRight != -1 && distLeft == -1)
            obstacleSteerX += 1.0f;
        else
            obstacleSteerX += 1.0f;
    }*/

    // combine directions

    float finalSteerX = directionX;// +obstacleSteerX;
    float finalSteerY = directionY;

    float mag = sqrt(finalSteerX * finalSteerX + finalSteerY * finalSteerY);
    if (mag > 0.1f) {
        finalSteerX /= mag;
        finalSteerY /= mag;
    }

    targetDirection = new Vector2();
    targetDirection->x = (finalSteerX > 0.3f) ? 1 :
        (finalSteerX < -0.3f) ? -1 : 0;

    targetDirection->y = (finalSteerY > 0.3f) ? 1 :
        (finalSteerY < -0.3f) ? -1 : 0;
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

    float speedScaleVal = speedScale;
    float steerStrengthVal = steerStrength;

    if (!isHumanControlled) {
        if (difficulty == 1) {
            speedScaleVal *= 1.f;
            maxSteerAngle *= 1.5f;
            steerStrengthVal *= 1.5f;
        }
        else if (difficulty == 2) {
            speedScaleVal *= 1.075f;
            maxSteerAngle *= 2.2f;
            steerStrengthVal *= 2.2f;
        }
        else if (difficulty == 3) {
            speedScaleVal *= 1.15f;
            maxSteerAngle *= 3.f;
            steerStrengthVal *= 3.f;
        }
    }

	float targetAngularVelocity = targetDirection->x * maxSteerAngle * speedFactor;

	float currentW = body->GetAngularVelocity();

	float newW = currentW + (targetAngularVelocity - currentW) * (dt * steerStrengthVal);
	body->SetAngularVelocity(newW);

	b2Vec2 right = body->GetWorldVector(b2Vec2(1, 0));
	float lateralVel = b2Dot(right, body->GetLinearVelocity());

	float sideFriction = 8.0f;
	b2Vec2 killLateral = -lateralVel * sideFriction * right;

	body->ApplyForce(killLateral.x, killLateral.y);

	b2Vec2 forward = body->GetWorldVector(b2Vec2(0, targetDirection->y));
	auto force = engineForce * forward;
	if (nitroActive) force *= nitroMultiplier;
	force *= speedScaleVal;
	body->ApplyForce(force.x, force.y);
}

void Car::PlayRunAudio()
{
    if (std::abs(targetDirection->y) < 1) return;
    if (runFXTimer.ReadSec() > runFXSeconds) {
        audio->PlayFx(runFX);
        runFXTimer.Start();
    }
}

void Car::CheckNitro()
{


	if (nitroInput && availableNitros > 0 && !nitroActive)
	{
		audio->PlayFx(sprintFX);

		nitroActive = true;
		availableNitros--;
		nitroTimer.Start();
	}
	if (nitroActive && nitroTimer.ReadSec() > nitroTime) 
	{
		nitroActive = false;
	}
}

void Car::SetSpeedScale(float scale)
{
	speedScale = scale;
}

void Car::StartBoost(double scale, double time)
{
    SetSpeedScale(scale);
    boostTime = time;
    boostTimer.Start();
}

void Car::RunBoostTimer()
{
    if (boostTime < 0.f) return;
    boostTime -= boostTimer.ReadSec();
    if (boostTime < 0.f) {
        SetSpeedScale();
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
        if (isHumanControlled) PlayRunAudio();
        RunBoostTimer();
	}

	int x, y;
    body->GetPhysicPosition(x, y);
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
    /*render->rDrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
        Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
        Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);*/
}

void Car::OnCollision(PhysicEntity* other, bool isSensor)
{
	if (other->type == CHECKPOINT) {
		currentCheckpointNum = dynamic_cast<Checkpoint*>(other)->order;
	}
	if (other->type == FINISHLINE) {
		if (currentLap != 0 && dynamic_cast<Finishline*>(other)->requiredCheckpoint != currentCheckpointNum) return;
		if (currentLap + 1 > game->totalLaps && isHumanControlled) game->EndRace();
		if (isHumanControlled) game->MarkLap(currentLap);
		currentLap++;
		currentCheckpointNum = 0;
	}
	if (other->type == SLOWZONE) {
		double scale = dynamic_cast<SlowZone*>(other)->slowScale;
        if (boostTime < 0.f) SetSpeedScale(scale);
        else SetSpeedScale();
	}
    if (other->type == BOOSTER) {
        auto booster = dynamic_cast<Booster*>(other);
        double scale = booster->boostScale;
        double time = booster->boostTime;
        StartBoost(scale, time);
    }
    if ((other->type == CIRCUIT || other->type == OBSTACLE) && !isSensor) {
        if (isHumanControlled) audio->PlayFx(crashFX);
    }
    if ((other->type == CAR) && !isSensor) {
        if(isHumanControlled) audio->PlayFx(carCrashFX);
    }
}

void Car::OnCollisionEnd(PhysicEntity* other, bool isSensor)
{
	if (other->type == SLOWZONE) {
        if (boostTime < 0.f) SetSpeedScale();
	}
}
