#pragma once

#include "Module.h"
#include "Globals.h"

#include "box2d\box2d.h"

#include <vector>

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f
#define PI 3.14159265359f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)
#define DEGREES_TO_RADIANS(d) ((float) d * PI / 180.f)

enum EntityType {
	CAR,
	CHECKPOINT,
	FINISHLINE
};

class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{
	}

	//void GetPosition(int& x, int& y) const;
	void GetPhysicPosition(int& x, int& y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;
	void ApplyImpulse(float xNewtonSec, float yNewtonSec);

public:
	int width, height;
	b2Body* body;
	Module* listener;
	EntityType type;
	char letter = NULL;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener // TODO
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	void Step(float dt);

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	PhysBody* CreateCircle(int x, int y, int radius, float angle = 0.f, bool dynamic = true, float restitution = 0.f);
	PhysBody* CreateRectangle(int x, int y, int width, int height, float angle = 0.f, bool dynamic = true, float restitution = 0.f);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, float angle = 0.f, bool dynamic = true);
	PhysBody* CreateChain(int x, int y, const int* points, int size, float angle = 0.f, bool dynamic = true, float restitution = 0.f, bool reverse = false);
	void CreateRevoluteJoint(b2Body* b1, b2Body* b2, int xAnchor, int yAnchor, float lowerAngle, float upperAngle);
	void CreateMouseJoint(b2Body* body, b2Vec2 target);

	void DestroyBody(b2Body* body);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

	void CleanUpDestructionQueue();

	void ToggleDebug(b2Body* car = nullptr);
	void TogglePhysics();
	void ChangeGravity(bool add);
	void ChangeRestitution(b2Body* body, bool add);

	b2Vec2 mousePos;

private:

	bool debug;
	bool gravityOn;
	b2World* world;
	b2Body* ground;
	b2MouseJoint* mouseJoint;
	std::vector<b2Body*> bodiesToDestroy;
};