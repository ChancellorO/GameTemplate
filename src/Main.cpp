#include <raylib.h>
#include <box2d/box2d.h>
#include <entt/entt.hpp>

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

constexpr int kWindowWidth = 500;
constexpr int kWindowHeight = 500;
const char* kWindowTitle = "Title";

constexpr Color kBackgroundColor = Color { 255, 255, 255, 255 };

static Texture cat;

void MainLoop(){
	BeginDrawing();

	ClearBackground(kBackgroundColor);

	DrawTexturePro(
		cat,
		// source
		Rectangle {
			0, 0,
			static_cast<float>(cat.width), static_cast<float>(cat.height),
		},
		// dest
		Rectangle {
			24, 24,
			static_cast<float>(GetScreenWidth() - 48), static_cast<float>(GetScreenHeight() - 48),
		},
		// origin
		Vector2 { 0, 0 },
		// rotation
		0,
		// tint
		RAYWHITE
	);

	EndDrawing();
}

int main(){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	InitWindow(kWindowWidth, kWindowHeight, kWindowTitle);

	cat = LoadTexture("resources/cat.jpg");

#ifdef PLATFORM_WEB
	emscripten_set_main_loop(MainLoop, 0, 1);
#else
	while(!WindowShouldClose()){
		MainLoop();
	}
#endif

	CloseWindow();

	return 0;
}
