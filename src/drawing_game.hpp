#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "render_system.hpp"
#include "board_system.hpp"

#include <random>

class DrawingSystem
{
	std::default_random_engine generator;
public:
	RenderSystem* renderer;
	int window_height_px;
	int window_width_px;
	Entity randomDrawing;
	void init(RenderSystem* renderer, int window_width_px, int window_height_px);
	void step(float elapsed_ms);
	void deconstructDrawing();
	void buildDrawing();
	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	// character (controlled by players)
	// Entity createDrawing(RenderSystem* renderer, vec2 position, GEOMETRY_BUFFER_ID drawingNumber);
	Entity createDrawing(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID drawingNumber);
	Entity createPlayerWriting(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID drawingNumber);
	Entity createLamp(RenderSystem* renderer);
	void drawLine(vec2 position, vec2 scale);
};