#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are ahrd coded to the dimensions of the entity texture
//const float FISH_BB_WIDTH = 0.4f * 296.f;
//const float FISH_BB_HEIGHT = 0.4f * 165.f;
//const float TURTLE_BB_WIDTH = 0.4f * 300.f;
//const float TURTLE_BB_HEIGHT = 0.4f * 202.f;



// the player
Entity createDrawing(RenderSystem* renderer, vec2 pos);
// a red line for debugging purposes
// Entity createLine(vec2 position, vec2 size);
// a pebble
Entity createPebble(vec2 pos, vec2 size);
//// the player in tank game
//Entity createPlayerTank(RenderSystem* renderer, vec2 pos);
//// the wall in tank game
//Entity createWalls(RenderSystem* renderer, vec2 position);
//// the enemy tank in tank game
//Entity createAITank(RenderSystem* renderer, vec2 position);
//// a AI bullet in tank game
//Entity createAIBullet(vec2 pos, vec2 size);
//// a player bullet in tank game
//Entity createPlayerBullet(vec2 pos, vec2 size);





