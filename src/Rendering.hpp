#pragma once

#include "Game.hpp"

#include <string>

namespace g {
	struct AnimationController {
		std::vector<entt::resource<Texture>> textures;
		double frame_interval = 0.5;
		int current_frame = -1;
		double last_frame_time = -INFINITY;
	};

	struct Sprite {
		entt::resource<Texture> tex;
		Vector2 pivotPercent = Vector2 { 0.5f, 0.5f };
	};

	using UITag = entt::tag<entt::hashed_string{"UI"}>;
	using HiddenTag = entt::tag<entt::hashed_string{"hidden"}>;

	struct SolidRect {
		Color color;
		Vector2 pivotPercent = Vector2 { 0.5f, 0.5f };
	};

	struct Text {
		entt::resource<Font> font;
		std::string text;
		size_t size;
		Color color;
		Vector2 pivotPercent = Vector2 { 0.5f, 0.5f };
	};

	struct Rendering : public System {
		using System::System;

		void Start() override;
		void Update(float dt) override;
		void Destroy() override;

		void UpdateAnimation(AnimationController& ac, g::Sprite& s);
	};
}