#pragma once

#include <raylib.h>
#include <entt/resource/resource.hpp>
#include <entt/resource/cache.hpp>
#include <entt/entt.hpp>
#include <box2d/box2d.h>
#include <memory>
#include <iostream>

// constexpr evaluates the value of a variable or function at compile time.
constexpr float kZoom = 5.0f;
constexpr int kWindowWidth = 640;
constexpr int kWindowHeight = 480;
constexpr int kWorldWidth = 640 / kZoom;
constexpr int kWorldHeight = 480 / kZoom;
inline const char* kWindowTitle = "Title";

constexpr float kGravity = 100.0f;

constexpr float kCharacterSwapRange = 10.0f;

constexpr Color kBackgroundColor = Color { 255, 255, 255, 255 };

struct TextureLoader {
	using result_type = std::shared_ptr<Texture>;

	result_type operator()(const char* path) {
        auto tex = LoadTexture(path);
        if (tex.id == 0) {
            std::cerr << "COULD NOT LOAD TEXTURE: " << path << std::endl;
            throw std::runtime_error{ "COULD NOT LOAD TEXTURE" };
        }
		return std::shared_ptr<Texture>(
			new Texture { tex },
			[](Texture* tex){
				UnloadTexture(*tex);
				delete tex;
			}
		);
	}
};

namespace g {

	struct Transform {
		b2Vec2 p;
		float angle;
		b2Vec2 halfExtents;
	};

}

struct System;

struct Game {
	entt::registry reg;
	entt::resource_cache<Texture, TextureLoader> textureCache;

	std::vector<std::shared_ptr<System>> systems;

	void Start();
	void Update();
	void Destroy();
};

struct System {
	Game& g;

	inline System(Game& game) : g{game} {}

	virtual void Start() {}
	virtual void Update(float dt) {}
	virtual void Destroy() {}
};

extern Game game;