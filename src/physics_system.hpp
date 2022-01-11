#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include "render_system.hpp"
#include <SDL_mixer.h>


// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	void step(float elapsed_ms, float window_width_px, float window_height_px);
	void* boardSystem;
	RenderSystem* renderer;
	int window_height_px;
	int window_width_px;
	void init(void* Board, RenderSystem* renderer, int window_width_px, int window_height_px);

	PhysicsSystem()
	{
	}
private:
    
    Mix_Chunk* tank_hit_sound;
    Mix_Chunk* tank_on_fire_sound;
    Mix_Chunk* wall_collapsed_sound;
    
    void sphere_collision(Entity entity_i, Entity entity_j);
};
