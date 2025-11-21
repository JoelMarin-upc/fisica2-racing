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
	virtual void Update() = 0;

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

	void Update() override
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

	void Update() override
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

	void Update() override
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

	void Update() override
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
		: Box(physics, game, _x, _y, _listener, _texture, CAR, angle), carNum(carNum), isHumanControlled(isHuman), currrentPosition(carNum), currentLap(0), nitro(false), active(false)
	{
		
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

	void Move() {
		// use target direction to move car
	}

	void Enable() {
		active = true;
	}

	void Disable() {
		active = false;
	}

	void Update() override
	{
		if (active)
		{
			GetTargetDirection();
			Move();
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
	int currrentPosition;
	int currentLap;

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
	if (currentNumber == 0)
	{
		StartRace();
		countdownText = "Start";
	}
	else countdownText = std::to_string(currentNumber);
	App->renderer->DrawText(countdownText.c_str(), GetScreenWidth() / 2, GetScreenHeight() / 2, { 20 }, 5, { 255, 0, 0, 0 });
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
	if (IsKeyPressed(KEY_RIGHT)) movementInput->x = 1;
	if (IsKeyPressed(KEY_LEFT)) movementInput->x = -1;
	if (IsKeyPressed(KEY_UP)) movementInput->y = 1;
	if (IsKeyPressed(KEY_DOWN)) movementInput->y = -1;
	if (IsKeyPressed(KEY_SPACE)) nitroInput = true;
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
update_status ModuleGame::Update()
{
	PerformCountdown();
	if (raceActive) {
		GetInput();
		PerformNitro();
	}

	for (Car* c : cars) c->Update();

	for (PhysicEntity* entity : entities) entity->Update();

	return UPDATE_CONTINUE;
}
