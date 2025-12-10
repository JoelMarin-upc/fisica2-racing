#pragma once

#include "Module.h"
#include "Globals.h"

#include "box2d\box2d.h"

#include <vector>
#include <unordered_set>

#define GRAVITY_X 0.0f
#define GRAVITY_Y 0.0f
#define PI 3.14159265359f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)
#define DEGREES_TO_RADIANS(d) ((float) d * PI / 180.f)

enum EntityType {
	CAR,
	CHECKPOINT,
	FINISHLINE,
	SLOWZONE,
	OBSTACLE,
	CIRCUIT
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
	void ApplyForce(float xNewton, float yNewton);
	b2Vec2 GetWorldVector(b2Vec2 direction);
	void SetLinearDamping(float linearDamping);
	void SetAngularDamping(float angularDamping);
	void SetAngularVelocity(float velocity);
	b2Vec2 GetLinearVelocity();
	float GetAngularVelocity();

public:
	int width, height;
	b2Body* body = nullptr;
	Module* listener;
	EntityType type;
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
	PhysBody* CreateChain(int x, int y, int* points, unsigned int size, float angle = 0.f, bool dynamic = true, float restitution = 0.f, bool reverse = false, bool isSensor = false);
	b2MouseJoint* CreateMouseJoint(b2Body* body, b2Vec2 target);

	float GetDistance(b2Body* bodyA, b2Body* bodyB);
	bool TestPoint(b2Body* body, b2Vec2 point);

	void DestroyBody(b2Body* body);
	void DestroyJoint(b2Joint* joint);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);

	void CleanUpDestructionQueue();

	void ToggleDebug(b2Body* car = nullptr);
	void TogglePhysics();
	void ChangeGravity(bool add);
	void ChangeRestitution(b2Body* body, bool add);

private:

	bool debug;
	bool gravityOn;
	b2World* world;
	b2Body* ground;
	std::unordered_set<b2Body*> bodiesToDestroy;
	std::unordered_set<b2Joint*> jointsToDestroy;
};