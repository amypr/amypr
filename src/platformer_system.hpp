// TODO put constructors and deconstructors for the game board here
#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "render_system.hpp"
#include <random>
#include "board_system.hpp"
#include "platformer_ai.hpp"

class PlatformerSystem
{
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist;
	float next_ai_update;
	float speed = 1.0f;

public:
	RenderSystem* renderer;
	int window_height_px;
	int window_width_px;
	Entity player_one;
	Entity player_two;
	Entity ai_one;
	Entity ai_two;
	float chanceForTrap;

	void init(RenderSystem* r, int width, int height);
	void step(float elapsed_ms);
	void buildPlatformer();
	void deconstructPlatformer();

	// character (controlled by players/ai)
	void createCharacters();

	// platforms
	Entity createPlatform(vec2 pos);
	// goal
	Entity createGoal(vec2 pos);
	// trap
	Entity createTrap(vec2 pos);

};

extern std::vector<std::tuple<BTNode*, Entity>> ai_registry;