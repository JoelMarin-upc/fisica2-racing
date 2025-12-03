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
	return UPDATE_CONTINUE;
}

// Update: debug camera
update_status ModuleRender::Update()
{
    ClearBackground(background);

    // NOTE: This function setups render batching system for
    // maximum performance, all consecutive Draw() calls are
    // not processed until EndDrawing() is called
    BeginDrawing();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRender::PostUpdate()
{
    // Draw everything in our batch!
    DrawFPS(10, 10);

    EndDrawing();

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	return true;
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

    position.x = (float)(x-pivot_x) * scale + camera.x;
    position.y = (float)(y-pivot_y) * scale + camera.y;

	rect.width *= scale;
	rect.height *= scale;

    DrawTextureRec(texture, rect, position, WHITE);

	return ret;
}

bool ModuleRender::rDrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color color) const
{
    bool ret = true;

    dest.x += camera.x;
    dest.y += camera.y;
    DrawTexturePro(texture, source, dest, origin, rotation, color);

    return ret;
}

bool ModuleRender::DrawText(const char * text, int x, int y, Font font, int spacing, Color tint) const
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
    DrawCircle(x + camera.x, y + camera.y, radius, color);
    return ret;
}

bool ModuleRender::rDrawLine(int x1, int y1, int x2, int y2, Color color) const
{
    bool ret = true;
    DrawLine(x1 + camera.x, y1 + camera.y, x2 + camera.x, y2 + camera.y, color);
    return ret;
}
