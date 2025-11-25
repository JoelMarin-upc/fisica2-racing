#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include <iostream> 
#include <string> 

class PhysicEntity
{
protected:

	PhysicEntity(PhysBody* _body, ModulePhysics* _physics, ModuleGame* _game, Module* _listener, EntityType type)
		: body(_body)
		, physics(_physics)
		, game(_game)
		, listener(_listener)
		, type(type)
	{
		body->listener = listener;
	}

public:
	virtual ~PhysicEntity() = default;
	virtual void Update(float dt) = 0;

	virtual int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
	{
		return 0;
	}

	PhysBody* body;
	Module* listener;
	EntityType type;
	ModulePhysics* physics;
	ModuleGame* game;
};

class Circle : public PhysicEntity
{
public:
	Circle(ModulePhysics* physics, ModuleGame* game, int _x, int _y, Module* _listener, Texture2D _texture, EntityType type, float angle = 0.f, bool dynamic = true, float restitution = 0.f)
		: PhysicEntity(physics->CreateCircle(_x, _y, _texture.height / 2, angle, dynamic, restitution), physics, game, _listener, type)
		, texture(_texture)
	{
		body->type = type;
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
		DrawTexturePro(texture, source, dest, origin, rotation, WHITE);
	}

	Texture2D texture;

};

class Box : public PhysicEntity
{
public:
	Box(ModulePhysics* physics, ModuleGame* game, int _x, int _y, Module* _listener, Texture2D _texture, EntityType type, float angle = 0.f, bool dynamic = true, float restitution = 0.f)
		: PhysicEntity(physics->CreateRectangle(_x, _y, _texture.width, _texture.height, angle, dynamic, restitution), physics, game, _listener, type)
		, texture(_texture)
	{
		body->type = type;
	}

	void Update(float dt) override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x, (float)y, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

	Texture2D texture;

};

class Chain : public PhysicEntity
{
public:
	Chain(ModulePhysics* physics, ModuleGame* game, int _x, int _y, int* points, int size, Module* _listener, Texture2D _texture, EntityType type, float angle = 0.f, bool dynamic = true, float restitution = 0.f, bool reverse = false)
		: PhysicEntity(physics->CreateChain(_x - _texture.width / 2, _y - _texture.height / 2, points, size, angle, dynamic, restitution, reverse), physics, game, _listener, type)
		, texture(_texture)
	{
		body->type = type;
	}

	void Update(float dt) override
	{
		int x, y;
		body->GetPhysicPosition(x, y);
		DrawTexturePro(texture, Rectangle{ 0, 0, (float)texture.width, (float)texture.height },
			Rectangle{ (float)x + texture.width / 2, (float)y + texture.height / 2, (float)texture.width, (float)texture.height },
			Vector2{ (float)texture.width / 2.0f, (float)texture.height / 2.0f }, body->GetRotation() * RAD2DEG, WHITE);
	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

	Texture2D texture;

};

class BoxSensor : public PhysicEntity
{
public:
	BoxSensor(ModulePhysics* physics, ModuleGame* game, int _x, int _y, int width, int height, Module* _listener, EntityType type, float angle = 0.f, bool dynamic = true)
		: PhysicEntity(physics->CreateRectangleSensor(_x, _y, width, height, angle, dynamic), physics, game, _listener, type)
	{
		body->type = type;
	}

	void Update(float dt) override
	{

	}

	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override
	{
		return body->RayCast(ray.x, ray.y, mouse.x, mouse.y, normal.x, normal.y);;
	}

};

class Car : public Box
{
public:
	Car(ModulePhysics* physics, ModuleGame* game, int _x, int _y, float angle, Module* _listener, Texture2D _texture, int carNum, bool isHuman)
		: Box(physics, game, _x, _y, _listener, _texture, CAR, angle), 
		carNum(carNum), isHumanControlled(isHuman), currentPosition(carNum), targetDirection(new Vector2{ 0, 0 }), currentLap(0), nitro(false), active(false)
	{
		body->SetLinearDamping(1.0f);
		body->SetAngularDamping(2.0f);
	}

	~Car() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}

	void GetInput() {
		nitro = game->nitroActive;
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
		body->ApplyForce(force.x, force.y);
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

	Vector2* targetDirection;
	bool active;
	bool nitro;
	bool isHumanControlled;
	int carNum;
	int currentPosition;
	int currentLap;
	const float engineForce = 10.f;
	const float steerStrength = 12.f;

};

class Checkpoint : public BoxSensor
{
public:
	Checkpoint(ModulePhysics* physics, ModuleGame* game, int _x, int _y, int width, int height, float angle, Module* _listener, Texture2D _texture, int carNum)
		: BoxSensor(physics, game, _x, _y, width, height, _listener, CHECKPOINT, angle, false)
	{

	}

	~Checkpoint() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}
};

class Finishline : public BoxSensor
{
public:
	Finishline(ModulePhysics* physics, ModuleGame* game, int _x, int _y, int width, int height, float angle, Module* _listener, Texture2D _texture, int carNum)
		: BoxSensor(physics, game, _x, _y, width, height, _listener, FINISHLINE, angle, false)
	{

	}

	~Finishline() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}
};

ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

ModuleGame::~ModuleGame()
{}

// Load assets
bool ModuleGame::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	CreateMap();
	AddCars();

	return ret;
}

// Load assets
bool ModuleGame::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

void ModuleGame::CreateMap()
{
	// add circuit, checkpoints and finishline
}

void ModuleGame::AddCars()
{
	// add cars
	carTex = LoadTexture("Assets/car1.png");
	car = new Car(App->physics, this, 300, 300, 0, this, carTex, 1, true);
	cars.push_back(car);
	entities.push_back(new Circle(App->physics, this, 600, 600, this, carTex, EntityType::CAR, 0, true, 0));
}

void ModuleGame::PerformCountdown()
{
	if (raceActive) return;
	if (!countdownStarted)
	{
		countdownTimer.Start();
		countdownStarted = true;
	}
	int currentNumber = 3 - floor(countdownTimer.ReadSec());
	std::string countdownText = "";
	if (currentNumber == 0) countdownText = "Start";
	else if (currentNumber == -1) StartRace();
	else countdownText = std::to_string(currentNumber);
	App->renderer->DrawText(countdownText.c_str(), GetScreenWidth() / 2, GetScreenHeight() / 2, { 20 }, 5, { 255, 0, 0, 255 });
}

void ModuleGame::StartRace()
{
	raceActive = true;
	for (Car* c : cars) c->Enable();
}

void ModuleGame::EndRace()
{
	raceActive = false;
	for (Car* c : cars) c->Disable();
}

void ModuleGame::GetInput()
{
	movementInput = new Vector2();
	nitroInput = false;
	if (IsKeyDown(KEY_RIGHT)) movementInput->x = 1;
	if (IsKeyDown(KEY_LEFT)) movementInput->x = -1;
	if (IsKeyDown(KEY_UP)) movementInput->y = -1;
	if (IsKeyDown(KEY_DOWN)) movementInput->y = 1;
	if (IsKeyDown(KEY_SPACE)) nitroInput = true;
}

void ModuleGame::PerformNitro()
{
	if (nitroInput && availableNitros > 0 && !nitroActive) {
		nitroActive = true;
		nitroTimer.Start();
	}
	if (nitroActive && nitroTimer.ReadSec() > maxNitroTime) {
		nitroActive = false;
	}
}

void ModuleGame::AdjustCamera()
{
	// adjust position and rotation of the camera to have the players car in the center
	// App->renderer->camera
}

// Update: draw background
update_status ModuleGame::Update(float dt)
{
	PerformCountdown();
	if (raceActive) {
		GetInput();
		PerformNitro();
	}

	for (Car* c : cars) c->Update(dt);

	for (PhysicEntity* entity : entities) entity->Update(dt);

	App->physics->Step(dt);

	return UPDATE_CONTINUE;
}
