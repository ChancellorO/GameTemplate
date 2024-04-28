#include "Rendering.hpp"

using namespace entt::literals;

void g::Rendering::Start(){}
void g::Rendering::Update(float dt){
	BeginDrawing();
	BeginMode2D(g.reg.ctx().get<Camera2D&>("main_camera"_hs));

	ClearBackground(kBackgroundColor);

	g.reg.view<g::Transform, g::Sprite>(entt::exclude<UITag>).each([&](auto entity, g::Transform& transform, g::Sprite& sprite){
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
			Vector2 { transform.halfExtents.x * 2 * sprite.pivotPercent.x, transform.halfExtents.y * 2 * sprite.pivotPercent.y },
			transform.angle * RAD2DEG,
			RAYWHITE
		);
	});

	g.reg.view<g::AnimationController, g::Sprite>().each([&](AnimationController& ac, Sprite& s ) {
		UpdateAnimation(ac, s);
	});
	EndMode2D();

	// sprite
	g.reg.view<g::Transform, g::Sprite, g::UITag>().each([&](auto entity, g::Transform& transform, g::Sprite& sprite){
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
			Vector2 { transform.halfExtents.x * 2 * sprite.pivotPercent.x, transform.halfExtents.y * 2 * sprite.pivotPercent.y },
			transform.angle * RAD2DEG,
			RAYWHITE
		);
	});

	// solid rect
	g.reg.view<g::Transform, g::SolidRect, g::UITag>(entt::exclude<g::HiddenTag>)
		.each([&](auto entity, g::Transform& transform, g::SolidRect& rect){
			DrawRectanglePro(
				Rectangle {
					transform.p.x,
					transform.p.y,
					2 * transform.halfExtents.x,
					2 * transform.halfExtents.y,
				},
				Vector2 { transform.halfExtents.x * 2 * rect.pivotPercent.x, transform.halfExtents.y * 2 * rect.pivotPercent.y },
				transform.angle * RAD2DEG,
				rect.color
			);
		});

	// text
	g.reg.view<g::Transform, g::Text, g::UITag>(entt::exclude<g::HiddenTag>)
		.each([&](auto entity, g::Transform& transform, g::Text& text){
			auto size = MeasureTextEx(text.font, text.text.c_str(), text.size, 0);
			DrawTextPro(
				*text.font,
				text.text.c_str(),
				Vector2 { transform.p.x, transform.p.y },
				Vector2 { size.x * text.pivotPercent.x, size.y * text.pivotPercent.y },
				0, text.size,
				0, text.color
			);
		});

	EndDrawing();
}
void g::Rendering::Destroy(){}

void g::Rendering::UpdateAnimation(AnimationController& ac, Sprite& s ) {

	//guard clause
	if (GetTime() <= ac.last_frame_time + ac.frame_interval) {
		return;
	}

	if (ac.textures.size() == 0) {
		return;
	}

	if (ac.current_frame + 1 < ac.textures.size()) {
		ac.current_frame++;
	} else {
		ac.current_frame = 0;
	}

	// Change sprite's texture
	s.tex = ac.textures[ac.current_frame];

	ac.last_frame_time = GetTime();

}