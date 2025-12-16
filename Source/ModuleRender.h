#pragma once
#include "Module.h"
#include "Globals.h"
#include "PhysicEntity.h"

#include <functional>
#include <iostream>
#include <vector>
#include <limits.h>

class ModuleRender : public Module
{
public:
	ModuleRender(Application* app, bool start_enabled = true);
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	void SetCameraTarget(PhysicEntity* target);
	void AdjustCamera();
    void SetBackgroundColor(Color color);
	bool Draw(Texture2D texture, int x, int y, const Rectangle* section = NULL, double angle = 0, int pivot_x = 0, int pivot_y = 0) const;
	bool rDrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color color) const;
	void DrawTextureUI(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color color);
	void rDrawText(const char* text, int x, int y, Font font, int spacing, Color tint);
	void rDrawTextCentered(const char* text, int centerX, int centerY, Font font, int spacing, Color tint);
	bool DrawText(const char* text, int x, int y, Font font, int spacing, Color tint) const;
	bool DrawTextCentered(const char* text, int centerX, int centerY, Font font, int spacing, Color tint) const;
	void DrawCircleUI(int x, int y, float radius, Color color);
	bool rDrawCircle(int x, int y, float radius, Color color) const;
	void DrawLineUI(int x1, int y1, int x2, int y2, Color color);
	bool rDrawLine(int x1, int y1, int x2, int y2, Color color) const;

public: 
	Camera2D camera = { 0 };
	bool cameraRotationActive = false;
	Texture2D whiteTexture; 

private:

	Color background;
	//Rectangle camera;
	PhysicEntity* cameraTarget;
	float cameraRotation = 0.0f;
	std::vector<std::function<void()>> uiCalls;
};