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

	g.reg.view<g::AnimationController, g::Sprite>().each([&](AnimationController& ac, Sprite& s ) {
		UpdateAnimation(ac, s);
	});
	EndMode2D();
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