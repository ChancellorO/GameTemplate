#include <raylib.h>
#include <box2d/box2d.h>
#include <entt/entt.hpp>

constexpr int kWindowWidth = 500;
constexpr int kWindowHeight = 500;
const char* kWindowTitle = "Title";

constexpr Color kBackgroundColor = Color { 255, 255, 255, 255 };

int main(){
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	InitWindow(kWindowWidth, kWindowHeight, kWindowTitle);

	const auto cat = LoadTexture("resources/cat.jpg");

	while(!WindowShouldClose()){
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

	CloseWindow();

	return 0;
}