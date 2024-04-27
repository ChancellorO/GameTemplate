#include "Game.hpp"
#include "Physics.hpp"
#include "Rendering.hpp"

#include <box2d/b2_distance.h>
#include <raylib.h>
#include <optional>
#include <algorithm>
#include <entt/entt.hpp>
#include <iostream>

using namespace entt::literals;

// struct CharacterInRange {
// 	entt::
// };

entt::entity CreateSprite(entt::registry& reg, g::Transform trans, g::PhysicsObject po, g::Sprite s, g::AnimationController ac){
	entt::entity entity = reg.create();

	reg.emplace<g::Transform>(entity, trans);
	reg.emplace<g::PhysicsObject>(entity, po);
	reg.emplace<g::Sprite>(entity, s);
    reg.emplace<g::AnimationController>(entity, ac);

	return entity;
}

void Game::Start() {
	SetConfigFlags(0);

	InitWindow(kWindowWidth, kWindowHeight, kWindowTitle);

	reg.ctx().emplace_as<Camera2D>("camera"_hs, Camera2D {
		.offset = { 0 },
		.target = { 0 },
		.rotation = { 0 },
		.zoom = kZoom
	});

	systems = std::vector<std::shared_ptr<System>> {
		std::make_shared<g::Rendering>(g::Rendering { *this }),
		std::make_shared<g::Physics>(g::Physics { *this }),
	};

	for(auto& sys : systems){
		sys->Start();
	}

	auto entity = reg.create();

	textureCache.load("cat"_hs, "resources/cat.jpg");
    textureCache.load("computer_run0"_hs, "resources/computer_run/sprite_0.png");
    textureCache.load("computer_run1"_hs, "resources/computer_run/sprite_1.png");    
    textureCache.load("computer_run2"_hs, "resources/computer_run/sprite_2.png");
    textureCache.load("computer_idle0"_hs, "resources/computer_idle/sprite_0.png");
    textureCache.load("computer_idle1"_hs, "resources/computer_idle/sprite_1.png");    
    textureCache.load("battery_run0"_hs, "resources/battery_run/sprite_0.png");
    textureCache.load("battery_run1"_hs, "resources/battery_run/sprite_1.png");
    textureCache.load("battery_run2"_hs, "resources/battery_run/sprite_2.png");
    textureCache.load("battery_run3"_hs, "resources/battery_run/sprite_3.png");
    textureCache.load("battery_idle0"_hs, "resources/battery_idle/sprite_0.png");
    textureCache.load("battery_idle1"_hs, "resources/battery_idle/sprite_1.png");
    textureCache.load("battery_idle2"_hs, "resources/battery_idle/sprite_2.png");
    textureCache.load("battery_idle0"_hs, "resources/battery_idle/sprite_0.png");
    textureCache.load("alarm_idle0"_hs, "resources/alarm_idle/sprite_0.png");
    textureCache.load("alarm_idle1"_hs, "resources/alarm_idle/sprite_1.png");
    textureCache.load("water_idle0"_hs, "resources/alarm_idle/sprite_0.png");
    textureCache.load("water_idle1"_hs, "resources/alarm_idle/sprite_1.png");
    textureCache.load("water_idle2"_hs, "resources/alarm_idle/sprite_2.png");
    


	auto player = CreateSprite(
		reg,
		g::Transform {
			.p = b2Vec2 { 10, 10 },
			.halfExtents = b2Vec2 { 16, 16 },
		},
		g::PhysicsObject {
			.dynamic = true,
			.density = 10,
			.fixedRotation = true,
			.drag = 0,
			.halfExtentOffset = { -3, -3 },
		},
		g::Sprite { textureCache["cat"_hs] },
        g::AnimationController {
            .textures = {textureCache["battery_run0"_hs], textureCache["battery_run1"_hs], textureCache["battery_run2"_hs], textureCache["battery_run3"_hs]},
        }
	);
	reg.emplace<entt::tag<"player"_hs>>(player);

	auto character = CreateSprite(
		reg,
		g::Transform {
			.p = b2Vec2 { 50, 10 },
			.halfExtents = b2Vec2 { 16, 16 },
		},
		g::PhysicsObject {
			.dynamic = true,
			.density = 5,
			.fixedRotation = true,
			.drag = 1,
            .positionOffset = b2Vec2 { -4, -1 },
            .halfExtentOffset = b2Vec2 { -7, -4 },
            
		},
		g::Sprite { textureCache["cat"_hs] },
        g::AnimationController {
            .textures = {textureCache["computer_run0"_hs], textureCache["computer_run1"_hs], textureCache["computer_run2"_hs]}
        }        
	);
	reg.emplace<entt::tag<"character"_hs>>(character);

	CreateSprite(
		reg,
		g::Transform {
			.p = b2Vec2 { kWorldWidth / 2, kWorldHeight - 5 },
			.halfExtents = b2Vec2 { kWorldWidth / 2, 5 },
		},
		g::PhysicsObject {},
		g::Sprite { textureCache["cat"_hs] },
        g::AnimationController {
        }        
	);}

void Game::Update() {
	float dt = GetFrameTime();

	for(auto& sys : systems){
		sys->Update(dt);
	}

	reg.view<b2Body*, entt::tag<"player"_hs>>().each([&](entt::entity entity, b2Body*& body) {
		b2Vec2 inputDir {
			IsKeyDown(KEY_D) * 1.0f + IsKeyDown(KEY_A) * -1.0f,
			IsKeyDown(KEY_S) * 1.0f + IsKeyDown(KEY_W) * -1.0f,
		};

		inputDir.Normalize();
		inputDir *= 30.0f;

		body->SetLinearVelocity(inputDir);


		if(IsKeyPressed(KEY_SPACE) && reg.ctx().contains<entt::entity>("touching_player"_hs)){
			auto other = reg.ctx().get<entt::entity>("touching_player"_hs);
			reg.remove<entt::tag<"player"_hs>>(entity);
			reg.emplace<entt::tag<"character"_hs>>(entity);

			reg.remove<entt::tag<"character"_hs>>(other);
			reg.emplace<entt::tag<"player"_hs>>(other);

			reg.ctx().erase<entt::entity>("touching_player"_hs);
		}
	});

	reg.view<b2Body*, const g::Transform, entt::tag<"player"_hs>>().each([&](entt::entity entity, b2Body*& body, const g::Transform& t) {
		auto world = reg.ctx().get<std::shared_ptr<b2World>>();

		bool contacting = false;

		struct BBResults : public b2QueryCallback {
			Game& g;
			b2Body* playerBody;
			std::optional<std::pair<float, entt::entity>> result;
			BBResults(Game& g, b2Body* playerBody) : g{g}, playerBody{playerBody} {}

			bool ReportFixture(b2Fixture* fixture) override {
				b2Vec2 vec = fixture->GetBody()->GetPosition();
				vec -= playerBody->GetPosition();
				float dist2 = vec.LengthSquared();

				float otherDist2 = result.has_value() ? result->first : INFINITY;
				if(otherDist2 < dist2) return true;

				auto ent = static_cast<entt::entity>(fixture->GetBody()->GetUserData().pointer);
				if(g.reg.storage<entt::tag<"character"_hs>>().contains(ent)) {
					result = std::make_pair(dist2, ent);
				}
				return true;
			};
		};

		BBResults res { game, body };
		b2AABB bb;

		b2Vec2 swapRangeExtents {
			kCharacterSwapRange + t.halfExtents.x,
			kCharacterSwapRange + t.halfExtents.y,
		};

		bb.lowerBound = body->GetPosition();
		bb.lowerBound -= swapRangeExtents;
		bb.upperBound = body->GetPosition();
		bb.upperBound += swapRangeExtents;
		world->QueryAABB(&res, bb);

		if(res.result){
			auto other = res.result->second;
			reg.ctx().emplace_as<entt::entity>("touching_player"_hs, other);
		}
		else {
			reg.ctx().erase<entt::entity>("touching_player"_hs);
		}
	});

	if (dt < 1/60.0f){
		WaitTime(1/60.0f - dt);
	}
}

void Game::Destroy() {
	for(auto& sys : systems){
		sys->Destroy();
	}

	textureCache.clear();
	CloseWindow();
}