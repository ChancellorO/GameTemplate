#pragma once

#include <iostream>

#include <raylib.h>
#include <entt/resource/resource.hpp>
#include <entt/resource/cache.hpp>
#include <entt/entt.hpp>
#include <box2d/box2d.h>
#include <memory>

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
		return std::shared_ptr<Texture>(
			new Texture { LoadTexture(path) },
			[](Texture* tex){
				UnloadTexture(*tex);
				delete tex;
			}
		);
	}
};

struct FontLoader {
	using result_type = std::shared_ptr<Font>;

	result_type operator()(const char* path) {
		Font font = LoadFont(path);
		if(font.texture.id == 0) {
			std::cerr << "Could not load font: " << path << std::endl;
			throw std::runtime_error{"Could not load font"};
		}
		return std::shared_ptr<Font>(
			new Font { LoadFont(path) },
			[](Font* f){
				UnloadFont(*f);
				delete f;
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
	entt::resource_cache<Font, FontLoader> fontCache;

	std::vector<std::shared_ptr<System>> systems;

	void Start();
	void Update();
	void Destroy();

	void AddGameOverUI();
};

struct System {
	Game& g;

	inline System(Game& game) : g{game} {}

	virtual void Start() {}
	virtual void Update(float dt) {}
	virtual void Destroy() {}
};

extern Game game;