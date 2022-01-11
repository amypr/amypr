
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "ai_system.hpp"
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"
#include "board_system.hpp"
#include "platformer_system.hpp"
#include "tank_system.hpp"
#include "drawing_game.hpp"
#include "menu.hpp"
#include "start_screen.hpp"

// for convenience


using Clock = std::chrono::high_resolution_clock;

int window_width_px = 1200;
int window_height_px = 800;
// Entry point
int main()
{	
	glfwInit();
	// screen res adapted from https://gamedev.stackexchange.com/questions/60244/how-to-find-monitor-resolution-with-glfw3 that was referenced on Piazza @194
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	window_width_px = mode->width * 0.9f;
    window_height_px = mode->height * 0.9f;
	
	// Global systems
	WorldSystem world;
	RenderSystem renderer;
	PhysicsSystem physics;
	AISystem ai;
	BoardSystem board;
	PlatformerSystem platformer;
    TankSystem tankGame;
	DrawingSystem drawingGame;
	MenuSystem menuSystem;
	StartSystem startSystem;
	
	// Initializing window
	GLFWwindow* window = world.create_window(window_width_px, window_height_px);
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// initialize the main systems
	renderer.init(window_width_px, window_height_px, window);
	startSystem.init(&renderer, window_width_px, window_height_px);
	board.init(&renderer, window_width_px, window_height_px);
	platformer.init(&renderer, window_width_px, window_height_px);
    tankGame.init(&renderer, window_width_px, window_height_px);
	drawingGame.init(&renderer, window_width_px, window_height_px);
	menuSystem.init(&renderer, startSystem, window_width_px, window_height_px);
	physics.init(&board, &renderer, window_width_px, window_height_px);
	world.init(&renderer, startSystem, menuSystem);
	ai.init(&renderer, window_width_px, window_height_px);
	

	// variable timestep loop
	auto t = Clock::now();
	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become
		// unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;
		
		Menu& menu = registry.menu.components[0];
		StartScreen& ss = registry.startScreen.components[0];
		Camera& camera = registry.camera.components[0];
        TankGame& tg = registry.tankGame.components[0];
		if (menu.paused && !menu.active) {
			menuSystem.buildMenu();
		} 
		else if (!menu.active && !ss.active) {
			board.step(elapsed_ms);
			platformer.step(elapsed_ms);
			world.step(elapsed_ms);
            if (tg.active && tg.loaded) {
                ai.step(elapsed_ms);
            }
            tankGame.step(elapsed_ms);
			drawingGame.step(elapsed_ms);
			physics.step(elapsed_ms, window_width_px, window_height_px);
			world.handle_collisions();
			renderer.step(elapsed_ms);
		}
		if (ss.active && ss.restarted) {
			// step through all systems to ensure components are removed
			board.step(elapsed_ms);
			platformer.step(elapsed_ms);
			tankGame.step(elapsed_ms);
			drawingGame.step(elapsed_ms);
			renderer.step(elapsed_ms);
			// set gameboard for board_controller to the one from first init so that we're using the same one (prevent emplacing another gameboard component for board_controller)
			board.init(&renderer, window_width_px, window_height_px);

			// remove the menu screen, build the start screen
			menu.paused = false;
			menuSystem.deconstructMenu();
			camera.x = 0;
			startSystem.buildScreen();
			ss.restarted = false;
		}

		renderer.draw();

		// TODO A2: you can implement the debug freeze here but other places are possible too.
	}

	return EXIT_SUCCESS;
}

