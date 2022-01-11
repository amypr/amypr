#pragma once

#include <vector>
#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "render_system.hpp"
#include <random>
#include <SDL.h>
#include <SDL_mixer.h>
#include "board_fsm.hpp"

const vec2 SCOREBOARD_DIMS = vec2(100.0f, 75.0f);
const float SCOREBOARD_SPACING = 150.0f * 0.75f;
const vec2 NUMBER_DIMS = vec2(70.0f, 90.0f);
const float NUMBER_SPACING = NUMBER_DIMS.x * 0.2f;
//extern Entity board_controller;
extern Entity board_controller;

class BoardSystem
{
	std::default_random_engine generator;
public:
	RenderSystem* renderer;
	int window_height_px;
	int window_width_px;

	vec2 GAME_TILE_DIMS = vec2(150.0f, 150.0f);
	vec2 GAME_TILE_PERSPECTIVE_DISTORTION = vec2((GAME_TILE_DIMS).x, (GAME_TILE_DIMS).y * 0.7f);
	float GAME_TILE_SPACING = (GAME_TILE_DIMS).x * 1.5f;
	void setBoardDims(RenderSystem* renderer,GameBoard* g);
	int renderStage = 0;
	int background = 0;
	void init(RenderSystem* renderer, int window_width_px, int window_height_px);
	void step(float elapsed_ms);
	void deconstructGameBoard(GameBoard* board);
	void generateBoardBackground(RenderSystem* renderer, GameBoard* board);
	void generateBoardBackgroundOnce(RenderSystem* renderer, GameBoard* board, int i);
	// a game tile
	Entity createGameTile(RenderSystem* renderer, GAMETILE_TYPE type, vec2 pos);
	// a shop item
	Entity createItem(RenderSystem* renderer, TEXTURE_ASSET_ID texture, vec2 pos, vec2 scale);
	// coin animation
	void createCoins(RenderSystem* renderer, float xPos, int numCoins);
	// coin animation
	void createCoin(RenderSystem* renderer, vec2 pos);
	// a keyframe animation
	Entity createKFAnimation(RenderSystem* renderer, float angle, vec2 scale, TEXTURE_ASSET_ID texture, std::vector<vec2> positions);
	// a shop background
	void createShopBackground(RenderSystem* renderer, float xPos);
	// a shop background
	Entity createTemple(RenderSystem* renderer, vec2 pos);
	// a game board
	void generateGameBoard(RenderSystem* renderer, GameBoard* board);
	// a die for the board game
	Entity createDie(RenderSystem* renderer, vec2 position);
	// TODO other game logic here 
	void parallaxMovementUpdate();
	float cameraPositionX(float camera_x, int width);
	void drawCharacters(RenderSystem* renderer,GameBoard* board);
	void initCharacters(RenderSystem* renderer,GameBoard* board);
	void displayScoreboard();
	void removeScoreboard();
	void scoreboardMovementUpdate(float x_pos);
	void loadLevel(GameBoard* g,int window_width_px,int window_height_px);
	Entity createDialogue(TEXTURE_ASSET_ID dialogueType, int nextFrame);
	Entity createEmitter(Entity player_target, vec2 offset,vec2 scale,GEOMETRY_BUFFER_ID mesh,vec2 velocity, int num);
	Entity createSmokeEmitter(vec2 startingPos, vec2 offset);
	void destroyEmitter(Entity e);
	Entity walkEmitter;
	Entity createDialogueSprite(Motion& dialogueMotion);
	~BoardSystem();
private:
	Mix_Chunk* tile_pass_ring;	
	Mix_Chunk* dice_roll_sound;
	Mix_Chunk* dice_big_roll_sound;
	// C++ random number generator
	std::default_random_engine rng;
	float randomNumber;
    float random_level_choice;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
	void displayScoreNumber(float x_pos, int player, vec2 boardScale);
	void drawNumber(float x_pos, TEXTURE_ASSET_ID texture, vec2 boardScale);
	Entity drawNumberOnly(vec2 pos, TEXTURE_ASSET_ID texture);
	std::vector<Entity> drawn_nums {};
	TEXTURE_ASSET_ID getTexture(int digit);
};
