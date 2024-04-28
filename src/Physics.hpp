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
		/// @brief if true, the body will have forces applied to it
		bool dynamic = false;
		/// @brief the weight of the body
		float density = 10;
		float friction = 0;
		/// @brief set to 0 for no gravity
		float gravityScale = 1;
		/// @brief if true, the body will not rotate
		bool fixedRotation = true;
		/// @brief slowing force so that does not move linearly infinitely
		float drag = 1;
        /// @brief Check for collision with water
        bool isSensor = false;
		/// @brief the position of the collider relative to the transform center
		b2Vec2 positionOffset = { 0, 0 };
		/// @brief the size of the collider relative to the transform rect
		b2Vec2 halfExtentOffset = { 0, 0 };
	};

	struct Physics : public System {
		std::shared_ptr<b2World> world;

		using System::System;

		void Start() override;
		void Update(float dt) override;

		void OnPhysicsObjectDestruct(entt::registry&, entt::entity entity);

		void OnPhysicsObjectConstruct(entt::registry&, entt::entity entity);
	};
}