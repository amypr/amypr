#include "platformer_system.hpp"
#include "platformer_ai.hpp"

std::vector<std::tuple<BTNode*, Entity>> ai_registry;
const float AI_DELAY = 1;
const int MAX_TRAPS = 3;
std::vector<vec2> trapPositions;

void PlatformerSystem::step(float elapsed_ms)
{
	Platformer& p = registry.platformer.get(board_controller);
	if (!p.loaded && p.active) {
		buildPlatformer();
	}
	else if (p.loaded && !p.active) {
		deconstructPlatformer();
	}else if (p.loaded && p.active){
		next_ai_update -= elapsed_ms * speed;
		if(next_ai_update < 0.f){
			// Reset timer
			next_ai_update = (AI_DELAY / 2.0f) + uniform_dist(rng) * (AI_DELAY / 2.0f);
			std::vector<std::tuple<BTNode*, Entity>>::iterator it;
			for (it = ai_registry.begin(); it != ai_registry.end();)
			{
				auto entity = std::get<1>(*it);
				auto node = std::get<0>(*it);	
				auto state = node->process(entity);
				it++;
			}
		}
		// animate character if moving
		std::vector<std::tuple<BTNode*, Entity>>::iterator it2;
			for (it2 = ai_registry.begin(); it2 != ai_registry.end();)
			{
				auto entity = std::get<1>(*it2);
				if(registry.motions.has(entity) && registry.sprites.has(entity)){
					Motion& m = registry.motions.get(entity);
					Sprite& sprite = registry.sprites.get(entity);
						if (m.velocity.x > 0.0f) {
							// check to see if enough time has elapsed 
							sprite.time_last_update += elapsed_ms;
							// switch frames regardless of time if it is a direction change
							if (sprite.frame.y == 1) {
								sprite.time_last_update = 0;
								sprite.frame.x = fmod(sprite.frame.x + 1, (float)sprite.num_frames);
								sprite.frame.y = 0;
							}
							else if (sprite.time_last_update >= sprite.animation_speed) {
								sprite.time_last_update = 0;
								sprite.frame.x = fmod(sprite.frame.x + 1, (float)sprite.num_frames);
								sprite.frame.y = 0;
							}
						}
						else if (m.velocity.x < 0.0f) {

							// check to see if enough time has elapsed 
							sprite.time_last_update += elapsed_ms;
							// switch frames regardless of time if it is a direction change
							if (sprite.frame.y == 0) {
								sprite.time_last_update = 0;
								sprite.frame.x = fmod(sprite.frame.x + 1, (float)sprite.num_frames);
								sprite.frame.y = 1;
							}
							else if (sprite.time_last_update >= sprite.animation_speed) {
								sprite.time_last_update = 0;
								sprite.frame.x = fmod(sprite.frame.x + 1, (float)sprite.num_frames);
								sprite.frame.y = 1;
							}
						}
				
				it2++;
			}
			}
	}
	
	std::vector<std::tuple<BTNode*, Entity>>::iterator it3;
			for (it3 = ai_registry.begin(); it3 != ai_registry.end();)
			{
				auto entity = std::get<1>(*it3);
				if(registry.stunTimers.has(entity) && registry.motions.has(entity)){
					registry.motions.get(entity).velocity.x = 0.0f;
				}
				it3++;
	}

}

void PlatformerSystem::init(RenderSystem* r, int width, int height) {
	window_width_px = width;
	window_height_px = height;
	renderer = r;
	chanceForTrap = 0.f;
	rng = std::default_random_engine(std::random_device()());
	registry.platformer.emplace(board_controller);
}

void PlatformerSystem::buildPlatformer() {
	// todo: there's a weird problem of window not being sized at 0-window_width_px?? on pc, not sure about laptop

	Platformer& p = registry.platformer.get(board_controller);
	
	float platformX = window_width_px / 2.f; // first platform generated at centre of screen
	float heightBtwnPlatforms = window_height_px / 8.f;
	float widthBtwnPlatforms = window_width_px / 8.f;
	float distanceBtwnSection;
	float minDistFromWindowEdge = window_width_px / 6.f;
	int i = 1;

	// create centre platform
	Entity centrePlatform = createPlatform(vec2(platformX, window_height_px - i * heightBtwnPlatforms));
	// get scale of platform for later use
	float platformSize = registry.motions.get(centrePlatform).scale.x;
	// set distance btwn platform pieces to be size of platform
	distanceBtwnSection = platformSize;

	// set chance for trap to spawn on ground floor to 0
	chanceForTrap = 0.f;
	// create a ground platform
	float position = 0.f;
	while (position < window_width_px + platformSize) {
		createPlatform({ position, window_height_px });
		position += distanceBtwnSection;
	}

	// change possibility of traps to > 0 after ground platform created
	chanceForTrap = 0.15;

	// create some platforms randomly generated
	for (i = 2; i < window_height_px / heightBtwnPlatforms - 1; i++) {
		// randomly move platform left or right from last platform
		float rngValue = uniform_dist(rng);
		if (rngValue > 0.5 && platformX > minDistFromWindowEdge) {
			platformX -= rngValue * window_width_px/24 + widthBtwnPlatforms;
		}
		else if (rngValue <= 0.5 && platformX < window_width_px - minDistFromWindowEdge) {
			platformX += rngValue * window_width_px / 24 + widthBtwnPlatforms;
		}
		else if (platformX >= window_width_px - minDistFromWindowEdge) {
			platformX -= rngValue * window_width_px / 24 + widthBtwnPlatforms;
		}
		else {
			platformX += rngValue * window_width_px / 24 + widthBtwnPlatforms;
		}

		// reset platform location to within bounds if too far out
		if (platformX < platformSize / 2.f) {
			platformX = platformSize / 2.f;
		}
		else if (platformX > window_width_px - platformSize / 2.f) {
			platformX = window_width_px - platformSize / 2.f;
		}

		// save positions of sides of guaranteed platform so that other platforms created do not interfere with it
		float guaranteedPlatLeft = platformX - platformSize / 2.f;

		// create platform that forms route GUARANTEED to make it to the top

		if (i == window_height_px / heightBtwnPlatforms - 2) {
			// don't let traps spawn on last level of platforms (aka where goal will be)
			// cause that would be evil...
			chanceForTrap = 0.f;
		}
		createPlatform(vec2(platformX, window_height_px - i * heightBtwnPlatforms));
		// lengthen platform at random
		float lengthOfPlatform = 1;
		while (uniform_dist(rng) > 0.5 && lengthOfPlatform < 3 && platformX < window_width_px - minDistFromWindowEdge) {
			platformX += distanceBtwnSection;
			createPlatform(vec2(platformX, window_height_px - i * heightBtwnPlatforms));
			lengthOfPlatform++;
			rngValue = uniform_dist(rng);
		}
		// save positions of sides of guaranteed platform so that other platforms created do not interfere with it
		float guaranteedPlatRight = platformX + platformSize / 2.f;

		// reset chance for traps on platforms that aren't integral to reach goal
		chanceForTrap = 0.15f;

		// create random platforms at the same height level to introduce more challenge/variety
		float tempX = platformSize/2.f;
		while (tempX < window_width_px - platformSize /2.f) {
			// don't create platform if within guaranteed platform bounds
			if ((tempX - platformSize >= guaranteedPlatLeft && tempX - platformSize <= guaranteedPlatRight)
				|| (tempX + platformSize >= guaranteedPlatLeft && tempX + platformSize <= guaranteedPlatRight)) {
				tempX += rngValue * window_width_px / 24;
				continue;
			}

			// randomly determine IF a platform should even be built at tempX position, weighted lower
			if (uniform_dist(rng) < 0.25) {
				createPlatform(vec2(tempX, window_height_px - i * heightBtwnPlatforms));
				lengthOfPlatform = 1;

				// lengthen this platform at random
				while (uniform_dist(rng) > 0.5 && lengthOfPlatform < 2 && tempX < window_width_px - minDistFromWindowEdge) {
					tempX += distanceBtwnSection;
					if ((tempX - platformSize >= guaranteedPlatLeft && tempX - platformSize <= guaranteedPlatRight)
						|| (tempX + platformSize >= guaranteedPlatLeft && tempX + platformSize <= guaranteedPlatRight)) {
						break;
					}
					createPlatform(vec2(tempX, window_height_px - i * heightBtwnPlatforms));
					lengthOfPlatform++;
					rngValue = uniform_dist(rng);
				}

			}

			tempX += rngValue * window_width_px / 20 + widthBtwnPlatforms;
		}

	}

	// draw traps now (so they appear in front of platforms)
	for (vec2 position : trapPositions) {
		createTrap(position);
	}

	// place goal @ end of stage to exit platformer mini-game
	createGoal(vec2(platformX, window_height_px - i * heightBtwnPlatforms));

	// draw players + AI (todo project: allow for character selection)
	createCharacters();
	Motion& motionOne = registry.motions.get(player_one);
	motionOne.position = { widthBtwnPlatforms, window_height_px - motionOne.scale.y };
	Motion& motionTwo = registry.motions.get(player_two);
	motionTwo.position = { window_width_px - widthBtwnPlatforms, window_height_px - motionTwo.scale.y };
	Motion& motionThree = registry.motions.get(ai_one);
	motionThree.position = { widthBtwnPlatforms - motionThree.scale.x, window_height_px - motionThree.scale.y };
	Motion& motionFour = registry.motions.get(ai_two);
	motionFour.position = { window_width_px - widthBtwnPlatforms + motionFour.scale.x, window_height_px - motionFour.scale.y };

	// platformer now loaded
	p.loaded = true;
}

void PlatformerSystem::deconstructPlatformer() {
	Platformer& p = registry.platformer.get(board_controller);

	while (registry.platforms.entities.size() > 0) {
		registry.remove_all_components_of(registry.platforms.entities.back());
	}
		
	while (registry.goals.entities.size() > 0) {
		registry.remove_all_components_of(registry.goals.entities.back());
	}
	while (registry.traps.entities.size() > 0) {
		registry.remove_all_components_of(registry.traps.entities.back());
	}
	trapPositions.clear();

	registry.remove_all_components_of(player_one);
	registry.remove_all_components_of(player_two);
	registry.remove_all_components_of(ai_one);
	registry.remove_all_components_of(ai_two);

	std::vector<std::tuple<BTNode*, Entity>>::iterator it;
	for (it = ai_registry.begin(); it != ai_registry.end();)
	{
		auto entity = std::get<1>(*it);
		auto node = std::get<0>(*it);	
		it = ai_registry.erase(it);
		node->destroy();
	}

	p.loaded = false;
}

void PlatformerSystem::createCharacters()
{
	// scaling objects shown based on window size (the smaller of the sizes is used to scale)
	float scaleOfObjects = window_width_px > window_height_px ? window_height_px / 10 : window_width_px / 15;

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::CHARACTER);
	registry.meshPtrs.emplace(player_one, &mesh);
	registry.meshPtrs.emplace(player_two, &mesh);

	// Initialize the position, scale, and physics components
	auto& motionOne = registry.motions.emplace(player_one);
	motionOne.scale = mesh.original_size * scaleOfObjects;
	auto& motionTwo = registry.motions.emplace(player_two);
	motionTwo.scale = mesh.original_size * scaleOfObjects;

	// Create an (empty) Player component to be able to refer to all players
	registry.playerTanks.emplace(player_one);
	registry.playerTanks.emplace(player_two);

	// Set sprite attributes to character
	registry.sprites.emplace(player_one);
	registry.sprites.emplace(player_two);

	// set character to be affected by platformer physics
	registry.platformerPhysics.emplace(player_one);
	registry.platformerPhysics.emplace(player_two);

	// create render requests for both p1 & p2
	registry.renderRequests.insert(
		player_one,
		{ playerAssets[0],
			EFFECT_ASSET_ID::CHARACTER,
			GEOMETRY_BUFFER_ID::CHARACTER });
	registry.renderRequests.insert(
		player_two,
		{ playerAssets[1],
			EFFECT_ASSET_ID::CHARACTER,
			GEOMETRY_BUFFER_ID::CHARACTER });
	//-------------------------------------------------------------------------------------------------------
	// below is creation of AI on platformer (they don't have "player" component)

	registry.meshPtrs.emplace(ai_one, &mesh);
	registry.meshPtrs.emplace(ai_two, &mesh);
	// Initialize the position, scale, and physics components
	auto& motionThree = registry.motions.emplace(ai_one);
	motionThree.scale = mesh.original_size * scaleOfObjects;
	auto& motionFour = registry.motions.emplace(ai_two);
	motionFour.scale = mesh.original_size * scaleOfObjects;
	// Set sprite attributes to character
	registry.sprites.emplace(ai_one);
	registry.sprites.emplace(ai_two);
	// set character to be affected by platformer physics
	registry.platformerPhysics.emplace(ai_one);
	registry.platformerPhysics.emplace(ai_two);
	// create render requests for both ai1 & ai2
	registry.renderRequests.insert(
		ai_one,
		{ playerAssets[2],
			EFFECT_ASSET_ID::CHARACTER,
			GEOMETRY_BUFFER_ID::CHARACTER });
	registry.renderRequests.insert(
		ai_two,
		{ playerAssets[3],
			EFFECT_ASSET_ID::CHARACTER,
			GEOMETRY_BUFFER_ID::CHARACTER });
	// AI controllers init

	//get AI world data
	float horizontalVelocity = this->window_width_px / 7.f;
	float verticalVelocity = this->window_height_px / 2.f;
	float gravity = (float)window_height_px / 80;
	// player 3
	PlatformAIAgent* player3Brain = new PlatformAIAgent(horizontalVelocity,verticalVelocity,gravity,this->window_width_px,this->window_height_px);
	player3Brain->init(ai_one);
	std::tuple<BTNode*, Entity> p3 = std::make_tuple(player3Brain,ai_one);
	ai_registry.push_back(p3);

	// player 4
	PlatformAIAgent* player4Brain = new PlatformAIAgent(horizontalVelocity,verticalVelocity,gravity,this->window_width_px,this->window_height_px);
	player4Brain->init(ai_two);
	std::tuple<BTNode*, Entity> p4 = std::make_tuple(player4Brain,ai_two);
	ai_registry.push_back(p4);
}


// todo project: add moving platforms?
Entity PlatformerSystem::createPlatform(vec2 position)
{
	// scaling objects shown based on window size (the smaller of the sizes is used to scale)
	float scaleOfObjects = window_width_px > window_height_px ? window_height_px / 10 : window_width_px / 15;

	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;

	motion.scale = mesh.original_size * scaleOfObjects;

	registry.platforms.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLATFORM,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	// everytime there's a platform, there's a small chance of spawning a trap on top
	if (trapPositions.size() < MAX_TRAPS) {
		if (uniform_dist(rng) < chanceForTrap) {
			// add position to trapPositions to draw later (after all platforms have been drawn)
			vec2 trapPosition = { motion.position.x, motion.position.y - motion.scale.y };
			trapPositions.push_back(trapPosition);
		}
	}

	return entity;
}

Entity PlatformerSystem::createGoal(vec2 position)
{
	// scaling objects shown based on window size (the smaller of the sizes is used to scale)
	float scaleOfObjects = window_width_px > window_height_px ? window_height_px / 10 : window_width_px / 15;

	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;

	motion.scale = mesh.original_size * scaleOfObjects;

	registry.goals.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::GOAL,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity PlatformerSystem::createTrap(vec2 position)
{
	// scaling objects shown based on window size (the smaller of the sizes is used to scale)
	float scaleOfObjects = window_width_px > window_height_px ? window_height_px / 10 : window_width_px / 15;

	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;

	motion.scale = mesh.original_size * scaleOfObjects;

	// set entity as a trap
	registry.traps.emplace(entity);

	// TODO CHANGE ASSET TO TRAP ASSET
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TRAP,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}
