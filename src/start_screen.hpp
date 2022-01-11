// TODO put constructors and deconstructors for the game board here
#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "render_system.hpp"
#include <random>
#include "board_system.hpp"
#include <iostream> 
#include <sstream>
#include <fstream>

class StartSystem
{
public:
	RenderSystem* renderer;
	int window_height_px;
	int window_width_px;
	std::string pathForSaveFile;
	Entity new_button;
	Entity load_button;
	Entity level1_button;
	Entity level2_button;
	Entity background;
	Entity logo;
	Entity board_help;
	Entity drawing_help;
	Entity platformer_help;
	void init(RenderSystem* renderer, int window_width_px, int window_height_px);
	void buildScreen();
	void deconstructScreen();
	void buildLevelSelectScreen();

	// Button on menu to press
	Entity createButton(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID buttonType);
	// Background
	Entity createBackground(RenderSystem* renderer, vec2 position);
	// Logo
	Entity createLogo(RenderSystem* renderer, vec2 position);
	Entity makeLabel(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID texture);
	// save + load (along with helper functions)
	void serialize(const GameBoard& g);
	bool loadSave();
	void readBasicGameBoard(GameBoard& g, std::string token, std::string delimiter, int player);
	void readGameBoardBackground(GameBoard& g, std::string token, std::string delimiter);
	void readBackGroundComponent(std::vector<SceneBackGroundComponent>& vector, std::string token, std::string delimiter);
};
// overriding operator to read in objects
std::ostream& operator<<(std::ostream& os, const GameBoard& g);
std::ostream& operator<<(std::ostream& os, const std::vector<SceneBackGround>& backgrounds);
std::ostream& operator<<(std::ostream& os, const SceneBackGroundComponent& sbc);