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

struct WalkingCharacter {
	float speed = 30;
};

struct JumpingCharacter {
	float strength = 100;

	bool isJumping = false;
	double jumpDuration = 0.5;
	double jumpStartTime = INFINITY;
	double lastJumpTime = -INFINITY;
};

struct Character {
	float energy = 10.0f;
	float energyUseRate = 1.0f;
};

using SavedObjectTag = entt::tag<"dynamic_object"_hs>;

using TemporaryCharacter = entt::tag<"temporary_character"_hs>;

using EnergyBarTag = entt::tag<"energy_bar"_hs>;

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

	systems = std::vector<std::shared_ptr<System>> {
		std::make_shared<g::Rendering>(g::Rendering { *this }),
		std::make_shared<g::Physics>(g::Physics { *this }),
	};

	for(auto& sys : systems){
		sys->Start();
	}

	auto cameraEntity = reg.create();
	auto& camera = reg.emplace<Camera2D>(cameraEntity, Camera2D {
		.offset = { 0 },
		.target = { 0 },
		.rotation = { 0 },
		.zoom = kZoom
	});
	reg.ctx().emplace_as<Camera2D&>("main_camera"_hs, camera);

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

	fontCache.load("m5x7"_hs, "resources/fonts/m5x7.ttf");

	auto player = CreateSprite(
		reg,
		g::Transform {
			.p = b2Vec2 { 10, 10 },
			.halfExtents = b2Vec2 { 16, 16 },
		},
		g::PhysicsObject {
			.dynamic = true,
			.density = 1,
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
	reg.emplace<entt::tag<"battery"_hs>>(player);
	reg.emplace<Character>(player, Character {
		.energy = 50,
		.energyUseRate = 50,
	});
	reg.emplace<WalkingCharacter>(player, WalkingCharacter {});
	reg.emplace<JumpingCharacter>(player, JumpingCharacter {
		.strength = 100,
	});
	reg.emplace<TemporaryCharacter>(player);
	reg.emplace<SavedObjectTag>(player);


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
	reg.emplace<Character>(character, Character {
		.energy = 10,
		.energyUseRate = 1,
	});
	reg.emplace<entt::tag<"clock"_hs>>(character);
	reg.emplace<WalkingCharacter>(character, WalkingCharacter { .speed = 10 });
	reg.emplace<SavedObjectTag>(character);

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
	);

	auto testBar = reg.create();
	reg.emplace<g::Transform>(testBar, g::Transform {
		.p = {10, kWindowHeight - 10},
		.halfExtents = { 10, kWindowHeight/2 - 10 },
	});
	reg.emplace<g::SolidRect>(testBar, g::SolidRect {
		.color = Color { 255, 0, 0, 255 },
		.pivotPercent = { 0, 1 }
	});
	reg.emplace<g::UITag>(testBar);
	reg.emplace<EnergyBarTag>(testBar);
}

void Game::Update() {
	float dt = GetFrameTime();

	// energy updating and UI
	reg.view<
		entt::tag<"player"_hs>,
		Character
	>().each([&](entt::entity ent, Character& ch) {
		bool gameOver = false;
		// decrease energy
		ch.energy -= ch.energyUseRate * dt;
		if(ch.energy < 0){
			gameOver = true;
			reg.erase<entt::tag<"player"_hs>>(ent);
			AddGameOverUI();
		}

		reg.ctx().emplace_as<float>("energy"_hs, ch.energy);

		// update ui
		reg.view<
			EnergyBarTag,
			g::Transform
		>(entt::exclude<g::HiddenTag>).each([&](entt::entity energyBar, g::Transform& t){
			if(gameOver){
				reg.emplace<g::HiddenTag>(energyBar);
			}
			t.halfExtents = { 10, (kWindowHeight/2 - 10) * b2Clamp(ch.energy / 100, 0.0f, 1.0f) };
		});
	});

	// start jumping system
	reg.view<
		b2Body*,
		const g::Transform,
		const g::PhysicsObject,
		JumpingCharacter,
		entt::tag<"player"_hs>
	>()
		.each([&](
			entt::entity entity,
			b2Body*& body,
			const g::Transform trans,
			const g::PhysicsObject& po,
			JumpingCharacter& ch
		) {
			struct RaycastResult : public b2RayCastCallback {
				bool hit = false;
				float ReportFixture(
					b2Fixture* fixture,
					const b2Vec2& point,
					const b2Vec2& normal,
					float fraction
				) override {
					hit = true;
					return 0;
				}
			};

			if(IsKeyPressed(KEY_W) && GetTime() > ch.lastJumpTime + 0.2){
				auto& world = reg.ctx().get<std::shared_ptr<b2World>>();
				RaycastResult res;
				b2Vec2 initPos = body->GetPosition();
				b2Vec2 finalPos = initPos;
				finalPos += b2Vec2{0, trans.halfExtents.y + po.halfExtentOffset.y + 1.0f};
				world->RayCast(&res, initPos, finalPos);
				if(res.hit){
					b2Vec2 jumpImpulse { 0, -100 * ch.strength };
					body->ApplyLinearImpulseToCenter(jumpImpulse, true);
					ch.lastJumpTime = GetTime();
					ch.jumpStartTime = GetTime();
					ch.isJumping = true;
				}
			}
		});

	// apply jump force
	reg.view<
		b2Body*,
		JumpingCharacter
	>().each([&](entt::entity e, b2Body* body, JumpingCharacter& ch){
		if(!ch.isJumping) return;

		if(!reg.storage<entt::tag<"player"_hs>>().contains(e)) {
			ch.isJumping = false;
		}
		else{
			if(!IsKeyDown(KEY_W) || GetTime() > ch.jumpStartTime + ch.jumpDuration){
				ch.isJumping = false;
			}
		}

		if(ch.isJumping){
			b2Vec2 force = { 0, -150 * ch.strength };
			body->ApplyForceToCenter(force, true);
		}
	});

	for(auto& sys : systems){
		sys->Update(dt);
	}


	// walking system
	reg.view<b2Body*, const WalkingCharacter, entt::tag<"player"_hs>>()
		.each([&](entt::entity entity, b2Body*& body, const WalkingCharacter& ch) {
			b2Vec2 inputDir {
				IsKeyDown(KEY_D) * 1.0f + IsKeyDown(KEY_A) * -1.0f,
				IsKeyDown(KEY_S) * 1.0f + IsKeyDown(KEY_W) * -1.0f,
			};

			inputDir.Normalize();

			b2Vec2 vel = body->GetLinearVelocity();
			vel.x = inputDir.x * ch.speed * (reg.ctx().get<float>("energy"_hs) / 100.0f + 1.0f);

			body->SetLinearVelocity(vel);
		});

	//
	reg.view<Character, entt::tag<"player"_hs>>()
		.each([&](entt::entity ent, Character& ch) {
			if(IsKeyPressed(KEY_SPACE) && reg.ctx().contains<entt::entity>("touching_player"_hs)){
				auto other = reg.ctx().get<entt::entity>("touching_player"_hs);

				reg.remove<entt::tag<"player"_hs>>(ent);
				reg.emplace<entt::tag<"player"_hs>>(other);

				// add energy from current character to other
				auto& otherCh = reg.get<Character>(other);
				otherCh.energy += ch.energy;

				// kill the currrent character if they are temporary
				if(reg.storage<TemporaryCharacter>().contains(ent)){
					reg.erase<Character>(ent);
					reg.erase<TemporaryCharacter>(ent);
				}

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
				if(fixture->GetBody() == playerBody) return true;
				b2Vec2 vec = fixture->GetBody()->GetPosition();
				vec -= playerBody->GetPosition();
				float dist2 = vec.LengthSquared();

				float otherDist2 = result.has_value() ? result->first : INFINITY;
				if(otherDist2 < dist2) return true;

				auto ent = static_cast<entt::entity>(fixture->GetBody()->GetUserData().pointer);
				if(g.reg.storage<Character>().contains(ent)) {
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

void Game::AddGameOverUI(){
	auto text = reg.create();
	reg.emplace<g::Transform>(text, g::Transform {
		.p = { kWindowWidth/2, kWindowHeight/2 },
	});
	reg.emplace<g::UITag>(text);

	reg.emplace<g::Text>(text, g::Text {
		.font = fontCache["m5x7"_hs],
		.text = "GAME OVER",
		.size = 48,
		.color = Color { 255, 0, 0, 255 },
	});

	auto spaceToRestart = reg.create();
	reg.emplace<g::Transform>(spaceToRestart, g::Transform {
		.p = { kWindowWidth/2, kWindowHeight/2 + 48 },
	});
	reg.emplace<g::UITag>(spaceToRestart);

	reg.emplace<g::Text>(spaceToRestart, g::Text {
		.font = fontCache["m5x7"_hs],
		.text = "PRESS SPACE TO RESTART",
		.size = 36,
		.color = Color { 255, 0, 0, 255 },
	});
}