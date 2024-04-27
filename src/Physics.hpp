#pragma once

#include "Game.hpp"

#include <raylib.h>
#include <entt/resource/resource.hpp>
#include <entt/resource/cache.hpp>
#include <entt/entt.hpp>
#include <box2d/box2d.h>
#include <memory>

namespace g {
	struct PhysicsObject {
		bool dynamic;
		float density;
		float friction;
		float gravityScale;
		bool fixedRotation;
		float drag;
	};

	struct Physics : public System {
		std::shared_ptr<b2World> world;

		using System::System;

		void Start() override;
		void Update(float dt) override;

		void OnPhysicsObjectConstruct(entt::registry&, entt::entity entity);
	};
}