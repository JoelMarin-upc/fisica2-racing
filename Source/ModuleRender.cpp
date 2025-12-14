#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include <math.h>

ModuleRender::ModuleRender(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    background = RAYWHITE;
}

// Destructor
ModuleRender::~ModuleRender()
{}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");
	bool ret = true;

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRender::PreUpdate()
{
    ClearBackground(background);

    AdjustCamera();

    // NOTE: This function setups render batching system for
    // maximum performance, all consecutive Draw() calls are
    // not processed until EndDrawing() is called
    BeginDrawing();
    if (cameraTarget) BeginMode2D(camera);

	return UPDATE_CONTINUE;
}

// Update: debug camera
update_status ModuleRender::Update()
{
    

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRender::PostUpdate()
{
    // Draw everything in our batch!

    if (cameraTarget) EndMode2D();

    for (auto& f : uiCalls) f();
    uiCalls.clear();

    EndDrawing();

    DrawFPS(10, 10);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	return true;
}

void ModuleRender::SetCameraTarget(PhysicEntity* target)
{
    cameraTarget = target;
    camera = { 0 };
    int x, y;
    cameraTarget->body->GetPhysicPosition(x, y);
    Vector2 pos = Vector2();
    pos.x = x;
    pos.y = y;
    camera.target = pos;
    Vector2 off = Vector2();
    off.x = GetScreenWidth() / 2.f;
    off.y = GetScreenHeight() / 2.f;
    camera.offset = off;
    camera.rotation = 0.f;
    camera.zoom = 1.f;
}

static float LerpAngle(float from, float to, float t)
{
    float delta = fmodf(to - from + 180.0f, 360.0f) - 180.0f;
    return from + delta * t;
}

void ModuleRender::AdjustCamera()
{
    if (!cameraTarget) return;
    int x, y;
    cameraTarget->body->GetPhysicPosition(x, y);
    camera.target = { (float)x, (float)y };
    if (cameraRotationActive) {
        float targetRot = cameraTarget->body->GetRotation() * RAD2DEG;

        float smooth = 0.05f;

        cameraRotation = LerpAngle(cameraRotation, targetRot, smooth);
        camera.rotation = -cameraRotation;
    }
    else {
        camera.rotation = 0.f;
    }
}

void ModuleRender::SetBackgroundColor(Color color)
{
	background = color;
}

// Draw to screen
bool ModuleRender::Draw(Texture2D texture, int x, int y, const Rectangle* section, double angle, int pivot_x, int pivot_y) const
{
	bool ret = true;

	float scale = 1.0f;
    Vector2 position = { (float)x, (float)y };
    Rectangle rect = { 0.f, 0.f, (float)texture.width, (float)texture.height };

    if (section != NULL) rect = *section;

    /*position.x = (float)(x-pivot_x) * scale;
    position.y = (float)(y-pivot_y) * scale;*/

	/*rect.width *= scale;
	rect.height *= scale;*/
    Vector2 worldPos = { (float)x - pivot_x, (float)y - pivot_y };

    DrawTextureRec(texture, rect, worldPos, WHITE);

	return ret;
}

bool ModuleRender::rDrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color color) const
{
    bool ret = true;

    DrawTexturePro(texture, source, dest, origin, rotation, color);

    return ret;
}

void ModuleRender::rDrawText(const char * text, int x, int y, Font font, int spacing, Color tint)
{
    std::string textCopy = text;
    uiCalls.push_back([=]() { DrawText(textCopy.c_str(), x, y, font, spacing, tint); });
}

void ModuleRender::rDrawTextCentered(const char* text, int centerX, int centerY, Font font, int spacing, Color tint)
{
    std::string textCopy = text;
    uiCalls.push_back([=]() { DrawTextCentered(textCopy.c_str(), centerX, centerY, font, spacing, tint); });
}

bool ModuleRender::DrawText(const char* text, int x, int y, Font font, int spacing, Color tint) const
{
    bool ret = true;

    Vector2 position = { (float)x, (float)y };

    DrawTextEx(font, text, position, (float)font.baseSize, (float)spacing, tint);

    return ret;
}

bool ModuleRender::DrawTextCentered(const char* text, int centerX, int centerY, Font font, int spacing, Color tint) const
{
    bool ret = true;

    Vector2 textSize = MeasureTextEx(font, text, (float)font.baseSize, (float)spacing);

    Vector2 pos = { (float)centerX - textSize.x * 0.5f, (float)centerY - textSize.y * 0.5f };

    DrawTextEx(font, text, pos, (float)font.baseSize, (float)spacing, tint);

    return ret;
}

bool ModuleRender::rDrawCircle(int x, int y, float radius, Color color) const
{
    bool ret = true;
    DrawCircle(x, y, radius, color);
    return ret;
}

bool ModuleRender::rDrawLine(int x1, int y1, int x2, int y2, Color color) const
{
    bool ret = true;
    DrawLine(x1, y1, x2, y2, color);
    return ret;
}
