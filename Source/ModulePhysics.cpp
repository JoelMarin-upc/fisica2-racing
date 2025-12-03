#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"

#include "p2Point.h"

#include <math.h>



ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	ground = NULL;
	debug = false;
	gravityOn = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	// needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	return true;
}

update_status ModulePhysics::PreUpdate()
{
	//if (gravityOn) world->Step(1.f / GetFPS(), 8, 3);

	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if (c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			b2BodyUserData data1 = c->GetFixtureA()->GetBody()->GetUserData();
			b2BodyUserData data2 = c->GetFixtureA()->GetBody()->GetUserData();

			PhysBody* pb1 = (PhysBody*)data1.pointer;
			PhysBody* pb2 = (PhysBody*)data2.pointer;
			if (pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, float angle, bool dynamic, float restitution)
{
	b2BodyDef body;
	body.type = dynamic ? b2_dynamicBody : b2_staticBody;
	body.bullet = dynamic;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	PhysBody* pbody = new PhysBody();
	b2Body* b = world->CreateBody(&body);
	b->GetUserData().pointer = reinterpret_cast<uintptr_t>(pbody);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 5.0f;
	fixture.restitution = restitution;

	b->CreateFixture(&fixture);

	pbody->body = b;
	//body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);
	pbody->width = pbody->height = radius * 2;

	b2Transform transform = b->GetTransform();
	b->SetTransform(transform.p, DEGREES_TO_RADIANS(angle));

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, float angle, bool dynamic, float restitution)
{
	b2BodyDef body;
	body.type = dynamic ? b2_dynamicBody : b2_staticBody;
	body.bullet = dynamic;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	PhysBody* pbody = new PhysBody();
	b2Body* b = world->CreateBody(&body);
	b->GetUserData().pointer = reinterpret_cast<uintptr_t>(pbody);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.restitution = restitution;

	b->CreateFixture(&fixture);

	pbody->body = b;
	//body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);
	pbody->width = width;
	pbody->height = height;

	b2Transform transform = b->GetTransform();
	b->SetTransform(transform.p, DEGREES_TO_RADIANS(angle));

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height, float angle, bool dynamic)
{
	b2BodyDef body;
	body.type = dynamic ? b2_dynamicBody : b2_staticBody;
	body.bullet = dynamic;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	PhysBody* pbody = new PhysBody();
	b2Body* b = world->CreateBody(&body);
	b->GetUserData().pointer = reinterpret_cast<uintptr_t>(pbody);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	pbody->body = b;
	//body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);
	pbody->width = width;
	pbody->height = height;

	b2Transform transform = b->GetTransform();
	b->SetTransform(transform.p, DEGREES_TO_RADIANS(angle));

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, unsigned int size, float angle, bool dynamic, float restitution, bool reverse)
{
	b2BodyDef body;
	body.type = dynamic ? b2_dynamicBody : b2_staticBody;
	body.bullet = dynamic;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	PhysBody* pbody = new PhysBody();
	b2Body* b = world->CreateBody(&body);
	b->GetUserData().pointer = reinterpret_cast<uintptr_t>(pbody);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (int i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	if (reverse) std::reverse(p, p + size / 2);
	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.restitution = restitution;

	b->CreateFixture(&fixture);

	delete p;

	pbody->body = b;
	//body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);
	pbody->width = pbody->height = 0;

	b2Transform transform = b->GetTransform();
	b->SetTransform(transform.p, DEGREES_TO_RADIANS(angle));

	return pbody;
}

void ModulePhysics::Step(float dt)
{
	const int velocityIterations = 8;
	const int positionIterations = 3;

	if (gravityOn) world->Step(dt, velocityIterations, positionIterations);
}

b2MouseJoint* ModulePhysics::CreateMouseJoint(b2Body* body, b2Vec2 target)
{
	b2MouseJointDef mouseJointDef;

	mouseJointDef.bodyA = ground;
	mouseJointDef.bodyB = body;
	mouseJointDef.target = { PIXEL_TO_METERS(target.x), PIXEL_TO_METERS(target.y) };
	mouseJointDef.damping = 10.f;
	mouseJointDef.stiffness = 150.f;
	mouseJointDef.maxForce = 2000.0f * body->GetMass();
	return (b2MouseJoint*)world->CreateJoint(&mouseJointDef);
}

float ModulePhysics::GetDistance(b2Body* bodyA, b2Body* bodyB)
{
	return b2Distance(bodyA->GetPosition(), bodyB->GetPosition());
}

bool ModulePhysics::TestPoint(b2Body* body, b2Vec2 point)
{
	return body->GetFixtureList()->TestPoint({ PIXEL_TO_METERS(point.x), PIXEL_TO_METERS(point.y) });
}

void ModulePhysics::DestroyBody(b2Body* body)
{
	if (body) bodiesToDestroy.insert(body);
}

void ModulePhysics::DestroyJoint(b2Joint* joint) 
{
	if (joint) jointsToDestroy.insert(joint);
}

void ModulePhysics::CleanUpDestructionQueue()
{
	for (b2Body* body : bodiesToDestroy) {
		if (body) {
			auto pbody = (PhysBody*)body->GetUserData().pointer;
			if (pbody) delete pbody;
			body->GetUserData().pointer = 0;
			world->DestroyBody(body);
		}
	}
	for (b2Joint* joint : jointsToDestroy) {
		if (joint) {
			world->DestroyJoint(joint);
		}
	}
	bodiesToDestroy.clear();
	jointsToDestroy.clear();
}

void ModulePhysics::ToggleDebug(b2Body* car)
{
	debug = !debug;
}

void ModulePhysics::TogglePhysics()
{
	gravityOn = !gravityOn;
}

void ModulePhysics::ChangeGravity(bool add)
{
	if (gravityOn) world->SetGravity(world->GetGravity() + b2Vec2(0, add ? 1 : -1));
}

void ModulePhysics::ChangeRestitution(b2Body* body, bool add)
{
	float change = add ? .1f : -.1f;
	for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
		float value = f->GetRestitution();
		value += change;
		if (value > 1.f) value = 1;
		else if (value < 0) value = 0;
		f->SetRestitution(value);
	}
}

// 
update_status ModulePhysics::PostUpdate()
{
	CleanUpDestructionQueue();

	if (IsKeyPressed(KEY_F1))
	{
		debug = !debug;
	}

	if (!debug)
	{
		return UPDATE_CONTINUE;
	}

	if (debug)
	{
		for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
		{
			for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
			{
				switch (f->GetType())
				{
					// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();

					App->renderer->rDrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), (float)METERS_TO_PIXELS(shape->m_radius), RED);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->m_count;
					b2Vec2 prev, v;

					for (int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->m_vertices[i]);
						if (i > 0)
							App->renderer->rDrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->m_vertices[0]);
					App->renderer->rDrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for (int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if (i > 0)
							App->renderer->rDrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->renderer->rDrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->renderer->rDrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), BLUE);
				}
				break;
				}
			}
		}
	}

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	b2BodyUserData dataA = contact->GetFixtureA()->GetBody()->GetUserData();
	b2BodyUserData dataB = contact->GetFixtureB()->GetBody()->GetUserData();

	PhysBody* physA = (PhysBody*)dataA.pointer;
	PhysBody* physB = (PhysBody*)dataB.pointer;

	if (physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}

void ModulePhysics::EndContact(b2Contact* contact)
{
	b2BodyUserData dataA = contact->GetFixtureA()->GetBody()->GetUserData();
	b2BodyUserData dataB = contact->GetFixtureB()->GetBody()->GetUserData();

	PhysBody* physA = (PhysBody*)dataA.pointer;
	PhysBody* physB = (PhysBody*)dataB.pointer;

	if (physA && physA->listener != NULL)
		physA->listener->OnCollisionEnd(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->OnCollisionEnd(physB, physA);
}

void PhysBody::GetPhysicPosition(int& x, int& y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x);
	y = METERS_TO_PIXELS(pos.y);
}

float PhysBody::GetRotation() const
{
	return body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = (float)(x2 - x1);
			float fy = (float)(y2 - y1);
			float dist = sqrtf((fx * fx) + (fy * fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return (int)(output.fraction * dist);
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void PhysBody::ApplyImpulse(float xNewtonSec, float yNewtonSec)
{
	b2Vec2 impulse(xNewtonSec, yNewtonSec);
	body->ApplyLinearImpulseToCenter(impulse, true);
}

void PhysBody::ApplyForce(float xNewton, float yNewton)
{
	b2Vec2 force(xNewton, yNewton);
	body->ApplyForceToCenter(force, true);
}

b2Vec2 PhysBody::GetWorldVector(b2Vec2 direction)
{
	return body->GetWorldVector(direction);
}

void PhysBody::SetLinearDamping(float linearDamping)
{
	body->SetLinearDamping(linearDamping);
}

void PhysBody::SetAngularDamping(float angularDamping)
{
	body->SetAngularDamping(angularDamping);
}

void PhysBody::SetAngularVelocity(float velocity)
{
	body->SetAngularVelocity(velocity);
}

b2Vec2 PhysBody::GetLinearVelocity()
{
	return body->GetLinearVelocity();
}

float PhysBody::GetAngularVelocity()
{
	return body->GetAngularVelocity();
}
