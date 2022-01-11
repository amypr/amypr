#pragma once

#include <vector>
#include <random>
#include <stack>
#include <queue>
#include "render_system.hpp"

#include "tiny_ecs_registry.hpp"
#include "common.hpp"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 3
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
class Node {
public:
    int i;
    int j;
    int choice;
};

class AISystem
{
public:
    RenderSystem* renderer;
    int window_height_px;
    int window_width_px;
    void step(float elapsed_ms);
    std::stack<Node> find_path(int path[22][39], int sx, int sy, int ex, int ey);
    void init(RenderSystem* renderer, int window_width_px, int window_height_px);
    std::default_random_engine rng;
    std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
