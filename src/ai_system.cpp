#include "ai_system.hpp"
#include "tank_system.hpp"
#include "world_system.hpp"
#include "render_system.hpp"
#include <iostream>
#include <stack>
#include <deque>
#include <queue>
#include <vector>


int backg[22][39] = {
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
    {2,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,2,1,1,1,0,0,0,0,0,0,0,2,0,0,1,0,0,0,0,0,1,1,1,1,1,1,1,2},
    {2,0,0,0,0,0,0,0,0,0,0,2,0,0,1,0,0,1,0,0,0,0,2,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,2},
    {2,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,1,1,0,0,0,0,0,0,2},
    {2,0,0,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,2,2,0,0,2},
    {2,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,2,0,0,0,2},
    {2,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2},
    {2,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,0,0,0,2},
    {2,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,1,0,0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,3,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
};


std::stack<Node> result;
std::queue<vec2> current_path;
std::vector<vec2> copy_path;

void AISystem::init(RenderSystem* renderer, int window_width_px, int window_height_px) {
    this->window_width_px = window_width_px;
    this->window_height_px = window_height_px;
    this->renderer = renderer;
}

void AISystem::step(float elapsed_ms)
{
    TankGame& t = registry.tankGame.get(board_controller);
    if (t.loaded && t.active && registry.playerTanks.size() > 0) {
        auto& AIs = registry.aiTanks.entities;
        auto& motion_registry = registry.motions;
        std::vector<vec2> positions;
        for (int i = 0; i < registry.playerTanks.size(); i++) {
            positions.push_back(motion_registry.get(registry.playerTanks.entities[i]).position);
        }
        rng = std::default_random_engine(std::random_device()());
        if (t.level == 2) {
            Entity& home = registry.tankHome.entities[0];
            Motion& motion_home = motion_registry.get(home);
            vec2 position_home = motion_home.position;
            registry.tankings.components[0].last_check += elapsed_ms;
            if (registry.tankings.components[0].last_check >= 2000.f) {
                Tanking& t = registry.tankings.components[0];
                t.last_check -= 500.f;
                Entity tanking = registry.tankings.entities[0];
                Motion& motion_ai = motion_registry.get(tanking);
                vec2 position_ai = motion_ai.position;

                if (result.empty()) {
                    result = find_path(backg, floor(position_home.y / (renderer->window_height_px / 22.0f)), floor(position_home.x / (renderer->window_width_px / 39.0f)), floor(position_ai.y / (renderer->window_height_px / 22.0f)), floor(position_ai.x / (renderer->window_width_px / 39.0f)));
                    while (!result.empty()) {
                        Node* node = new Node();
                        *node = result.top();
                        float y_i = node->i * (renderer->window_height_px / 22.0f) + (renderer->window_height_px / 22.0f)*0.5;
                        float x_i = node->j * (renderer->window_width_px / 39.0f) + (renderer->window_width_px / 39.0f)*0.5;
                        vec2 tmp_position = { x_i, y_i };
                        current_path.push(tmp_position);
                        copy_path.push_back(tmp_position);
                        result.pop();
                    }
                }
               if (!current_path.empty()) {
                   Motion& motion_ai = registry.motions.get(registry.tankings.entities[0]);
                   vec2 nodei = current_path.front();
                   float y_i = nodei.y;
                   float x_i = nodei.x;
                   current_path.pop();
                   if (!current_path.empty()) {
                   vec2 nodet = current_path.front();
                   float y_t = nodet.y;
                   float x_t = nodet.x;
                       
                   if (x_t > x_i) {
                       motion_ai.position.x += renderer->window_width_px / 39.0f;
                       motion_ai.angle = atan2(1, 0);
                   }
                   else if (x_t < x_i) {
                       motion_ai.position.x -= renderer->window_width_px / 39.0f;
                       motion_ai.angle = atan2(-1, 0);
                   }
                   else if (y_t > y_i) {
                       motion_ai.position.y += renderer->window_height_px / 22.0f;
                       motion_ai.angle = atan2(0, -1);
                   }
                   else {
                       motion_ai.position.y -= renderer->window_height_px / 22.0f;
                       motion_ai.angle = atan2(0, 1);
                   }
                   }
               }
            }
            if (debugging.in_debug_mode) {
                for (int i = 0; i < copy_path.size(); i++) {
                    vec2 posi = copy_path[i];
                    createLine({posi.x, posi.y }, { 15,15 });
                }
            }
        }
        for(uint i = 0; i< AIs.size(); i++) {
            Entity AI = AIs[i];
            vec2 position_player;
            registry.aiTanks.get(AI).last_check += elapsed_ms;
            if (i % 2 == 0 || positions.size() == 1) {
                position_player = positions[0];
            } else {
                position_player = positions[1];
            }
            if (registry.aiTanks.get(AI).last_check >= 2000.f) {
                    registry.aiTanks.get(AI).last_check -= 2000.f;
                    Motion& motion_ai = motion_registry.get(AI);
                    vec2 position_ai = motion_ai.position;
                    if (position_player.x > position_ai.x && position_player.y < position_ai.y) {
                        if (uniform_dist(rng) <= 0.5) {
                            motion_registry.get(AI).velocity.x = (renderer->window_width_px / 39.0f) * 1;
                            motion_registry.get(AI).velocity.y = 0;
                            motion_registry.get(AI).angle = atan2(1, 0);

                            Entity pebble = createAIBullet(motion_registry.get(AI).position, { 8,8 });
                            motion_registry.get(pebble).velocity = { (renderer->window_width_px / 39.0f) * 6, 0 };
                        }
                        else {
                            motion_registry.get(AI).velocity.y = -(renderer->window_height_px / 22.0f) * 1;
                            motion_registry.get(AI).velocity.x = 0;
                            motion_registry.get(AI).angle = atan2(0, 1);

                            Entity pebble = createAIBullet(motion_registry.get(AI).position, { 8,8 });
                            motion_registry.get(pebble).velocity = { 0, -(renderer->window_height_px / 22.0f) * 6 };
                        }
                        // player tank is on the left below the AI tank
                    }
                    else if (position_player.x < position_ai.x && position_player.y < position_ai.y) {
                        if (uniform_dist(rng) <= 0.5) {
                            motion_registry.get(AI).velocity.x = -(renderer->window_width_px / 39.0f) * 1;
                            motion_registry.get(AI).velocity.y = 0;
                            motion_registry.get(AI).angle = atan2(-1, 0);

                            Entity pebble = createAIBullet(motion_registry.get(AI).position, { 8,8 });
                            motion_registry.get(pebble).velocity = { -(renderer->window_width_px / 39.0f) * 6, 0 };
                        }
                        else {
                            motion_registry.get(AI).velocity.y = -(renderer->window_height_px / 22.0f) * 1;
                            motion_registry.get(AI).velocity.x = 0;
                            motion_registry.get(AI).angle = atan2(0, 1);

                            Entity pebble = createAIBullet(motion_registry.get(AI).position, { 8,8 });
                            motion_registry.get(pebble).velocity = { 0, -(renderer->window_height_px / 22.0f) * 6 };
                        }
                        // player tank is on the right above the AI tank
                    }
                    else if (position_player.x > position_ai.x && position_player.y > position_ai.y) {
                        if (uniform_dist(rng) <= 0.5) {
                            motion_registry.get(AI).velocity.x = (renderer->window_width_px / 39.0f) * 1;
                            motion_registry.get(AI).velocity.y = 0;
                            motion_registry.get(AI).angle = atan2(1, 0);

                            Entity pebble = createAIBullet(motion_registry.get(AI).position, { 8,8 });
                            motion_registry.get(pebble).velocity = { (renderer->window_width_px / 39.0f) * 6, 0 };
                        }
                        else {
                            motion_registry.get(AI).velocity.y = (renderer->window_height_px / 22.0f) * 1;
                            motion_registry.get(AI).velocity.x = 0;
                            motion_registry.get(AI).angle = atan2(0, -1);

                            Entity pebble = createAIBullet(motion_registry.get(AI).position, { 8,8 });
                            motion_registry.get(pebble).velocity = { 0, (renderer->window_height_px / 22.0f) * 6};
                        }
                        // player tank is on the left above the AI ta
                    }
                    else if (position_player.x < position_ai.x && position_player.y > position_ai.y) {
                        if (uniform_dist(rng) <= 0.5) {
                            motion_registry.get(AI).velocity.x = -(renderer->window_width_px / 39.0f) * 1;
                            motion_registry.get(AI).velocity.y = 0;
                            motion_registry.get(AI).angle = atan2(-1, 0);
                            
                            Entity pebble = createAIBullet(motion_registry.get(AI).position, { 8,8 });
                            motion_registry.get(pebble).velocity = { -(renderer->window_height_px / 22.0f) * 6, 0 };
                        }
                        else {
                            motion_registry.get(AI).velocity.y = (renderer->window_height_px / 22.0f) * 1;
                            motion_registry.get(AI).velocity.x = 0;
                            motion_registry.get(AI).angle = atan2(0, -1);
                            
                            Entity pebble = createAIBullet(motion_registry.get(AI).position, { 8,8 });
                            motion_registry.get(pebble).velocity = { 0, (renderer->window_height_px / 22.0f) * 6 };
                        }
                    }
                }
                        
            }
        }
    (void)elapsed_ms; 
}

std::stack<Node> AISystem::find_path(int path[22][39], int sx, int sy, int ex, int ey) {
        std::stack<Node> Frontier;
        Node *node = new Node();
        int i = 0, j = 0, counter;
        node->i = sx;
        node->j = sy;
        node->choice = -1;
        Frontier.push(*node);
        path[sx][sy] = -1;

        while (!Frontier.empty()) {
            *node = Frontier.top();
            if (node->i == ex && node->j == ey) {
                return Frontier;
            }
            counter = 0;
            while (node->choice < 4 && counter == 0) {
                node->choice++;
                switch (node->choice) {
                case 0: i = node->i - 1; j = node->j; break;
                case 1: i = node->i; j = node->j+1; break;
                case 2: i = node->i + 1; j = node->j; break;
                case 3: i = node->i; j = node->j -1 ; break;
                }

                if (path[i][j] == 0) {
                    counter = 1;
                }
            }
            if (counter == 1) {
                node = new Node;
                node->i = i;
                node->j = j;
                node->choice = -1;
                Frontier.push(*node);
                path[i][j] = -1;
            } else {
                Frontier.pop();
            }
        }
        return {};
    }
