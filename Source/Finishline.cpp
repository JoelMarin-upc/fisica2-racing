#include "Application.h"
#include "BoxSensor.cpp"

class Finishline : public BoxSensor
{
public:
	Finishline(Application* app, int _x, int _y, int width, int height, float angle, Module* _listener)
		: BoxSensor(app->physics, _x, _y, width, height, _listener, FINISHLINE, angle, false)
	{

	}

	~Finishline() {
		auto pbody = body->body;
		physics->DestroyBody(pbody);
	}
};