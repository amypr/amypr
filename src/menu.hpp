// TODO put constructors and deconstructors for the game board here
#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "render_system.hpp"
#include <random>
#include "board_system.hpp"
#include "start_screen.hpp"

class MenuSystem
{
	std::default_random_engine rng;
public:
	RenderSystem* renderer;
	int window_height_px;
	int window_width_px;
	Entity exit_button;
	Entity save_and_exit_button;
	Entity help_button;
	void init(RenderSystem* renderer, StartSystem& startSys, int window_width_px, int window_height_px);
	void buildMenu();
	void deconstructMenu();
	StartSystem startSystem;
	float cameraPositionX();
	void helpScreens(TEXTURE_ASSET_ID helpScreensType);
	Entity createHelpScreens(RenderSystem* renderer, TEXTURE_ASSET_ID helpScreensType);
};