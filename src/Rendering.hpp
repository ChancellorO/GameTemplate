#pragma once

#include "Game.hpp"

namespace g {
	struct AnimationController {
		std::vector<entt::resource<Texture>> textures;
		double frame_interval = 0.1;
		int current_frame = -1;
		double last_frame_time = -INFINITY;
	};

	struct Sprite {
		entt::resource<Texture> tex;
	};

	struct Rendering : public System {
		using System::System;

		void Start() override;
		void Update(float dt) override;
		void Destroy() override;

		void UpdateAnimation(AnimationController& ac, g::Sprite& s);
	};
}