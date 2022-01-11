// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include "board_system.hpp"
#include "tank_system.hpp"
#include <iostream>
#include <chrono>

using Clock = std::chrono::high_resolution_clock;

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

bool collides(const Motion& motion1, const Motion& motion2)
{
    vec2 dp = motion1.position - motion2.position;
    float dist_squared = dot(dp,dp);
    const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
    const float other_r_squared = dot(other_bonding_box, other_bonding_box);
    const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
    const float my_r_squared = dot(my_bonding_box, my_bonding_box);
    const float r_squared = max(other_r_squared, my_r_squared);
    if (dist_squared < r_squared)
        return true;
    return false;
}

// works for simple collisions in platformer, AABB
bool generalPlatformerCollision(const Motion& motion1, const Motion& motion2)
{
	const vec2 bounding_box_one = get_bounding_box(motion1);
	const vec2 bounding_box_two = get_bounding_box(motion2);

	// code referenced from https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection
	// collision x-axis?
    // this looks scuffed and that's cause it is! there's extra space in the character sprites b/c I didn't make it a textured mesh
    // this accounts for that extra sprite space that would make it look like the collision happens earlier, and all collisions we care involve character sprites
	bool collisionX = motion1.position.x + bounding_box_one.x - bounding_box_one.x / 5 >= motion2.position.x && motion2.position.x + bounding_box_two.x - bounding_box_two.x / 5 >= motion1.position.x;

	// collision y-axis?
	bool collisionY = motion1.position.y + bounding_box_one.y >= motion2.position.y && motion2.position.y + bounding_box_two.y >= motion1.position.y;
	// collision only if on both axes
	return collisionX && collisionY;
}

// checks for character-platform collisions where character is on top
bool characterLandsOnPlatform(const Motion& motion1, const Motion& motion2, vec2 prevPosOne, vec2 prevPosTwo, const Entity& entity1, const Entity& entity2)
{	
	const vec2 bounding_box_one = get_bounding_box(motion1);
	const vec2 bounding_box_two = get_bounding_box(motion2);

	// check if obj 1 was previously above obj 2 before collision
	if (prevPosOne.y + bounding_box_one.y / 2 <= prevPosTwo.y - bounding_box_two.y / 2) {
		// check if obj 1 = character, obj 2 = platform
		if (registry.platformerPhysics.has(entity1) && registry.platforms.has(entity2)) {
			return true;
		}
		
	}
	// check if obj  1 was previously below obj 2 before collision
	if (prevPosTwo.y + bounding_box_two.y / 2 <= prevPosOne.y - bounding_box_one.y / 2) {
		// check if obj 1 = platform, obj 2 = character
		if (registry.platformerPhysics.has(entity2) && registry.platforms.has(entity1)) {
			return true;
		}
		
	}
	// check if obj 1 was to the left of obj 2 before collision
	else if (prevPosOne.x + bounding_box_one.x / 2 < prevPosTwo.x - bounding_box_two.x / 2) {
		return false;
	}
	// check if obj 1 was to the right of obj 2 before collision
	else if (prevPosTwo.x + bounding_box_two.x / 2 < prevPosOne.x - bounding_box_one.x / 2) {
		return false;
	}

	// currently colliding
	return false;
}

void PhysicsSystem::init(void* Board, RenderSystem* renderer, int window_width_px, int window_height_px){
	this->boardSystem = Board;
    tank_hit_sound = Mix_LoadWAV(audio_path("tank_hit.wav").c_str());
    wall_collapsed_sound = Mix_LoadWAV(audio_path("wall_collapsed.wav").c_str());
    tank_on_fire_sound = Mix_LoadWAV(audio_path("tank_on_fire.wav").c_str());
    if (tank_hit_sound == nullptr || wall_collapsed_sound == nullptr) {
        fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n %s\n %s\n %s\n %s \n make sure the data directory is present",
        audio_path("tank_hit.wav").c_str(),
        audio_path("wall_collapsed.wav").c_str(),
        audio_path("tank_on_fire.wav").c_str());
    }
    this->window_width_px = window_width_px;
    this->window_height_px = window_height_px;
    this->renderer = renderer;
}

void PhysicsSystem::sphere_collision(Entity entity_i, Entity entity_j)  {
    Motion& motion_i = registry.motions.get(entity_i);
    Motion& motion_j = registry.motions.get(entity_j);
    float radius_i = motion_i.scale.x;
    float radius_j = motion_j.scale.x;
    vec2 position_i = motion_i.position;
    vec2 position_j = motion_j.position;
    vec2 velocity_i = motion_i.velocity;
    vec2 velocity_j = motion_j.velocity;
    float distance = sqrt(pow(abs(position_i.x - position_j.x),2) + pow(abs(position_i.y - position_j.y),2));
    if (distance <= radius_i + radius_j) {
        vec2 delta_velocity_i = velocity_i - velocity_j;
        vec2 delta_position_i = position_i - position_j;
        vec2 delta_velocity_j = velocity_j - velocity_i;
        vec2 delta_position_j = position_j - position_i;
        float mass_i = registry.physics.get(entity_i).mass;
        float mass_j = registry.physics.get(entity_j).mass;
        float factor_i = (2 * mass_j) / (mass_i + mass_j) / 5;
        float factor_j = (2 * mass_i) / (mass_i + mass_j) / 5;
        float denominator_i = sqrt(pow(delta_position_i.x, 2) + pow(delta_position_i.y, 2));
        float denominator_j = sqrt(pow(delta_position_j.x, 2) + pow(delta_position_j.y, 2));
        motion_i.velocity -= (factor_i * (delta_velocity_i * delta_position_i) * delta_position_i) / denominator_i;
        motion_j.velocity -= (factor_j * (delta_velocity_j * delta_position_j) * delta_position_j) / denominator_j;
    }
}

void PhysicsSystem::step(float elapsed_ms, float window_width_px, float window_height_px)
{
	auto& motion_registry = registry.motions;
	float step_seconds = 1.0f * (elapsed_ms / 1000.f);
	// for platformer
	float gravity = (float)window_height_px / 80;
	Camera& camera = registry.camera.components[0];
	Platformer& p = registry.platformer.get(board_controller);
    TankGame& t = registry.tankGame.get(board_controller);
	GameBoard& b = registry.gameBoard.get(board_controller);
	for(uint i = 0; i< motion_registry.size(); i++)
	{
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];

		motion.position += motion.velocity * step_seconds;
		// camera following entity target if exists ONLY IF on board game
		if(camera.target != 0 && camera.target == entity && b.active){
			camera.x = -2.0f * motion.position.x / window_width_px + 0.3f;
			((BoardSystem*)boardSystem)->scoreboardMovementUpdate(((BoardSystem*)boardSystem)->cameraPositionX(camera.x, window_width_px));
		}
		// platformer specific code
		if (p.active && p.loaded) {
			if (registry.platformerPhysics.has(entity)) {

				// prevent fall if at bottom of screen
				if (motion.position.y + abs(motion.scale.y) / 2 >= (float)window_height_px) {
					motion.velocity.y = 0;
				}
				else {
					// make players fall (gravity)
					motion.velocity.y += gravity;
                    registry.platformerPhysics.get(entity).onPlatform = false;
				}
			}
		}
		
	}

	// update player walking game board TODO move somewhere else?
	// TODO find active player ,check if in walking stage, animate like in world system
	GameBoard& g = registry.gameBoard.get(board_controller);
	if(g.stage == 13){
		Entity p = *(&std::get<0>(g.player_positions[g.turn]));
		// Motion& motion = registry.motions.get(p);
		Sprite& sprite = registry.sprites.get(p);
		sprite.time_last_update += elapsed_ms;
		// switch frames regardless of time if it is a direction change
		if (sprite.frame.y == 1) {
			sprite.time_last_update = 0;
			sprite.frame.x = fmod(sprite.frame.x + 1, (float)sprite.num_frames);
			sprite.frame.y = 0;
		}else if (sprite.time_last_update >= sprite.animation_speed) {
			sprite.time_last_update = 0;
			sprite.frame.x = fmod(sprite.frame.x + 1, (float)sprite.num_frames);
			sprite.frame.y = 0;
		}

		// registry.sprites.get(*(&std::get<0>(g.player_positions[g.turn]))).animation_speed = 1;
	}
	

	// Update dice
	ComponentContainer<GameDie>& dice_container = registry.dice;
	float angleDampener = (M_PI / 180.0f) * 2;
	vec2 velocityDampener = vec2(10.0f);
	for (uint i = 0; i < dice_container.components.size(); i++)
	{
		GameDie& dice = dice_container.components[i];
		if (dice.rolling == true && registry.motions.has(dice_container.entities.at(i))) {
			Motion& motion_d = registry.motions.get(dice_container.entities.at(i));
			
			if (glm::length(velocityDampener) > glm::length(dice.velocity) && angleDampener > dice.angular_velocity) {
				// stop here
				dice.rolling = false;
                if(dice.value > 4){
                    dice.boom = true;    
                }
				dice.angular_velocity = 0.0f;
				dice.velocity = vec2(0.0f);
			}
			else {
				motion_d.angle += dice.angular_velocity*elapsed_ms;

                // vec2 d_bbox = get_bounding_box(motion_d);
                // float y0 = motion_d.position.y - (d_bbox.y / 2.0f);
                // float y1 = motion_d.position.y + (d_bbox.y / 2.0f);
                // float x0 = motion_d.position.x - (d_bbox.x / 2.0f);
                // float x1 = motion_d.position.x + (d_bbox.x / 2.0f);
                
				motion_d.position += elapsed_ms*dice.velocity/100.0f;

                // if(x0 <= 0.0 || x1 <= 0.0 || x0 >= window_width_px || x1 >= window_width_px){
                //     motion_d.velocity.x = -1.0f * motion_d.velocity.x;
                //     if(x0 >= window_width_px || x1 >= window_width_px){
                //         motion_d.position.x = window_width_px - d_bbox.x;
                //     }else{
                //         motion_d.position.x = 0.0f + d_bbox.x;
                //     }
                    
                // }

                // if(y0 <= 0.0 || y1 <= 0.0 || y0 >= window_height_px || y1 >= window_height_px){
                //     motion_d.velocity.y = -1.0f * motion_d.velocity.y;
                //     if(y0 >= window_height_px || y1 >= window_height_px){
                //         motion_d.position.y = window_height_px - d_bbox.y;
                //     }else{
                //         motion_d.position.y = 0.0f + d_bbox.y;
                //     }
                // }

				if (dice.angular_velocity >= 0.0f) {
					 dice.angular_velocity = max(0.0f, dice.angular_velocity - angleDampener);
				}
				else {
					dice.angular_velocity = min(0.0f, dice.angular_velocity + angleDampener);
				}
				
				if (dice.velocity.x >= 0) {
					dice.velocity.x = max(0.0f, dice.velocity.x - velocityDampener.x);
				}
				else {
					dice.velocity.x = min(0.0f, dice.velocity.x + velocityDampener.x);
				}
				if (dice.velocity.y >= 0) {
					dice.velocity.y = max(0.0f, dice.velocity.y - velocityDampener.y);
				}
				else {
					dice.velocity.y = min(0.0f, dice.velocity.y + velocityDampener.y);
				}
			}
			
		}else if (dice.rolling == false && dice.boom == true && registry.motions.has(dice_container.entities.at(i))){
           Motion& motion_d = registry.motions.get(dice_container.entities.at(i));
           motion_d.position += (motion_d.velocity + vec2(0, 50.0*gravity)) * step_seconds;
        }
	}

	// Check for collisions between all moving entities
    ComponentContainer<Motion> &motion_container = registry.motions;
	for(uint i = 0; i<motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
		for(uint j = 0; j<motion_container.components.size(); j++) // i+1
		{
			if (i == j)
				continue;
            
			Motion& motion_j = motion_container.components[j];
			// platformer specific code
			if (p.loaded && p.active)
			{
				Entity entity_j = motion_container.entities[j];
				if (generalPlatformerCollision(motion_i, motion_j)) {
					
					// Create a collisions event
					// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity

					// check previous position of motion i and j to determine direction the collision came from
					// velocity changed if entity = player so adjust that
					vec2 previousPositionI;
					vec2 previousPositionJ;
					if (registry.platformerPhysics.has(entity_i)) {
						previousPositionI = motion_i.position - (motion_i.velocity - vec2(0, gravity)) * step_seconds;
					}
					else {
						previousPositionI = motion_i.position - motion_i.velocity * step_seconds;
					}
					if (registry.platformerPhysics.has(entity_j)) {
						previousPositionJ = motion_j.position - (motion_j.velocity - vec2(0, gravity)) * step_seconds;
					}
					else {
						previousPositionJ = motion_j.position - motion_j.velocity * step_seconds;
					}
					// only add to registry if it's a collision we're interested in!
					// we want collisions where characters (those affected by physics) are landing on the platform/are currently on platform
					// or if character has reached goal / hit a trap
					bool characterAbovePlatform = characterLandsOnPlatform(motion_i, motion_j, previousPositionI, previousPositionJ, entity_i, entity_j);
					bool characterGoalCollision = (registry.goals.has(entity_i) && registry.platformerPhysics.has(entity_j)) || (registry.goals.has(entity_j) && registry.platformerPhysics.has(entity_i));
                    bool characterTrapCollision = (registry.traps.has(entity_i) && registry.platformerPhysics.has(entity_j)) || (registry.traps.has(entity_j) && registry.platformerPhysics.has(entity_i));
					if (characterAbovePlatform || characterGoalCollision || characterTrapCollision) {
						// place collision with character entity first
						if (registry.platformerPhysics.has(entity_i)) {
							registry.collisions.emplace_with_duplicates(entity_i, entity_j);
                            registry.platformerPhysics.get(entity_i).onPlatform = true;
						}
						else {
                            registry.platformerPhysics.get(entity_j).onPlatform = true;
							registry.collisions.emplace_with_duplicates(entity_j, entity_i);
						}
						
					}	
				}
			}
			// potential section for tank code ?
		}
	}

	// handle character - wall collisions for platformer
	if (p.loaded && p.active) {
		auto& platformerPhysicsRegistry = registry.platformerPhysics;
		for (uint i = 0; i < platformerPhysicsRegistry.components.size(); i++) {
			Entity character = platformerPhysicsRegistry.entities[i];
			Motion& motion = motion_registry.get(character);

			// check if sprite goes past wall 
			//(slightly off-centred for x because of extra transparent space in sprite img)
			float spriteDifferenceX = abs(motion.scale.x) / 4;
			float spriteDifferenceY = abs(motion.scale.y) / 2;

			// left side
			if (motion.position.x - spriteDifferenceX < 0.f) {
				motion.position.x = spriteDifferenceX;
			}
			// right side
			else if (motion.position.x + spriteDifferenceX > (float)window_width_px) {
				motion.position.x = (float)window_width_px - spriteDifferenceX;
			}
			// top
			else if (motion.position.y - spriteDifferenceY < 0.f) {
				motion.position.y = spriteDifferenceY;
			}
			// bottom 
			else if (motion.position.y + spriteDifferenceY > (float)window_height_px) {
				motion.position.y = (float)window_height_px - spriteDifferenceY;
			}

		}
	}
    if (t.loaded && t.active) {
        ComponentContainer<Motion> &motion_container = registry.motions;
            for(uint i = 0; i<motion_container.components.size(); i++)
            {
                Motion& motion_i = motion_container.components[i];
                Entity entity_i = motion_container.entities[i];
                for(uint j = 0; j<motion_container.components.size(); j++) // i+1
                {
                    if (i == j)
                        continue;

                    Motion& motion_j = motion_container.components[j];
                    if (collides(motion_i, motion_j))
                    {
                        Entity entity_j = motion_container.entities[j];
                        // Create a collisions event
                        // We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
                        registry.collisions.emplace_with_duplicates(entity_i, entity_j);
                        registry.collisions.emplace_with_duplicates(entity_j, entity_i);
                    }
                }
            }
        auto& collisionsRegistry = registry.collisions;
            for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
                // The entity and its collider
                Entity entity = collisionsRegistry.entities[i];
                Entity entity_other = collisionsRegistry.components[i].other;

                if (registry.playerTanks.has(entity)) {
                    //Player& player = registry.players.get(entity);

                    // Checking Player - AI collisions
                    if (registry.aiTanks.has(entity_other)) {
                        sphere_collision(entity, entity_other);
                        
                        // Checking Player - Player collisions
                    } else if (registry.playerTanks.has(entity_other)) {
                        registry.motions.get(entity_other).velocity = {0,0};
                        registry.motions.get(entity).velocity = {0,0};

                       // Checking Player - wall collisions
                    } else if (registry.walls.has(entity_other)) {
                        Motion& motion = registry.motions.get(entity);
                            Motion& otherMotion = registry.motions.get(entity_other);
                            
                            if (motion.velocity.x > 0) {
                             motion.velocity.x = 0;
                             motion.position.x = otherMotion.position.x + motion.scale.x + 10;
                            }
                            else if (motion.velocity.x < 0) {
                             motion.velocity.x = 0;
                             motion.position.x = otherMotion.position.x - motion.scale.x - 10;
                            }
                            else if (motion.velocity.y < 0) {
                             motion.velocity.y = 0;
                             motion.position.y = otherMotion.position.y + motion.scale.y + 20;
                            }
                            else if (motion.velocity.y > 0) {
                             motion.velocity.y = 0;
                             motion.position.y = otherMotion.position.y - motion.scale.y - 20;
                            }
                    } else if (registry.hardWall.has(entity_other)) {
                        Motion& motion = registry.motions.get(entity);
                        Motion& otherMotion = registry.motions.get(entity_other);
                        
                        if (motion.velocity.x > 0) {
                            motion.velocity.x = 0;
                            motion.position.x = otherMotion.position.x + motion.scale.x + 10;
                        }
                        else if (motion.velocity.x < 0) {
                            motion.velocity.x = 0;
                            motion.position.x = otherMotion.position.x - motion.scale.x - 10;
                        }
                        else if (motion.velocity.y < 0) {
                            motion.velocity.y = 0;
                            motion.position.y = otherMotion.position.y + motion.scale.y + 20;
                        }
                        else if (motion.velocity.y > 0) {
                            motion.velocity.y = 0;
                            motion.position.y = otherMotion.position.y - motion.scale.y - 20;
                        }
                    }
                    
                    // Checking AI - wall collisions
                } else if (registry.aiTanks.has(entity)) {
                    if (registry.walls.has(entity_other)) {
                       
                        Motion& motion = registry.motions.get(entity);
                        Motion& otherMotion = registry.motions.get(entity_other);
                       
                        entity_other.set_last_check(entity_other.get_last_check() + 20.f);
                        if (entity_other.get_last_check() >= 20.f) {
                            registry.remove_all_components_of(entity_other);
                        }
                        registry.motions.get(entity).velocity = {0,0};
                        entity.set_last_check(20.f);
                        
                        // Handling AI - bullets collision
                    } else if (registry.hardWall.has(entity_other)) {
                        Motion& motion = registry.motions.get(entity);
                        Motion& otherMotion = registry.motions.get(entity_other);
                        // stop descent, set position to above platform
                        if (motion.velocity.x > 0) {
                            motion.velocity.x = 0;
                            motion.position.x = otherMotion.position.x + motion.scale.x;
                        }
                        else if (motion.velocity.x < 0) {
                            motion.velocity.x = 0;
                            motion.position.x = otherMotion.position.x - motion.scale.x;
                        }
                        else if (motion.velocity.y < 0) {
                            motion.velocity.y = 0;
                            motion.position.y = otherMotion.position.y + motion.scale.y + 20;
                        }
                        else if (motion.velocity.y > 0) {
                            motion.velocity.y = 0;
                            motion.position.y = otherMotion.position.y - motion.scale.y - 20;
                        }
                        
                        // Handling AI - AI collision
                    } else if (registry.aiTanks.has(entity_other)) {
                        Motion& motion = registry.motions.get(entity);
                        Motion& otherMotion = registry.motions.get(entity_other);
                        
                        if (motion.velocity.x > 0) {
                            motion.velocity.x = 0;
                            motion.position.x = otherMotion.position.x + motion.scale.x;
                        }
                        else if (motion.velocity.x < 0) {
                            motion.velocity.x = 0;
                            motion.position.x = otherMotion.position.x - motion.scale.x;
                        }
                        else if (motion.velocity.y < 0) {
                            motion.velocity.y = 0;
                            motion.position.y = otherMotion.position.y + motion.scale.y + 20;
                        }
                        else if (motion.velocity.y > 0) {
                            motion.velocity.y = 0;
                            motion.position.y = otherMotion.position.y - motion.scale.y - 20;
                        }
                    }

                    // Handling AI bullets - wall collisions
                } else if (registry.aiBullets.has(entity)) {
                    if(registry.walls.has(entity_other)) {
                        float remaining_health = registry.walls.get(entity_other).health - registry.aiBullets.get(entity).damage;
                        registry.walls.get(entity_other).health = remaining_health;
                        if (registry.walls.get(entity_other).health <= 0) {
                            registry.remove_all_components_of(entity_other);
                        } else if (registry.walls.get(entity_other).health <= 100) {
                            Entity broken_wall = createBrokenWall(renderer, registry.motions.get(entity_other).position);
                            registry.remove_all_components_of(entity_other);
                            registry.walls.get(broken_wall).health = remaining_health;
                        }
                        registry.remove_all_components_of(entity);
                        
                    // handling AI bullets - player collision
                    } else if (registry.playerTanks.has(entity_other)) {
                        Mix_PlayChannel(-1, tank_hit_sound, 0);
                        registry.playerTanks.get(entity_other).health -= registry.aiBullets.get(entity).damage;
                        if (registry.playerTanks.get(entity_other).health <= 0) {
                            if (t.level == 1 && (registry.playerTanks.size() == 0)) {
                                    if (registry.tankGamePoints.components[0].points > registry.tankGamePoints.components[1].points) {
                                        g.player_points[0] += 10;
                                        g.player_points[2] += 3;
                                    } else if (registry.tankGamePoints.components[0].points < registry.tankGamePoints.components[1].points){
                                        g.player_points[1] += 10;
                                        g.player_points[2] += 3;
                                    } else {
                                        g.player_points[0] += 5;
                                        g.player_points[1] += 5;
                                    }
                            } else if (t.level == 2) {
                                float player1_point = registry.tankGamePoints.components[0].points;
                                float player2_point = registry.tankGamePoints.components[1].points;
                                g.player_points[0] += floor(player1_point / 200);
                                g.player_points[1] += floor(player2_point / 200);
                            }
                                registry.remove_all_components_of(entity_other);
                                if (registry.playerTanks.size() == 0) {
                                    t.active = false;
                                }
                        }
                        registry.remove_all_components_of(entity);
                    }
                    
                    // handle AI bullet and hard wall collision
                    else if (registry.hardWall.has(entity_other)) {
                        registry.hardWall.get(entity_other).health -= registry.aiBullets.get(entity).damage;
                        if (registry.hardWall.get(entity_other).health <= 0) {
                            registry.remove_all_components_of(entity_other);
                        }
                        registry.remove_all_components_of(entity);
                    }
                    
                    // Handling player bullets - AI tanks collision
                } else if (registry.playerBullets.has(entity)) {
                    if (registry.aiTanks.has(entity_other)) {
                        Mix_PlayChannel(-1, wall_collapsed_sound, 0);
                        registry.aiTanks.get(entity_other).health -= registry.playerBullets.get(entity).damage;
                        if (registry.aiTanks.get(entity_other).health <= 0) {
                            registry.remove_all_components_of(entity_other);
                            if (registry.playerBullets.get(entity).shot_by == 1) {
                                registry.tankGamePoints.components[0].points += 100;
                            } else if (registry.playerBullets.get(entity).shot_by == 2) {
                                registry.tankGamePoints.components[1].points += 100;
                            }
                        }
                        registry.remove_all_components_of(entity);
                        
                    // Handling player bullets - tankings collision
                    } else if (registry.tankings.has(entity_other) && t.level == 2) {
                        Mix_PlayChannel(-1, wall_collapsed_sound, 0);
                        registry.tankings.get(entity_other).health -= registry.playerBullets.get(entity).damage;
                        if (registry.tankings.get(entity_other).health <= 0) {
                            registry.remove_all_components_of(entity_other);
                            if (g.active_item[0] == TEXTURE_ASSET_ID::INV_LIGHTENING) {
                                g.player_points[0] += 16;
                                g.active_item[0] = TEXTURE_ASSET_ID::BOARD_HELP;
                            } else {
                                g.player_points[0] += 8;
                            }
                            if (g.active_item[1] == TEXTURE_ASSET_ID::INV_LIGHTENING) {
                                g.player_points[1] += 16;
                                g.active_item[1] = TEXTURE_ASSET_ID::BOARD_HELP;
                            } else {
                                g.player_points[1] += 8;
                            }
                            float player1_point = registry.tankGamePoints.components[0].points;
                            float player2_point = registry.tankGamePoints.components[1].points;
                            g.player_points[0] += floor(player1_point / 200);
                            g.player_points[1] += floor(player2_point / 200);
                            t.active = false;
                        }
                        registry.remove_all_components_of(entity);
                        
                        // Handling player bullets - wall collision
                    } else if (registry.walls.has(entity_other)) {
                        Mix_PlayChannel(-1, wall_collapsed_sound, 0);
                        float remaining_health = registry.walls.get(entity_other).health - registry.playerBullets.get(entity).damage;
                        registry.walls.get(entity_other).health = remaining_health;
                        if (registry.walls.get(entity_other).health <= 0) {
                            registry.remove_all_components_of(entity_other);
                        } else if (registry.walls.get(entity_other).health <= 100) {
                            Entity broken_wall = createBrokenWall(renderer, registry.motions.get(entity_other).position);
                            registry.walls.get(broken_wall).health = remaining_health;
                            registry.remove_all_components_of(entity_other);
                        }
                        registry.remove_all_components_of(entity);
                        
                        // Handling player bullets - player tank collision
                    } else if (registry.playerTanks.has(entity_other) && t.level == 1) {
                        if (registry.playerTanks.get(entity_other).id != registry.playerBullets.get(entity).shot_by) {
                            Mix_PlayChannel(-1, tank_hit_sound, 0);
                            registry.playerTanks.get(entity_other).health -= registry.playerBullets.get(entity).damage;
                            if (registry.playerTanks.get(entity_other).health <= 0) {
                                std::cout << registry.playerTanks.get(entity_other).health;
                                if (!registry.deathTimers.has(entity_other)) {
                                    registry.deathTimers.emplace(entity_other);
                                    registry.motions.get(entity_other).angle = 3.1415f;
                                    registry.motions.get(entity_other).velocity = { 0, (renderer->window_height_px/22.f)*1.6};
                                    registry.colors.get(entity_other) = {1,0,0};
                                    registry.remove_all_components_of(entity_other);
                                    
                                    if (registry.playerBullets.get(entity).shot_by == 1) {
                                        registry.tankGamePoints.components[0].points += 500;
                                    } else if (registry.playerBullets.get(entity).shot_by == 2) {
                                        registry.tankGamePoints.components[1].points += 500;
                                    }
                                    
                                    // The tank game will not end here. The game will continue with one player
                                    if (t.level == 1 && registry.playerTanks.size() == 0) {
                                        if (registry.tankGamePoints.components[0].points > registry.tankGamePoints.components[1].points) {
                                            g.player_points[0] += 10;
                                            g.player_points[2] += 3;
                                        } else if (registry.tankGamePoints.components[0].points < registry.tankGamePoints.components[1].points){
                                            g.player_points[1] += 10;
                                            g.player_points[3] += 3;
                                        } else {
                                            g.player_points[0] += 5;
                                            g.player_points[1] += 5;
                                        }
                                    }
                                    
                                    if (registry.playerTanks.size() == 0) {
                                        t.active = false;
                                    }
                                }
                            }
                            registry.remove_all_components_of(entity);
                        }
                    
                        // Handling player bullets - hard wall collision
                    } else if (registry.hardWall.has(entity_other)){
                        registry.hardWall.get(entity_other).health -= registry.playerBullets.get(entity).damage;
                        if (registry.hardWall.get(entity_other).health <= 0) {
                            registry.remove_all_components_of(entity_other);
                        }
                        registry.remove_all_components_of(entity);
                    }
                    
                        // Handling player bullets - base collision
                    else if (registry.tankHome.has(entity_other)) {
                        registry.tankHome.get(entity_other).health -= registry.playerBullets.get(entity).damage;
                        if (registry.tankHome.get(entity_other).health <= 0) {
                            registry.remove_all_components_of(entity_other);
                        }
                        registry.remove_all_components_of(entity);
                    }
                    
                        // Handling player bullets - AI bullets wall collision
                    else if (registry.aiBullets.has(entity_other)) {
                        sphere_collision(entity, entity_other);
                    }
                    
                        // Handling player bullets - player bullets wall collision
                    else if (registry.playerBullets.has(entity_other)) {
                        if (registry.playerBullets.get(entity).shot_by != registry.playerBullets.get(entity_other).shot_by) {
                            sphere_collision(entity, entity_other);
                        }
                    }
                    
                    // Handling tanking - wall collision
                } else if (registry.tankings.has(entity)) {
                    if (registry.walls.has(entity_other)) {
                        registry.motions.get(entity).velocity = {0,0};
                        entity.set_last_check(20.f);
                        
                        // Handing tanking - base collision
                    } else if (registry.tankHome.has(entity_other)) {
                        float player1_point = registry.tankGamePoints.components[0].points;
                        float player2_point = registry.tankGamePoints.components[1].points;
                        g.player_points[0] += floor(player1_point / 200);
                        g.player_points[1] += floor(player2_point / 200);
                        t.active = false;
                        
                        // Handling tanking - player collision
                    } else if (registry.playerTanks.has(entity_other)) {
                        Mix_PlayChannel(-1, tank_on_fire_sound, 0);
                        registry.playerTanks.get(entity_other).health -= 0.2;
                        if (registry.playerTanks.get(entity_other).health <= 0) {
                            registry.remove_all_components_of(entity_other);
                            if (registry.playerTanks.size() == 0) {
                                float player1_point = registry.tankGamePoints.components[0].points;
                                float player2_point = registry.tankGamePoints.components[1].points;
                                g.player_points[0] += floor(player1_point / 200);
                                g.player_points[1] += floor(player2_point / 200);
                                t.active = false;
                            }
                        }
                    }
                }
            }
        
        if (debugging.in_debug_mode) {
                uint size_before_adding_new = (uint)motion_container.components.size();
                for (uint i = 0; i < size_before_adding_new; i++)
                {
                    Motion& motion_i = motion_container.components[i];
                    Entity entity_i = motion_container.entities[i];
                    if (!registry.tankHome.has(entity_i)) {
                        if (registry.playerTanks.has(entity_i)) {
                            if ((motion_i.angle >= 3.14 && motion_i.angle <=  3.15) || motion_i.angle == 0) {
                                const vec2 bonding_box = get_bounding_box(motion_i);
                                float radius = sqrt(dot(bonding_box / 2.f, bonding_box / 2.f));
                                vec2 line_scale1 = { motion_i.scale.x / 20, abs(motion_i.scale.y) };
                                vec2 right_pos = { motion_i.position.x + abs(motion_i.scale.x / 2), motion_i.position.y };
                                vec2 left_pos = { motion_i.position.x - abs(motion_i.scale.x / 2), motion_i.position.y };
                                vec2 up_pos = { motion_i.position.x, motion_i.position.y - abs(motion_i.scale.y / 2) };
                                vec2 down_pos = { motion_i.position.x, motion_i.position.y + abs(motion_i.scale.y / 2) };
                                createLine(right_pos, line_scale1);
                                createLine(left_pos, line_scale1);
                                vec2 line_scale2 = { 2 * radius - 20, motion_i.scale.x / 20 };
                                createLine(up_pos, line_scale2);
                                createLine(down_pos, line_scale2);
                            }
                            else if ((motion_i.angle >= 1.57 && motion_i.angle <= 1.58) || (motion_i.angle >= -1.58 && motion_i.angle <= -1.57)) {
                                const vec2 bonding_box = get_bounding_box(motion_i);
                                float radius = sqrt(dot(bonding_box / 2.f, bonding_box / 2.f));
                                vec2 line_scale1 = { motion_i.scale.x / 20, 2 * radius - 20};
                                vec2 right_pos = { motion_i.position.x + abs(motion_i.scale.y / 2), motion_i.position.y };
                                vec2 left_pos = { motion_i.position.x - abs(motion_i.scale.y / 2), motion_i.position.y };
                                vec2 up_pos = { motion_i.position.x, motion_i.position.y - abs(motion_i.scale.x / 2) };
                                vec2 down_pos = { motion_i.position.x, motion_i.position.y + abs(motion_i.scale.x / 2) };
                                vec2 line_scale2 = { abs(motion_i.scale.y), motion_i.scale.x / 20 };
                                createLine(right_pos, line_scale1);
                                createLine(left_pos, line_scale1);
                                createLine(up_pos, line_scale2);
                                createLine(down_pos, line_scale2);
                            }
                        }
                        else {
                            const vec2 bonding_box = get_bounding_box(motion_i);
                            float radius = sqrt(dot(bonding_box / 2.f, bonding_box / 2.f));
                            vec2 line_scale1 = { motion_i.scale.x / 20, abs(motion_i.scale.y) };
                            vec2 right_pos = { motion_i.position.x + abs(motion_i.scale.x / 2), motion_i.position.y };
                            vec2 left_pos = { motion_i.position.x - abs(motion_i.scale.x / 2), motion_i.position.y };
                            vec2 up_pos = { motion_i.position.x, motion_i.position.y - abs(motion_i.scale.y / 2) };
                            vec2 down_pos = { motion_i.position.x, motion_i.position.y + abs(motion_i.scale.y / 2) };
                            createLine(right_pos, line_scale1);
                            createLine(left_pos, line_scale1);
                            vec2 line_scale2 = { 2 * radius - 20, motion_i.scale.x / 20 };
                            createLine(up_pos, line_scale2);
                            createLine(down_pos, line_scale2);
                        }
                    }
                    else {
                        const vec2 bonding_box = get_bounding_box(motion_i);
                        float radius = sqrt(dot(bonding_box / 2.f, bonding_box / 2.f));
                        vec2 line_scale1 = { motion_i.scale.x / 60, abs(motion_i.scale.y) };
                        vec2 line_scale2 = { 2 * radius - 40, motion_i.scale.x / 60 };
                        vec2 right_pos = { motion_i.position.x + abs(motion_i.scale.x / 2), motion_i.position.y };
                        vec2 left_pos = { motion_i.position.x - abs(motion_i.scale.x / 2), motion_i.position.y };
                        vec2 up_pos = { motion_i.position.x, motion_i.position.y - abs(motion_i.scale.y / 2) };
                        vec2 down_pos = { motion_i.position.x, motion_i.position.y + abs(motion_i.scale.y / 2) };
                        createLine(right_pos, line_scale1);
                        createLine(left_pos, line_scale1);
                        createLine(up_pos, line_scale2);
                        createLine(down_pos, line_scale2);
                    }
                }
            }
        }
}
