#include "Rendering.hpp"

using namespace entt::literals;

void g::Rendering::Start(){}
void g::Rendering::Update(float dt){
	BeginDrawing();
	BeginMode2D(g.reg.ctx().get<Camera2D>("camera"_hs));

	ClearBackground(kBackgroundColor);

	g.reg.view<g::Transform, g::Sprite>().each([&](auto entity, g::Transform& transform, g::Sprite& sprite){
		DrawTexturePro(
			sprite.tex,
			Rectangle {
				0, 0,
				static_cast<float>(sprite.tex->width),
				static_cast<float>(sprite.tex->height),
			},
			Rectangle {
				transform.p.x,
				transform.p.y,
				2 * transform.halfExtents.x,
				2 * transform.halfExtents.y,
			},
			Vector2 { transform.halfExtents.x, transform.halfExtents.y },
			transform.angle * RAD2DEG,
			RAYWHITE
		);
	});

	EndMode2D();
	EndDrawing();
}
void g::Rendering::Destroy(){}