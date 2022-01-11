#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"
#include "start_screen.hpp"
#include "menu.hpp"
#include "drawing_game.hpp"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods

class WorldSystem
{
public:
	WorldSystem();

	// Creates a window
	GLFWwindow* create_window(int width, int height);

	// starts the game
	void init(RenderSystem* renderer, StartSystem& startSys, MenuSystem& menuSys);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	// Check for collisions
	void handle_collisions();

	// Should the game be over ?
	bool is_over()const;
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void mouse_button_callback(int button, int action, int mods);
	void on_mouse_move(vec2 pos);

	// restart level
	void restart_game();

	// OpenGL window handle
	GLFWwindow* window;

	// Number of fish eaten by the salmon, displayed in the window title
	unsigned int points;

	// start + menu that are accessed in world, from main
	// should be accessible to main
	StartSystem startSystem;
	MenuSystem menuSystem;
	DrawingSystem drawingSystem;

	// Game state
	RenderSystem* renderer;
	float current_speed;
	float next_turtle_spawn;
	float next_wall_spawn;
	Entity drawing;
	Entity player_one;
	Entity player_two;
    Entity player_tank1;
    Entity player_tank2;
	float elapsed_ms;

	// music references
	Mix_Music* background_music;
	Mix_Chunk* salmon_dead_sound;
	Mix_Chunk* salmon_eat_sound;
	Mix_Chunk* dice_roll_sound;
	Mix_Chunk* dice_pickup_sound;
	Mix_Chunk* jump_1_sound;
	Mix_Chunk* paint_1_sound;
    Mix_Chunk* cannon_fire_sound;
	Mix_Chunk* tank_music;
    Mix_Chunk* tank_hit_sound;
    Mix_Chunk* tank_on_fire_sound;
	

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
