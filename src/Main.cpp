#include <raylib.h>
#include <box2d/box2d.h>
#include <entt/entt.hpp>
#include "Game.hpp"

#include "Game.hpp"

#include "Game.hpp"

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

Game game;

void MainLoop(){
	game.Update();
}

int main(){
	game.Start();

#ifdef PLATFORM_WEB
	emscripten_set_main_loop(MainLoop, 0, 1);
#else
	while(!WindowShouldClose()){
		MainLoop();
	}
#endif

	game.Destroy();

	return 0;
}
