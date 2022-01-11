#pragma once

#include <vector>
#include "tiny_ecs_registry.hpp"
#include "tiny_ecs.hpp"
#include "common.hpp"
#include "components.hpp"
#include <chrono>
#include <random>

class platformerAI
{
public:
	void step(float elapsed_ms);
};

class BTNode {
public:
	virtual void init(Entity e) {};
	virtual BTPlayerState process(Entity e) = 0;
	virtual void destroy() = 0;
};


// player locates nearest platform above it and moves towards it in the x direction,
// when within jumping distance prepare to jump towards goal
class TargetNearestAbovePlatform : public BTNode {
private:
    // private class members here (eg children nodes)
	vec2 destination_pos;
	bool ready;
	bool gotoGoal;
	float horizontal_speed;
	float vertical_speed;
	float gravity;
	int width;
	int height;
	std::default_random_engine rng;
	
	vec2 goal_pos = vec2(0.0f);
	Entity findNextPlatform(Entity e) {
		
		Motion& self_m = registry.motions.get(e);
		Entity target = e;
		std::vector<Entity> targets;
		
		int size = registry.platforms.entities.size();
		int len = 0;
		for (int i = 0; i < size; i++)
		{
			Entity entity = registry.platforms.entities[i];
			Motion& m = registry.motions.get(entity);
			// find nearest platform above me
			float t = abs(-vertical_speed / -gravity);
			// peakY doesn't work out (due to pixels rather than real units?)
			float peakY = height/ 8.f;//-vertical_speed * t - 0.5f * gravity * pow(t, 2.0f) - abs(self_m.scale.x) / 2.0f - abs(m.scale.x) / 2.0f;
			if( abs(m.position.y - self_m.position.y) < abs(peakY) && (self_m.position.y + self_m.scale.y * 0.5f) > (m.position.y - m.scale.y * 0.5f) ){ 
				// just consider platforms one jump's worth of y distance away from player ,ignore platforms way above player
				len++;
				targets.push_back(entity);
			}
		}
		if(len == 0){
			// can't find a jump target, going to goal
			gotoGoal = true;
		}else{
			// choose a target from the list of acceptable targets randomly
			int choice = std::uniform_int_distribution<int>{0,len-1}(rng);
			target = targets.at(choice);
			destination = target;
			destination_pos = registry.motions.get(target).position;

		}
		ready = true;
		
		return target;
	}
public:
	Entity destination;
	void init(Entity e) override {
		ready = false;
		gotoGoal = false;
		goal_pos = registry.motions.get(registry.goals.entities[0]).position;
		
	}
	TargetNearestAbovePlatform(float horizontal_speed, float vertical_speed, float gravity, int width,int height) {
		this->horizontal_speed = horizontal_speed;
		this->vertical_speed = vertical_speed;
		this->gravity = gravity;
		this->width = width;
		this->height = height;
		rng = std::default_random_engine(std::random_device()());
		rng.seed(std::chrono::system_clock::now().time_since_epoch().count());
	}

	BTPlayerState process(Entity e) override {
		if (!ready && registry.motions.get(e).velocity.y == 0.0f && registry.platformerPhysics.get(e).onPlatform){
			findNextPlatform(e);
			ready = true;
		}
		Motion& self_m = registry.motions.get(e);

		if(gotoGoal){
				return BTPlayerState::Goal_in_sight;
		}

		if(!ready){
			return  BTPlayerState::Running;
		}
		// below platform cases
		if((self_m.position.y + abs(self_m.scale.y * 0.5f)) > (destination_pos.y - abs(registry.motions.get(destination).scale.y  * 0.5f)) ){
			// check if within jumping distance, stop and prepare to jump
			float t1 = abs(-vertical_speed / -gravity);
			// peakY doesn't work out (due to pixels rather than real units?)
			float peakY = height/ 8.f * 1.5;//-vertical_speed * t1 - 0.5f * gravity * pow(t1, 2.0f) - abs(self_m.scale.x)/2.0f - abs(registry.motions.get(destination).scale.x) / 2.0f; 
			float rootA = abs((-vertical_speed + sqrt(pow(vertical_speed,2.0f) - (2.0f * gravity * (peakY)))) / gravity);
			float rootB = abs((-vertical_speed - sqrt(pow(vertical_speed,2.0f) - (2.0f * gravity * (peakY)))) / gravity);
			float t2 = max(rootA,rootB);
			// peakX doesn't work out (due to pixels rather than real units?)
			float peak_x = width/8.f * 1.5; //(horizontal_speed * t2) / 70.0f; // the extra 100 divisor is because the units don't seem to work out?
			
			if(abs((self_m.position.y + (registry.motions.get(e).scale.y / 2.0f)) - ((destination_pos.y ) - (registry.motions.get(destination).scale.y  / 2.0f))) > (height / 8.f)){
				// too far away from target (must have fallen, retarget on next iteration)
				ready = false;
				return BTPlayerState::Running;
			}

			if(abs(self_m.position.x - destination_pos.x) < peak_x && abs((self_m.position.y + (registry.motions.get(e).scale.y / 2.0f)) - ((destination_pos.y ) - (registry.motions.get(destination).scale.y  / 2.0f))) < peakY && self_m.velocity.y == 0.0f && registry.platformerPhysics.get(e).onPlatform){
				// Within jump range
				return BTPlayerState::In_jump_range;
			} else if (abs(self_m.position.x - destination_pos.x) > (0.75 * (registry.motions.get(destination).scale.x ))) {
				// Running to goal
				// could add a way to figure out a fast way to determine when to jump if path to target has gaps, might make AI too hard
				float dir = (self_m.position.x - destination_pos.x) > 0.0f ? -1.0f : 1.0f;
				self_m.velocity.x = horizontal_speed * dir;
			}else{
				self_m.velocity.x = 0.0f;
			}
			
		}else if (abs(self_m.position.x - destination_pos.x) > (0.1 * (registry.motions.get(destination).scale.x ))){  // above target cases:
			// Midair adjust to target
			float dir = self_m.position.x - destination_pos.x > 0.0f ? -1.0f : 1.0f;
			self_m.velocity.x = horizontal_speed * dir;
		}else if (abs(self_m.position.x - destination_pos.x) < (0.1 * (registry.motions.get(destination).scale.x ))){ //&& self_m.velocity.y == 0.0f && self_m.position.y < destination_pos.y
			// above target platform within good range , adjust readyness, check if goal is on same plane
			self_m.velocity.x = 0.0f;
			if(abs(self_m.position.y - goal_pos.y) < height/ 8.f && registry.platformerPhysics.get(e).onPlatform){
				// close enough to goal, run to goal
				return BTPlayerState::Goal_in_sight;
			}
			if(self_m.velocity.y == 0.0f){
				// if grounded
				ready = false;
			}
		}
		if(self_m.velocity.y != 0){
			return BTPlayerState::Jumping;
		}
        return BTPlayerState::Running;
	}

	void destroy() override {
		delete this;
	}
};

// Agent needs to be able to navigate towards next platform,
// jumps once and then navigates through the air towards the goal before touching down
class JumpToTarget : public BTNode {
private:
	float vertical_speed;
	float horizontal_speed;
	Entity target;
public:
	void init(Entity e) override {
		// init target here
		target = e;
	}

	JumpToTarget(float horizontal_speed,float vertical_speed) {
		this->horizontal_speed = horizontal_speed;
		this->vertical_speed = vertical_speed;
	}

	BTPlayerState process(Entity e) override {
		Motion& m = registry.motions.get(e);
		vec2 destination_pos = registry.motions.get(target).position;
		if(m.velocity.y == 0.0f && registry.platformerPhysics.get(e).onPlatform){ // check if allowed to jump (check if standing on ground)
			m.velocity.y = -vertical_speed;
			float dir = m.position.x - destination_pos.x > 0.0f ? -1.0f : 1.0f;
			m.velocity.x = horizontal_speed * dir;
			return BTPlayerState::Jumping;
		}
        return BTPlayerState::In_jump_range;
	}

	void destroy() override {
		delete this;
	}
};

// runToGoal, when the player has a clear path to the goal,
// run at full speed to goal but be mindful of traps (TBA)
class RunToGoal : public BTNode {
private:
    // private class members here (eg children nodes)
	float horizontal_speed;
	float vertical_speed;
	int width;
	int height;
	vec2 goal_pos = vec2(0.0f);
public:
	void init(Entity e) override {
		goal_pos = registry.motions.get(registry.goals.entities[0]).position;
	}

	RunToGoal(float horizontal_speed, float vertical_speed, int width,int height) {
		this->horizontal_speed = horizontal_speed;
		this->vertical_speed = vertical_speed;
		this->width = width;
		this->height = height;
	}

	BTPlayerState process(Entity e) override {
		// set velocity x to goal
		Motion& self_m = registry.motions.get(e);
		float dir = self_m.position.x - goal_pos.x > 0.0f ? -1.0f : 1.0f;
		self_m.velocity.x = horizontal_speed * dir;

		// detect if fallen
		if(abs(self_m.position.y - goal_pos.y) > height/ 8.f){
				return BTPlayerState::StartRun;
		}

		// jump towards goal to be on safe side regarding gaps
		if(abs(self_m.position.x - goal_pos.x) > (self_m.scale.x / 2.0f))
		{
			// TODO
			int choice = rand()%2;
			if(choice == 1){
				return BTPlayerState::Running_to_goal;
			}else{
				return BTPlayerState::In_jump_range;
			}
			
		}else { // target is above player
			self_m.velocity.x = 0.0f;
			return BTPlayerState::In_jump_range;
		}
	}

	void destroy() override {
		delete this;
	}
};

// composite element, chooses action each frame for the AI player
class PlatformAIAgent : public BTNode {
private:
    // private class members here (eg children nodes)
	TargetNearestAbovePlatform* run;
	JumpToTarget* jump;
	RunToGoal* runToEnd;
	BTNode* target;
	float horizontal_speed;
	float vertical_speed;
	float gravity;
	int width;
	int height;
	
public:
	PlatformAIAgent(float horizontal_speed, float vertical_speed, float gravity, int width,int height) {
		this->horizontal_speed = horizontal_speed;
		this->vertical_speed = vertical_speed;
		this->gravity = gravity;
		this->width = width;
		this->height = height;
	}
    void init(Entity e) override {
		run = new TargetNearestAbovePlatform(horizontal_speed,vertical_speed, gravity,width,height);
		jump = new JumpToTarget(horizontal_speed,vertical_speed);
		runToEnd = new RunToGoal(horizontal_speed, vertical_speed, width,height);
		target = run;
		run->init(e);
	}

	BTPlayerState process(Entity e) override {
		BTPlayerState state = target->process(e);

		if(registry.stunTimers.has(e)){
			return state;
		}

        switch (state)
		{
		case BTPlayerState::StartRun:
			run->init(e);
		case BTPlayerState::Jumping :
		case BTPlayerState::Running :
			target = run;
			break;
		case BTPlayerState::In_jump_range:
			target = jump;
			target->init(run->destination);
			break;
		case BTPlayerState::Goal_in_sight:
			target = runToEnd;
			target->init(e);
			break;
		default:
			break;
		}
        return state;
	}

	void destroy() override {
        // delete any children nodes first
		run->destroy();
		jump->destroy();
		runToEnd->destroy();
		delete this;
	}
};