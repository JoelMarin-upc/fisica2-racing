#include "Module.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"

PhysicEntity::PhysicEntity(PhysBody* _body, ModulePhysics* _physics, ModuleRender* _render, Module* _listener, EntityType type)
	: body(_body)
	, physics(_physics)
	, render(_render)
	, listener(_listener)
	, type(type)
{
	body->listener = listener;
}
