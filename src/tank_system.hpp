// TODO put constructors and deconstructors for the game board here
#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "tiny_ecs.hpp"
#include "common.hpp"
#include "render_system.hpp"
#include <random>
#include "board_system.hpp"

const float TURTLE_BB_WIDTH = 0.4f * 150.f;
const float TURTLE_BB_HEIGHT = 0.4f * 152.f;

// tank game
Entity createTankGame(RenderSystem* renderer, vec2 pos);
// the player
Entity createPlayer1(RenderSystem* renderer, vec2 pos);
// the player
Entity createPlayer2(RenderSystem* renderer, vec2 pos);
// the wall
Entity createWall(RenderSystem* renderer, vec2 position);
// the broken wall
Entity createBrokenWall(RenderSystem* renderer, vec2 position);
// the enemy
Entity createAITank(RenderSystem* renderer, vec2 position);
// the big tank
Entity createTanking(RenderSystem* renderer, vec2 position);
// the hardwall
Entity createHardWall(RenderSystem* renderer, vec2 position);
// the home
Entity createHome(RenderSystem* renderer, vec2 position);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// a AI bullet
Entity createAIBullet(vec2 pos, vec2 size);
// a player bullet
Entity createPlayerBullet(vec2 pos, vec2 size);
// game point
Entity createPoints();

class TankSystem
{
	std::default_random_engine rng;
public:
	RenderSystem* renderer;
	int window_height_px;
	int window_width_px;
	void init(RenderSystem* renderer, int window_width_px, int window_height_px);
	void step(float elapsed_ms);
	void buildTankGame();
	void deconstructTankGame();
};
