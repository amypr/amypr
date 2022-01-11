// internal
#include "board_system.hpp"
#include <iostream>
#include <chrono>
#include "json_level_loader.hpp"
#include <glm/gtx/compatibility.hpp>

Entity board_controller = Entity();

bool rendering(){
	for (Entity entity : registry.renderRequests.entities)
	{
		if (!registry.motions.has(entity))
			return true;
	}
	return false;
}

void BoardSystem::step(float elapsed_ms)
{
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	GameBoard &g = registry.gameBoard.get(board_controller);
	Menu& menu = registry.menu.get(board_controller);
	TooManyItems& tmi = registry.tooManyItems.get(board_controller);
	// printf("stage: %d\n", g.stage);
	// visual elements
	if(g.stage == -1 || g.stage == 17){ // reset, TODO remove stage 17 when complete
		g.active = true;
	}else if(g.stage == -2){ // shutdown
		g.active = false;
		renderStage = 0;
		background = 0;
	}else if (g.stage == -99) {
		loadLevel(&g,this->renderer->window_width_px,this->renderer->window_height_px);
		printf("loading lvl \n");
		g.stage = -1;
	}
	if (g.active && !g.loaded && renderStage == 0 && !rendering()) { // a very annoying fix to avoid the renderer getting the order wrong for the backgrounds :(
		if(background < g.backgrounds.at(g.active_background).elements.size()){
			if(background == 0){
				setBoardDims(this->renderer,&g);
			}
			BoardSystem::generateBoardBackgroundOnce(this->renderer, &g,background);
			background++;
		}else {
			renderStage = 1;
			background = 0;
		}
	}else if (g.active && !g.loaded && renderStage == 1  && !rendering() ) {
		BoardSystem::generateGameBoard(this->renderer, &g);
		renderStage = 2;
	}else if (g.active && !g.loaded && renderStage == 2  && !rendering()){
		BoardSystem::drawCharacters(this->renderer,&g);
		if (registry.board.entities.empty()) {
			displayScoreboard();
		}
		renderStage = 3;
		// draw in the dialogue box here
		// IF: not seen before. resets on game restart, is saved from previous game if loaded.
		if (!g.introSeen) {
			createDialogueSprite(registry.motions.get(createDialogue(TEXTURE_ASSET_ID::INTRO_SPRITESHEET, 1)));
		}
	}else if (g.active && !g.loaded && renderStage == 3  && !rendering()){
		g.loaded = true;
		renderStage = 0;
		background = 0;
		g.stage = 0;
		for (int i = 0; i < g.players; i++) {
			if (g.is_poisoned[i] && !registry.poisoned.has(std::get<0>(g.player_positions[i]))) {
				registry.poisoned.emplace(std::get<0>(g.player_positions[i]));
			}
		}
	}
	else if (!g.active && g.loaded) {
		BoardSystem::deconstructGameBoard(&g);
		removeScoreboard();
	}

	if(g.active == true && g.loaded == true){
		Camera& camera = registry.camera.components[0];
		bool busy = false;
		Entity empty;
		int dice = 1;
		float cameraPos;
		renderStage = 0;
		TEXTURE_ASSET_ID item;
		float lifeSpan;
		std::vector<Entity> to_remove;
		float step_seconds = 1.0f * (elapsed_ms / 1000.f);
		CoinInfo& coinInfo = registry.coinInfo.get(board_controller);
		Temple& temple = registry.temple.components[0];
		// Entity temple_entity = registry.temple.entities[0];
		// Motion& temple_motion = registry.motions.get(temple_entity);
		// // temple_motion.angle += 0.08;
		temple.rotationX += 0.008;
		// temple.rotationY += 0.08;
		switch(g.stage)
		{
			case 0: // choose to roll, use item, look ahead

				// create tutorial here based on conditions
				if (g.introSeen && !g.tutorialSeen && registry.dialogue.entities.size() == 0) {
					createDialogueSprite(registry.motions.get(createDialogue(TEXTURE_ASSET_ID::TUTORIAL_SPRITESHEET, 1)));
				}
				dice = 1;
				camera.target = *(&std::get<0>(g.player_positions[g.turn]));
				if (g.aiFlag[g.turn]) {
					g.playerStates[g.turn] = BOARD_AI_STATE::TURN_START;
					Board_FSM::process(g.turn, g.playerStates[g.turn], &g, 0,(void*)this);
				}
				if (g.inv_active && !g.inv_loaded) {
					cameraPos = cameraPositionX(camera.x, window_width_px);
					createItem(renderer, TEXTURE_ASSET_ID::SHOP_BACKGROUND, {cameraPos, window_height_px / 2}, {window_width_px, window_height_px});
					int player_inv_num = g.playerInv[g.turn].size();
					vec2 spacing = {window_width_px / 5, window_height_px / 5};
					vec2 position = {cameraPos, window_height_px / 2};
					for (int i = 0; i < player_inv_num; i++) {
						if (i == 0 || i == 3 || i == 6) {
							position.x -= spacing.x;
						}
						if (i == 2 || i == 5 || i == 8) {
							position.x += spacing.x;
						}
						if (i < 3) {
							position.y -= spacing.y;
						}
						if (i > 5) {
							position.y += spacing.y;
						}
						item = g.playerInv[g.turn].at(i);
						createItem(renderer, item, position, {window_width_px / 7, window_height_px / 5.5});
						position = {cameraPos, window_height_px / 2};
					}
					g.inv_loaded = true;
				}
				break;
			case 1: // TODO item select
				if (g.aiFlag[g.turn]) {
					Board_FSM::process(g.turn, g.playerStates[g.turn], &g, 0, (void*)this);
				}
				break;
			case 2: // TODO item use
				if (g.aiFlag[g.turn]) {
					Board_FSM::process(g.turn, g.playerStates[g.turn], &g, 0, (void*)this);
				}
				if (g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_TRANSPORT) {
					int i = 1;
					int difference = 0;
					for (GameTile tile : g.config) {
						if (tile.type == GAMETILE_TYPE::GAIN_TREASURE && i > std::get<1>(g.player_positions[g.turn]).x) {
							break;
						}
						i++;
					}
					if (i != 0 && i > std::get<1>(g.player_positions[g.turn]).x && i < g.size) {
						difference = i - std::get<1>(g.player_positions[g.turn]).x;
						std::get<1>(g.player_positions[g.turn]).x = i;
					}
					*(&std::get<2>(g.player_positions[g.turn])) = std::get<1>(g.player_positions[g.turn]).x;
					registry.motions.get(std::get<0>(g.player_positions[g.turn])).position.x += (GAME_TILE_SPACING * difference);
					g.active_item[g.turn] = TEXTURE_ASSET_ID::BOARD_HELP; //dummy to avoid using any other items once done
				}
				if (g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_POISON || g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_LIGHTENING) {
					int player_to_affect = std::uniform_int_distribution<int>{0, g.players - 1}(this->generator);
					for (int i = 0; i < 1000; i++) {
						if (player_to_affect != g.turn) {
							if (!g.is_poisoned[player_to_affect]) {
								break;
							}
						}
						player_to_affect = std::uniform_int_distribution<int>{0, g.players - 1}(this->generator);
					}
					if (g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_LIGHTENING) {
						g.player_points[player_to_affect] = floor(g.player_points[player_to_affect] / 2);
						Entity scoreboard = registry.board.entities[player_to_affect];
						Motion& sbMotion = registry.motions.get(scoreboard);
						createKFAnimation(renderer, 0.f, {sbMotion.scale.x * 2, sbMotion.scale.y}, TEXTURE_ASSET_ID::LIGHTENING, {sbMotion.position});
						g.stage = 6;
					}
					if (g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_POISON) {
						g.is_poisoned[player_to_affect] = true;
						if (!registry.poisoned.has(std::get<0>(g.player_positions[player_to_affect]))) {
							registry.poisoned.emplace(std::get<0>(g.player_positions[player_to_affect]));
						}
					}
					g.active_item[g.turn] = TEXTURE_ASSET_ID::BOARD_HELP; //dummy to avoid using any other items once done
				}
				if (g.aiFlag[g.turn] && g.stage != 6) {
					g.stage = 9;
				}
				break;
			case 3: // edge scrolling, AI players never select this option
				// locked scrolling to camerapos.x > 0
				camera.target = 0;
				if(camera.x + g.xAxisScroll <= 0.1f)
					camera.x += g.xAxisScroll;
				break;
			case 4: // 4-8 reserved for item actions
				if (tmi.too_many && !tmi.active) {
					cameraPos = cameraPositionX(camera.x, window_width_px);
					createItem(renderer, TEXTURE_ASSET_ID::TOO_MANY_ITEMS, {cameraPos, window_height_px / 3}, {window_width_px / 1.2, window_height_px / 3.5});
					tmi.active = true;
				} else {
					for (int i = 0; i < registry.items.components.size(); i++) {
						Entity entity_i = registry.items.entities[i];
						Motion motion_i = registry.motions.get(entity_i);
						Items& itemBool = registry.items.get(entity_i);
						if (!itemBool.outOfService && itemBool.tooExpensive) {
							createItem(renderer, TEXTURE_ASSET_ID::NOT_ENOUGH_TREASURE, motion_i.position, {window_width_px / 3.5, window_height_px / 3.5});
						}
					}
				}
				break;
			case 5:
				break;
			case 6:
				for (Entity kfa : registry.keyframeAnimation.entities) {
					KeyframeAnimation& animation = registry.keyframeAnimation.get(kfa);
					Motion& animationMotion = registry.motions.get(kfa);
					// if (!animation.active) {
					// 	continue;
					// }

					if (animation.lifeRemaining <= 0.f) {
						// animation.active = false;
						removeScoreboard();
						displayScoreboard();
						if (animation.type == TEXTURE_ASSET_ID::TEXTURE_COUNT) {
							coinInfo.numCoins.clear();
							coinInfo.startingPositions.clear();
							g.stage = 14;
						} else if (animation.type == TEXTURE_ASSET_ID::LIGHTENING) {
							g.stage = 2;
						}
						registry.remove_all_components_of(kfa);
						continue;
					}
					
					if (animation.larpRemaining <= 0.f) {
						animation.larpRemaining = animation.larpTimeMs;
					}

					animation.lifeRemaining -= elapsed_ms;
					animation.larpRemaining -= elapsed_ms;
					lifeSpan = animation.larpRemaining / animation.larpTimeMs;
					if (animation.lifeRemaining <= animation.lifeTimeMs / 2) {
						if (animation.type == TEXTURE_ASSET_ID::LIGHTENING) {
							registry.colors.get(kfa) = glm::lerp(animation.kfColours[2], animation.kfColours[1], lifeSpan);
						}
						if (animation.type == TEXTURE_ASSET_ID::TEXTURE_COUNT) {
							animation.kfRotations[1] += 0.08;
							animationMotion.position = glm::lerp(animation.kfPositions[2], animation.kfPositions[1], lifeSpan);
						}
					} else {
						if (animation.type == TEXTURE_ASSET_ID::LIGHTENING) {
							registry.colors.get(kfa) = glm::lerp(animation.kfColours[1],animation.kfColours[0], lifeSpan);
						}
						if (animation.type == TEXTURE_ASSET_ID::TEXTURE_COUNT) {
							animation.kfRotations[1] += 0.08;
							animationMotion.position = glm::lerp(animation.kfPositions[1], animation.kfPositions[0], lifeSpan);
						}
					}
				}
				break;
			case 7:
				if (registry.coins.components.size() <= 0) {
					coinInfo.numCoins.clear();
					coinInfo.startingPositions.clear();
					removeScoreboard();
					displayScoreboard();
					g.stage = 14;
				}
				for (Entity ce : registry.coins.entities) {
					Motion& coin_motion = registry.motions.get(ce);
					Coins& coin = registry.coins.get(ce);
					if (coin_motion.position.y >= registry.motions.get(std::get<0>(g.player_positions[g.turn])).position.y) {
						to_remove.push_back(ce);
					} else {
						coin.rotationX += 0.08;
						// coin_motion.angle += 0.08;
						float new_vy = coin_motion.velocity.y + ((9.8 * step_seconds) + coin_motion.velocity.y * step_seconds);
						coin_motion.position.y += 0.5 * (coin_motion.velocity.y + new_vy) * step_seconds;
						coin_motion.position.g += 0.5 * (coin_motion.velocity.g + new_vy) * step_seconds;
						coin_motion.position.t += 0.5 * (coin_motion.velocity.t + new_vy) * step_seconds;
						coin_motion.velocity.y = new_vy;
					}
				}
				for (int i = 0; i < to_remove.size(); i++) {
					registry.remove_all_components_of(to_remove[i]);
				}
				to_remove.clear();
				break;
			case 8:
				g.stage = 9;
				break;
			case 9: //spawn dice
				if (g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_DICE) {
					dice = 2;
				}
				camera.target = *(&std::get<0>(g.player_positions[g.turn]));
				// put dice in middle part of screen relative to the player
				for(int l = 0; l < dice; l++){
					BoardSystem::createDie(this->renderer, vec2(l*this->renderer->window_width_px * 0.05 + registry.motions.get(*(&std::get<0>(g.player_positions[g.turn]))).position.x,this->renderer->window_height_px/2.0f));
				}
				if (g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_DICE) {
					dice = 1;
				}
				g.stage++;
				break;
			case 10:
				if (g.aiFlag[g.turn]) {
					Board_FSM::process(g.turn, g.playerStates[g.turn], &g, (int)this->window_width_px*0.05f, (void*)this);
				}
				//check for rolling dice and generate rand int on range [1,6]
				for (GameDie& dd : registry.dice.components){
					if(dd.value == -1){
						busy = true;
						if (g.aiFlag[g.turn])
							Mix_PlayChannel(-1, this->dice_roll_sound, 0);
					}
					if (dd.rolling == true) {
						// TODO GO BACK TO NORMAL LATER
						int i = std::uniform_int_distribution<int>{1,6}(this->generator);
						dd.value = i;
						busy = true;
					}
					if(!dd.rolling && dd.value != 1 && dd.boom && !dd.boomed){
						Mix_PlayChannel(-1, this->dice_big_roll_sound, 0);
						dd.boomed = true;
					}
					
				}
				if(!busy){ // all dice finished rolling
					g.roll = 0;
					for(GameDie dice: registry.dice.components){
						g.roll += dice.value;
					}
					if (g.is_poisoned[g.turn]) {
						printf("poisoned, originally rolled %d \n", g.roll); // remove after debug finished
						if (g.roll - 3 < 0) {
							g.roll = 0;
						} else {
							g.roll -= 3;
						}
						g.is_poisoned[g.turn] = false;
					}
					printf("rolled %d \n", g.roll); // remove after debug finished
					// TODO REMOVE LATER
					// if (g.turn == 1) {
					// 	std::get<1>(g.player_positions[g.turn]).x += 4;
					// }
					// TODO REMOVE LATER
					std::get<1>(g.player_positions[g.turn]).x += g.roll;
					if(std::get<1>(g.player_positions[g.turn]).x > g.size){
						std::get<1>(g.player_positions[g.turn]).x = g.size;
					}
					g.stage++;
				}
				break;
			case 11: // show roll value with on screen text, click to continue
				if (drawn_nums.size() == 0){
					if(g.roll >= 10){
						drawn_nums.push_back(this->drawNumberOnly(vec2((std::get<1>(g.player_positions[g.turn]).x - g.roll) * GAME_TILE_SPACING - g.start_position.x, std::get<1>(g.player_positions[g.turn]).y - this->window_width_px*0.2f),this->getTexture(g.roll%10)));
						drawn_nums.push_back(this->drawNumberOnly(vec2((std::get<1>(g.player_positions[g.turn]).x - g.roll) * GAME_TILE_SPACING - g.start_position.x + this->window_width_px*0.05f,std::get<1>(g.player_positions[g.turn]).y - this->window_width_px*0.2f),this->getTexture((g.roll/10)%10)));
					}else{
						drawn_nums.push_back(this->drawNumberOnly(vec2((std::get<1>(g.player_positions[g.turn]).x - g.roll) * GAME_TILE_SPACING - g.start_position.x, std::get<1>(g.player_positions[g.turn]).y - this->window_width_px*0.2f),this->getTexture(g.roll)));
					}
				}
				if (g.aiFlag[g.turn]) {
					g.stage++; 
				}
				break;
			case 12:  // dice / numbers cleanup
				while (registry.dice.entities.size() > 0){
					registry.remove_all_components_of(registry.dice.entities.back());
				}
				while (drawn_nums.size() > 0){
					registry.remove_all_components_of(drawn_nums.back());
					drawn_nums.pop_back();
				}
				walkEmitter = createEmitter(std::get<0>(g.player_positions[g.turn]),vec2(registry.motions.get(std::get<0>(g.player_positions[g.turn])).scale.x/-4.0,registry.motions.get(std::get<0>(g.player_positions[g.turn])).scale.y/2.0),vec2(this->renderer->window_width_px*0.002f,this->renderer->window_height_px*0.003f),g.level.substr(0,5) != "beach" ? GEOMETRY_BUFFER_ID::LEAF : GEOMETRY_BUFFER_ID::SAND,vec2(this->renderer->window_width_px*-0.01f,this->renderer->window_height_px*-0.001f), 1000);
				g.stage++;
				break;
			case 13: // Move player roll value spaces forward (move with positive x velocity until reached)
				if (g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_DICE || g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_TRANSPORT ||
					g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_POISON || g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_LIGHTENING) {
					g.active_item[g.turn] = TEXTURE_ASSET_ID::BOARD_HELP; //dummy to avoid using any other items once done
				}
				if(registry.motions.get(std::get<0>(g.player_positions[g.turn])).position.x < (std::get<1>(g.player_positions[g.turn]).x * GAME_TILE_SPACING) - g.start_position.x ){
					g.xAxisScroll = this->window_width_px*0.15f;
					// react to passing a board tile
					if(*(&std::get<2>(g.player_positions[g.turn])) != floor((registry.motions.get(std::get<0>(g.player_positions[g.turn])).position.x + g.start_position.x )/ GAME_TILE_SPACING )){
						// update nearest position
						*(&std::get<2>(g.player_positions[g.turn])) = floor((registry.motions.get(std::get<0>(g.player_positions[g.turn])).position.x + g.start_position.x) / GAME_TILE_SPACING);
						// play passing sound here
						Mix_PlayChannel(-1, this->tile_pass_ring, 0);
						if (g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_TREASURE_2) {
							g.player_points[g.turn] += 3;
							coinInfo.numCoins.push_back(3);
							// removeScoreboard();
							// displayScoreboard();
						}
						if (g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_HEART) {
							for (int i = 0; i < g.players; i++) {
								if (i == g.turn) {
									continue;
								}
								if (registry.motions.get(std::get<0>(g.player_positions[g.turn])).position.x - (GAME_TILE_SPACING / 2) < registry.motions.get(std::get<0>(g.player_positions[i])).position.x &&
									registry.motions.get(std::get<0>(g.player_positions[g.turn])).position.x + (GAME_TILE_SPACING / 2) > registry.motions.get(std::get<0>(g.player_positions[i])).position.x) {
									for (int k = 0; k < coinInfo.startingPositions.size(); k++) {
										if (coinInfo.startingPositions[k] == registry.motions.get(std::get<0>(g.player_positions[i])).position.x) {
											continue;
										}
									}
									if (g.player_points[i] >=3) {
										g.player_points[i] -= 3;
										g.player_points[g.turn] += 3;
										coinInfo.numCoins.push_back(3);
										coinInfo.startingPositions.push_back(registry.motions.get(std::get<0>(g.player_positions[i])).position.x);
									} else {
										g.player_points[g.turn] += g.player_points[i];
										if (g.player_points[i] > 0) {
											coinInfo.numCoins.push_back(g.player_points[i]);
											coinInfo.startingPositions.push_back(registry.motions.get(std::get<0>(g.player_positions[i])).position.x);
										}
										g.player_points[i] = 0;
									}
								}
							}
							// removeScoreboard();
							// displayScoreboard();
						}
					}
					scoreboardMovementUpdate(BoardSystem::cameraPositionX(camera.x,this->renderer->window_width_px));
					// set velocity to move player
					registry.motions.get(*(&std::get<0>(g.player_positions[g.turn]))).velocity.x = this->window_width_px*0.15f;
					registry.motions.get(walkEmitter).velocity.x = this->window_width_px*0.15f;
					// TODO set emitter if exists to same velocity
				}else{
					// player arrived
					g.xAxisScroll = 0.0f;
					randomNumber = std::uniform_int_distribution<int>{1,4}(this->generator);
					g.roll = -1;
					registry.motions.get(std::get<0>(g.player_positions[g.turn])).velocity.x = 0.0f;
					registry.motions.get(walkEmitter).velocity.x = 0.0f;
					destroyEmitter(walkEmitter);
					*(&std::get<2>(g.player_positions[g.turn])) = std::get<1>(g.player_positions[g.turn]).x;
					if (g.aiFlag[g.turn]) {
						g.playerStates[g.turn] = g.config[(int)std::get<1>(g.player_positions[g.turn]).x - 1].type == GAMETILE_TYPE::RANDOM_EVENT ? BOARD_AI_STATE::REACT_TO_RAND_EVENT : BOARD_AI_STATE::REACT_TO_TILE;
					}
					if (registry.poisoned.has(std::get<0>(g.player_positions[g.turn]))) {
						registry.poisoned.remove(std::get<0>(g.player_positions[g.turn]));
					}
					if (g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_TREASURE_2) {
						g.active_item[g.turn] = TEXTURE_ASSET_ID::BOARD_HELP; //dummy to avoid using any other items once done
						if (coinInfo.numCoins.size() > 0 && coinInfo.startingPositions.size() > 0) {
							createCoins(renderer, registry.motions.get(std::get<0>(g.player_positions[g.turn])).position.x, (int)coinInfo.numCoins.size());
							g.stage = 7;
							break;
						}
					}
					if (g.active_item[g.turn] == TEXTURE_ASSET_ID::INV_HEART) {
						g.active_item[g.turn] = TEXTURE_ASSET_ID::BOARD_HELP; //dummy to avoid using any other items once done
						for (int i = 0; i < coinInfo.numCoins.size(); i++) {
							if (coinInfo.startingPositions.size() > 0) {
								vec2 pos = registry.motions.get(std::get<0>(g.player_positions[g.turn])).position;
								float radius = (pos.x - coinInfo.startingPositions[i]) / 2;
								float height = pos.y - radius;

								createKFAnimation(renderer, 0.f, {window_width_px / 18, window_width_px / 46}, TEXTURE_ASSET_ID::TEXTURE_COUNT,
								{{coinInfo.startingPositions[i], pos.y}, {coinInfo.startingPositions[i] + radius, height}, pos});
							}
						}
						if (coinInfo.numCoins.size() > 0) {
							g.stage = 6;
						} else {
							g.stage++;
						}
					} else {
						g.stage++;
					}
				}
				break;
			case 14: // TODO Game tile effect on player (set stage to proper value), includes shop, play sound based on tile type
				// todo add animations and effects depending on space
				switch (g.config[(int)std::get<1>(g.player_positions[g.turn]).x - 1].type) {
					case GAMETILE_TYPE::NEUTRAL:
						1; // TODO animation?
						g.stage++; // todo remove after implimentation, allow for animation or effect specific to space to occur
						break;
					case GAMETILE_TYPE::GAIN_TREASURE:
						g.player_points[g.turn] += 1;
						// removeScoreboard();
						// displayScoreboard();
						g.stage++; // todo remove after implimentation, allow for animation or effect specific to space to occur
						break;
					case GAMETILE_TYPE::LOSE_TREASURE:
						if(g.player_points[g.turn] > 0){
							g.player_points[g.turn] -= 1;
							// removeScoreboard();
							// displayScoreboard();
						}
						g.stage++; // todo remove after implimentation, allow for animation or effect specific to space to occur
						break;
					case GAMETILE_TYPE::GAIN_MOVEMENT:
						*(&std::get<1>(g.player_positions[g.turn]).x) += 1;
						*(&std::get<2>(g.player_positions[g.turn])) = std::get<1>(g.player_positions[g.turn]).x;
						registry.motions.get(std::get<0>(g.player_positions[g.turn])).position.x += GAME_TILE_SPACING;
						g.stage++; // todo remove after implimentation, allow for animation or effect specific to space to occur
						break;
					case GAMETILE_TYPE::LOSE_MOVEMENT:
						if(std::get<1>(g.player_positions[g.turn]).x > 0){
							*(&std::get<1>(g.player_positions[g.turn]).x) -= 1;
							*(&std::get<2>(g.player_positions[g.turn])) = std::get<1>(g.player_positions[g.turn]).x;
							registry.motions.get(std::get<0>(g.player_positions[g.turn])).position.x -= GAME_TILE_SPACING;
						}
						g.stage++; // todo remove after implimentation, allow for animation or effect specific to space to occur
						break;
					case GAMETILE_TYPE::SHOP:
						cameraPos = cameraPositionX(camera.x, window_width_px);
						createItem(renderer, TEXTURE_ASSET_ID::SHOP_BACKGROUND, {cameraPos, window_height_px / 2}, {window_width_px, window_height_px});
						item = TEXTURE_ASSET_ID::BOARD_HELP;
						for (int i = 0; i < 3; i++) {
							randomNumber = std::uniform_int_distribution<int>{0,8}(this->generator);
							switch((int)randomNumber){
								case 0:
									item = TEXTURE_ASSET_ID::HEAVY_DAMAGE_ITEM;
									break;
								case 1:
									item = TEXTURE_ASSET_ID::HEART_ITEM;
									break;
								case 2:
									item = TEXTURE_ASSET_ID::LIGHTENING_ITEM;
									break;
								case 3:
									item = TEXTURE_ASSET_ID::POISON_ITEM;
									break;
								case 4:
									item = TEXTURE_ASSET_ID::TRANSPORT_ITEM;
									break;
								case 5:
									item = TEXTURE_ASSET_ID::TREASURE_1_ITEM;
									break;
								case 6:
									item = TEXTURE_ASSET_ID::TREASURE_2_ITEM;
									break;
                                case 7:
                                    item = TEXTURE_ASSET_ID::HEAVY_DAMAGE_ITEM;
                                    break;
                                case 8:
                                    item = TEXTURE_ASSET_ID::ENHANCED_CANNON_ITEM;
                                    break;
								case 9:
									item = TEXTURE_ASSET_ID::HP_BUFF_ITEM;
									break;
								default:
									break;
							}
							switch (i)
							{
							case 0:
								createItem(renderer, item, {cameraPos - (window_width_px / 3.5), window_height_px / 3}, {window_width_px / 5.5, window_height_px / 4});
								break;
							case 1:
								createItem(renderer, item, {cameraPos, window_height_px / 3}, {window_width_px / 5.5, window_height_px / 4});
								break;
							case 2:
								createItem(renderer, item, {cameraPos + (window_width_px / 3.5), window_height_px / 3}, {window_width_px / 5.5, window_height_px / 4});
								break;
							default:
								break;
							}
						}
						createItem(renderer, TEXTURE_ASSET_ID::EXIT_BUTTON, {cameraPos, window_height_px - (window_height_px / 4)}, {window_width_px / 7, window_height_px / 7});
						g.stage = 4;
						if (g.aiFlag[g.turn]) {
							g.stage = 5;
							Board_FSM::process(g.turn, g.playerStates[g.turn], &g, 0,(void*)this);
							g.stage = 15; // todo remove after implimentation, allow for animation or effect specific to space to occur
						}
						break;
					case GAMETILE_TYPE::RANDOM_EVENT: // roll for rand event
						if (g.aiFlag[g.turn]) {
							if(g.playerStates[g.turn] == BOARD_AI_STATE::TURN_END){
								g.stage++;
								break;
							}
							Board_FSM::process(g.turn, g.playerStates[g.turn], &g, randomNumber,(void*)this);
						}else {
							switch((int)randomNumber){
								case 1:
									printf("steal space \n");
									break;
								case 2:
									printf("swap space \n");
									break;
								case 3:
									printf("gain random item space\n")	;
									break;
								default:
									printf("none space \n");
							}
							g.stage++; // todo remove after implimentation, allow for animation or effect specific to space to occur
							break;
						}
				}
				break;
			case 15: // Player end turn, increment turn if more players remaining, else goto minigame queue
				removeScoreboard();
				displayScoreboard();
				// if a player has reached the end, display winning dialogue
				if (std::get<1>(g.player_positions[g.turn]).x >= g.size && registry.dialogue.components.size() == 0) {
					createDialogueSprite(registry.motions.get(createDialogue(TEXTURE_ASSET_ID::ENDING_SPRITESHEET, g.turn + 1)));
				}
				if (g.aiFlag[g.turn]) {
					g.playerStates[g.turn] = BOARD_AI_STATE::TURN_END;
				}
				if (registry.dialogue.components.size() > 0) {
					// ending dialogue has been spawned, don't progress game
					return;
				}
				if(g.turn + 1 >= g.players){
					g.stage++;
				}else{
					g.turn++;
					g.stage = 0;
					camera.target = 0;
					float player_offset = (window_width_px / 2) - GAME_TILE_SPACING;
					if ((std::get<1>(g.player_positions[g.turn]).x * GAME_TILE_SPACING) != GAME_TILE_SPACING) {
						scoreboardMovementUpdate(std::get<1>(g.player_positions[g.turn]).x + player_offset);
					} else {
						scoreboardMovementUpdate(window_width_px / 2);
					}
				}
				break;
			case 16: // Minigame select and load, unload game board
				g.active = false;
				deconstructGameBoard(&g);
				removeScoreboard();
				camera.target = 0;
				camera.x = 0;
				randomNumber = std::uniform_int_distribution<int>{1,3}(this->generator);
                random_level_choice = std::uniform_int_distribution<int>{1,2}(this->generator);
				scoreboardMovementUpdate(BoardSystem::cameraPositionX(camera.x,this->renderer->window_width_px));
				if (randomNumber == 1) {
                    TankGame& t = registry.tankGame.get(board_controller);
                    t.level = random_level_choice;
                    t.active = true;
                } else if (randomNumber == 2) {
					DrawingGame& drawingGame = registry.drawingGame.get(board_controller);
        			drawingGame.active = true;
                } else if (randomNumber == 3) {
					Platformer& p = registry.platformer.get(board_controller);
					p.active = true;
				}
				removeScoreboard();
				// prepare menu to open help screen before mini-game starts
				menu.menuType = 1;
				menu.paused = true;
			
				break;
			case 17: // return from minigame and update scores, set player turn to 0 and goto stage -1 (load board)
				g.turn = 0;
				//camera.x = cameraCache;
				g.stage = -1;
				break;
			default:
				break;
		}
	}

	// camera movement
	
	// parallax background update
	this->parallaxMovementUpdate();

}

void BoardSystem::setBoardDims(RenderSystem* renderer,GameBoard* g){

	this->GAME_TILE_DIMS = vec2(renderer->window_height_px*0.2f);
	this->GAME_TILE_PERSPECTIVE_DISTORTION = vec2((GAME_TILE_DIMS).x, (GAME_TILE_DIMS).y * 0.7f);
	this->GAME_TILE_SPACING = (GAME_TILE_DIMS).x * 1.5f;
	g->start_position = vec2(this->GAME_TILE_DIMS.x * 0.75f, 0.70 * renderer->window_height_px);
}

void BoardSystem::init(RenderSystem* renderer, int window_width_px,int window_height_px) {
	this->window_width_px = window_width_px;
	this->window_height_px = window_height_px;
	this->renderer = renderer;
	this->generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	this->drawn_nums.clear();
	GameBoard g = GameBoard();
	setBoardDims(renderer, &g);
	// TODO USE FOR TESTING THEN REMOVE
	// for(int i = 0; i < 4; i++){
	// 	g.playerInv[i].push_back(TEXTURE_ASSET_ID::INV_DICE);
	// 	g.playerInv[i].push_back(TEXTURE_ASSET_ID::INV_HEART);
	// 	g.playerInv[i].push_back(TEXTURE_ASSET_ID::INV_LIGHTENING);
	// 	g.playerInv[i].push_back(TEXTURE_ASSET_ID::INV_HEART);
	// 	g.playerInv[i].push_back(TEXTURE_ASSET_ID::INV_TRANSPORT);
	// 	g.playerInv[i].push_back(TEXTURE_ASSET_ID::INV_TREASURE_1);
	// 	g.playerInv[i].push_back(TEXTURE_ASSET_ID::INV_TREASURE_2);
	// 	g.playerInv[i].push_back(TEXTURE_ASSET_ID::INV_HEART);

	// }

	BoardSystem::initCharacters(renderer,&g);
	this->tile_pass_ring = Mix_LoadWAV(audio_path("tile_pass.wav").c_str());
	this->dice_roll_sound = Mix_LoadWAV(audio_path("dice_roll.wav").c_str());
	this->dice_big_roll_sound = Mix_LoadWAV(audio_path("big_roll.wav").c_str());
	if (this->tile_pass_ring == nullptr || this->dice_roll_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s \n %s \n make sure the data directory is present",
			audio_path("tile_pass.wav").c_str(), audio_path("dice_roll.wav").c_str(), audio_path("big_roll.wav").c_str());
		return;
	}
	
	StartScreen& ss = registry.startScreen.get(board_controller);
	if (ss.restarted) {
		GameBoard& new_g = registry.gameBoard.get(board_controller);
		new_g = g;
	} else {
		registry.gameBoard.emplace(board_controller, g);
	}
}

void BoardSystem::loadLevel(GameBoard* g,int window_width_px,int window_height_px) {
	SceneBackGround background = SceneBackGround();
	std::vector<SceneBackGroundComponent> backgroundElements{};
	json level = Level_Loader::load_level(board_level_path(g->level))["Gameboard"];
	g->backgrounds.clear();
	g->config.clear();
	
	g->config = std::vector<GameTile>(0);
	for (auto& ele : level["config"]) {
		GameTile gameTile = {ele};
		g->config.push_back(gameTile);
	}
	g->size = g->config.size();

	json background_y_scale = level["background_y_scale"];
	json background_y_multi = level["background_y_multi"];
	json background_movement_offset = level["background_movement_offset"];
	json background_assets = level["background_assets"];

	for(int l = level["background_assets"].size()-1; l >= 0; l--){
		float offset = background_movement_offset[l];
		vec2 startpos =  vec2(window_width_px * 0.5f,window_height_px * (float)background_y_multi[l]);
		vec2 scale = vec2(-1.0f*window_width_px,window_height_px* (float)background_y_scale[l]);
		TEXTURE_ASSET_ID asset = static_cast<TEXTURE_ASSET_ID>(background_assets[l]);

		//float offset,vec2 startpos,vec2 scale,TEXTURE_ASSET_ID image;
		backgroundElements.push_back({offset,startpos,scale,asset});
		backgroundElements.push_back({offset,vec2(-startpos.x,startpos.y),vec2(-scale.x,scale.y),asset});
	}
	
	background.elements = backgroundElements;
	g->backgrounds.push_back(background);
}

void BoardSystem::generateGameBoard(RenderSystem* renderer,GameBoard* board) {
	// TODO load board config here
	float x = board->start_position.x;
	
	for (GameTile g : board->config) {
		BoardSystem::createGameTile(renderer, g.type, vec2(x, board->start_position.y));
		x += GAME_TILE_SPACING;
	}
	x += window_width_px / 4.2;
	// x = window_width_px / 3.5;
	createTemple(renderer, {x, window_height_px / 2.3});
}

void BoardSystem::initCharacters(RenderSystem* renderer,GameBoard* board)
{
	for(int i = 0; i < board->players; i++){
		std::tuple<Entity,vec2,int>* p = &(board->player_positions[i]);
		std::get<1>(*p) = vec2(1,  0.6* renderer->window_height_px);
		std::get<2>(*p) = 1;
	}
}

void BoardSystem::drawCharacters(RenderSystem* renderer,GameBoard* board) {
	// REQUIRES board->players < playerAssets.length()
	for(int i = 0; i < board->players; i++){
		std::tuple<Entity,vec2,int>* p = &board->player_positions[i];
		vec2 position = std::get<1>(*p);
		// Reserve en entity, notify game board
		auto entity = Entity();
		std::get<0>(*p) = entity;
		// Store a reference to the potentially re-used mesh object
		Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::CHARACTER);
		registry.meshPtrs.emplace(entity, &mesh);

		// Initialize the position, scale, and physics components
		auto& motion = registry.motions.emplace(entity);
		motion.angle = 0.f;
		motion.velocity = { 0.f, 0.f };
		//float xPos = position * GAME_TILE_SPACING - g.start_position

		motion.position = vec2(position.x * GAME_TILE_SPACING - board->start_position.x,position.y);
		motion.scale = mesh.original_size * (float)renderer->window_height_px * 0.2f;
		// Create an (empty) Player component to be able to refer to all players
		registry.playerTanks.emplace(entity);

		// Set sprite attributes to character
		Sprite& s = registry.sprites.emplace(entity);
		s.animation_speed = 50.0f;
		registry.renderRequests.insert(
			entity,
			{ playerAssets[i],
				EFFECT_ASSET_ID::CHARACTER,
				GEOMETRY_BUFFER_ID::CHARACTER });
		}
}

// queue particle effects near player
Entity BoardSystem::createEmitter(Entity player_target, vec2 offset,vec2 scale,GEOMETRY_BUFFER_ID mesh,vec2 velocity, int num) {
	Motion& pm = registry.motions.get(player_target);
	auto entity = Entity();
	
	// setup emitter motion
	vec2 pos = pm.position + offset;
	auto& m = registry.motions.emplace(entity);
	m.position = pos;
	m.scale = scale;

	// generate particle setup
	auto& manager = registry.particles.emplace(entity);
	for(int i = 0; i < num; i++){ // TODO make user changeable?
		vec2 position = vec2(std::uniform_real_distribution<float>{0.0f,1.0f}(this->generator));
		vec2 lifespan_time = vec2(std::uniform_int_distribution<int>{500,4500}(this->generator));
		// add a bit of noise to veclocity
		vec2 velocityR = vec2(std::uniform_real_distribution<float>{0.2f,1.0f}(this->generator),std::uniform_real_distribution<float>{0.2f,1.0f}(this->generator)) * velocity;
		manager.particles.push_back({position,lifespan_time,velocityR});
	}
	
	// make render request
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::PARTICLE, 
			mesh });
	return entity;
}

Entity BoardSystem::createSmokeEmitter(vec2 startingPos, vec2 offset) {
	auto entity = Entity();
	
	// // setup emitter motion
	// vec2 pos = startingPos + offset;
	// auto& m = registry.motions.emplace(entity);
	// m.position = pos;
	// m.scale = {50.0f, 50.0f};

	// particle.colourBegin = glm::vec4(0.8f, 0.7f, 0.7f, 0.8f);
	// particle.colourEnd = glm::vec4(0.1f, 0.1f, 0.1f, 0.0f);
	// particle.sizeBegin = 50.0f;
	// particle.sizeEnd = 15.0f;

	// // generate particle setup
	// auto& manager = registry.particles.emplace(entity);
	// for(int i = 0; i < 4000; i++){ // TODO make user changeable?
	// 	vec2 position = vec2(std::uniform_real_distribution<float>{0.0f,1.0f}(this->generator));
	// 	// vec2 lifespan_time = vec2(std::uniform_int_distribution<int>{500,4500}(this->generator));
	// 	vec2 lifespan_time = {10000, 10000};
	// 	// add a bit of noise to veclocity
	// 	vec2 velocityR = vec2(std::uniform_real_distribution<float>{0.2f,1.0f}(this->generator),std::uniform_real_distribution<float>{0.2f,1.0f}(this->generator)) * velocity;
	// 	manager.particles.push_back({position,lifespan_time,velocityR});
	// }
	
	// // make render request
	// registry.renderRequests.insert(
	// 	entity,
	// 	{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
	// 		EFFECT_ASSET_ID::PARTICLE, 
	// 		GEOMETRY_BUFFER_ID::SAND });
	return entity;
}

void BoardSystem::destroyEmitter(Entity e){
	// TODO verify all memory released
	registry.remove_all_components_of(e);
}

void BoardSystem::parallaxMovementUpdate() {
	Camera& camera = registry.camera.components[0];
	for (Entity a : registry.backgroundElements.entities) {
		SceneBackGroundComponent& c = registry.backgroundElements.get(a);
		Motion& m = registry.motions.get(a);
		float camerapos =  BoardSystem::cameraPositionX(camera.x,this->renderer->window_width_px);
		float shift = camerapos * (1.0f - c.offset);
		float travel = camerapos * (c.offset);
		m.position.x = c.startpos.x + travel;
		if (shift > c.startpos.x + this->window_width_px) {
			c.startpos += this->window_width_px*2.0f;
		}
		else if (shift < c.startpos.x - this->window_width_px) {
			c.startpos -= this->window_width_px*2.0f;
		}
	}
}

float BoardSystem::cameraPositionX(float camera_x, int width){
	float camerapos =  ((-camera_x * width/2.0f) + width / 2.0f);
	return camerapos;
}

void BoardSystem::generateBoardBackground(RenderSystem* renderer, GameBoard* board) {
// bulk generate background, render gets confused with it for now
	SceneBackGround active = board->backgrounds.at(board->active_background);
	for (int i = 0; i < active.elements.size(); i++) {
		auto entity = Entity();
		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(entity, &mesh);

		// Initialize the motion
		auto& motion = registry.motions.emplace(entity);
		motion.scale = vec2(active.elements.at(i).scale.x, active.elements.at(i).scale.y);
		motion.position =  vec2(active.elements.at(i).startpos.x, active.elements.at(i).startpos.y);
		
		// add to board background registry
		auto& sceneBackGroundComponent = registry.backgroundElements.emplace(entity);
		sceneBackGroundComponent.offset = active.elements.at(i).offset;
		sceneBackGroundComponent.startpos = active.elements.at(i).startpos;
		sceneBackGroundComponent.image = active.elements.at(i).image;
		sceneBackGroundComponent.scale = active.elements.at(i).scale;
		TEXTURE_ASSET_ID texture = active.elements.at(i).image;
		// render entity
		registry.renderRequests.insert(
			entity,
			{ texture,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}
}

void BoardSystem::generateBoardBackgroundOnce(RenderSystem* renderer, GameBoard* board, int i) {
	SceneBackGround active = board->backgrounds.at(board->active_background);
		auto entity = Entity();
		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(entity, &mesh);

		// Initialize the motion
		auto& motion = registry.motions.emplace(entity);
		motion.scale = vec2(active.elements.at(i).scale.x, active.elements.at(i).scale.y);
		motion.position =  vec2(active.elements.at(i).startpos.x, active.elements.at(i).startpos.y);
		
		// add to board background registry
		auto& sceneBackGroundComponent = registry.backgroundElements.emplace(entity);
		sceneBackGroundComponent.offset = active.elements.at(i).offset;
		sceneBackGroundComponent.startpos = active.elements.at(i).startpos;
		sceneBackGroundComponent.image = active.elements.at(i).image;
		sceneBackGroundComponent.scale = active.elements.at(i).scale;
		TEXTURE_ASSET_ID texture = active.elements.at(i).image;
		// render entity
		registry.renderRequests.insert(
			entity,
			{ texture,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
}

Entity BoardSystem::createGameTile(RenderSystem* renderer, GAMETILE_TYPE type, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = GAME_TILE_PERSPECTIVE_DISTORTION;

	// add to game tile registry
	auto& gameTile = registry.gameTiles.emplace(entity);
	gameTile.type = type;

	TEXTURE_ASSET_ID texture = TEXTURE_ASSET_ID::NEUTRAL_TILE;
	switch (type)
	{
	case GAMETILE_TYPE::NEUTRAL:
		texture = TEXTURE_ASSET_ID::NEUTRAL_TILE;
		break;
	case GAMETILE_TYPE::GAIN_TREASURE:
		texture = TEXTURE_ASSET_ID::TREASUREGAIN_TILE;
		break;
	case GAMETILE_TYPE::LOSE_TREASURE:
		texture = TEXTURE_ASSET_ID::TREASURELOSS_TILE;
		break;
	case GAMETILE_TYPE::GAIN_MOVEMENT:
		texture = TEXTURE_ASSET_ID::MOVEMENTGAIN_TILE;
		break;
	case GAMETILE_TYPE::LOSE_MOVEMENT:
		texture = TEXTURE_ASSET_ID::MOVEMENTLOSS_TILE;
		break;
	case GAMETILE_TYPE::SHOP:
		texture = TEXTURE_ASSET_ID::SHOP_TILE;
		break;
	case GAMETILE_TYPE::RANDOM_EVENT:
		texture = TEXTURE_ASSET_ID::RANDOM_TILE;
		break;
	default:
		break;
	}
	registry.renderRequests.insert(
		entity,
		{ texture,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity BoardSystem::createItem(RenderSystem* renderer, TEXTURE_ASSET_ID texture, vec2 pos, vec2 scale) {
	auto entity = Entity();
	
	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = scale;

	if (texture == TEXTURE_ASSET_ID::SHOP_BACKGROUND) {
		registry.shopBackground.emplace(entity);
	} else if (texture == TEXTURE_ASSET_ID::NOT_ENOUGH_TREASURE) {
		registry.notEnoughMoneyBanner.emplace(entity);
	} else {
		Items& item = registry.items.emplace(entity);
		item.item_type = texture;
	}

	registry.renderRequests.insert(
		entity,
		{ texture,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });
	
	return entity;
}

Entity BoardSystem::createTemple(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::TEMPLE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = {window_width_px / 2, window_height_px / 1.5};
	motion.scale.y *= -1;

	Temple& temple = registry.temple.emplace(entity);
	temple.rotationY = 0.0;
    registry.colors.insert(entity, {1.f, 0.73f, 0.f});

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::GOLD,
			GEOMETRY_BUFFER_ID::TEMPLE //TODO change back to temple
			});

	return entity;
}

void BoardSystem::createCoins(RenderSystem* renderer, float xPos, int numCoins)
{
	float yPos = 0 - (window_width_px / 14) / 2;
	for (int i = 0; i < numCoins; i++) {
		createCoin(renderer, {xPos, yPos});
		yPos -= (window_width_px / 14);
	}
}

void BoardSystem::createCoin(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::COIN);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = {window_width_px / 18, window_width_px / 46};

	Coins& coin = registry.coins.emplace(entity);
	coin.rotationY = 1.5708f;
	registry.colors.insert(entity, {1.f, 0.73f, 0.f});

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::GOLD,
			GEOMETRY_BUFFER_ID::COIN });
}

Entity BoardSystem::createKFAnimation(RenderSystem* renderer, float angle, vec2 scale, TEXTURE_ASSET_ID texture, std::vector<vec2> positions)
{
	auto entity = Entity();
	// Store a reference to the potentially re-used mesh object
	GEOMETRY_BUFFER_ID geometry = GEOMETRY_BUFFER_ID::SPRITE;
	if (texture == TEXTURE_ASSET_ID::TEXTURE_COUNT) {
		geometry = GEOMETRY_BUFFER_ID::COIN;
	}
	Mesh& mesh = renderer->getMesh(geometry);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = positions[0];
	motion.angle = angle;
	motion.velocity = { 0.f, 0.f };
	motion.scale = scale;

	// Create and (empty) Salmon component to be able to refer to all turtles
	EFFECT_ASSET_ID effect = EFFECT_ASSET_ID::TEXTURED;
	KeyframeAnimation& keyframeAnimation = registry.keyframeAnimation.emplace(entity);
	switch(texture) {
		case TEXTURE_ASSET_ID::LIGHTENING:
			keyframeAnimation.kfColours.push_back({1.f, 0.73f, 0.f});
			keyframeAnimation.kfColours.push_back({1.f, 1.f, 0.f});
			keyframeAnimation.kfColours.push_back({0.49f, 0.98f, 1.f});
			keyframeAnimation.lifeRemaining = keyframeAnimation.lifeTimeMs;
			keyframeAnimation.larpTimeMs = keyframeAnimation.lifeTimeMs / 2;
			keyframeAnimation.larpRemaining = keyframeAnimation.lifeTimeMs / 2;
    		registry.colors.insert(entity, keyframeAnimation.kfColours.at(0));
			break;
		case TEXTURE_ASSET_ID::TEXTURE_COUNT:
			keyframeAnimation.kfPositions = positions;
    		registry.colors.insert(entity, {1.f, 0.73f, 0.f});
			keyframeAnimation.lifeTimeMs = 5000.f;
			keyframeAnimation.kfRotations.push_back(1.5708f);
			keyframeAnimation.kfRotations.push_back(0.0f);
			keyframeAnimation.lifeRemaining = keyframeAnimation.lifeTimeMs;
			keyframeAnimation.larpTimeMs = keyframeAnimation.lifeTimeMs / 2;
			keyframeAnimation.larpRemaining = keyframeAnimation.lifeTimeMs / 2;
			effect = EFFECT_ASSET_ID::GOLD;
			break;
		default:
			break;
	}
	keyframeAnimation.type = texture;

	registry.renderRequests.insert(
		entity,
		{ texture,
			effect,
			geometry });

	return entity;
}

void BoardSystem::deconstructGameBoard(GameBoard* board) {
	GameBoard& g = registry.gameBoard.get(board_controller);
	while (registry.gameTiles.entities.size() > 0)
		registry.remove_all_components_of(registry.gameTiles.entities.back());

	while (registry.backgroundElements.entities.size() > 0)
		registry.remove_all_components_of(registry.backgroundElements.entities.back());

	while (registry.dice.entities.size() > 0)
		registry.remove_all_components_of(registry.dice.entities.back());

	while (registry.playerTanks.entities.size() > 0)
		registry.remove_all_components_of(registry.playerTanks.entities.back());

	while (registry.temple.entities.size() > 0)
		registry.remove_all_components_of(registry.temple.entities.back());
	
	while (registry.poisoned.entities.size() > 0)
		registry.remove_all_components_of(registry.poisoned.entities.back());

	// dialogue isn't normally removed/created along with board game
	// but in case player exits game and thus is out of dialogue early, this is a failsafe to ensure the components are removed
	while (registry.dialogue.entities.size() > 0)
		registry.remove_all_components_of(registry.dialogue.entities.back());
	destroyEmitter(walkEmitter);
	g.loaded = false;
}


Entity BoardSystem::createDie(RenderSystem* renderer, vec2 position) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BOARD_DIE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = M_PI/180.0f*-90.0f;
	motion.velocity = { 0.f, 0.f };
	
	motion.scale = vec2(-1.0f * renderer->window_width_px * 0.05f, renderer->window_width_px * 0.05f) ;

	GameDie& dice = registry.dice.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
			EFFECT_ASSET_ID::DICE, 
			GEOMETRY_BUFFER_ID::BOARD_DIE });

	return entity;
}

void BoardSystem::displayScoreboard() {
	GameBoard& g = registry.gameBoard.get(board_controller);
	float x = (window_width_px / 2) + 150;
	TEXTURE_ASSET_ID texture = TEXTURE_ASSET_ID::SCOREBOARD_1;
	float scale_x = window_width_px / 9;
	float scale_y = window_height_px / 8;
	float spacing = window_width_px / 8;
	for (int i = 0; i < g.players; i++) {
		switch (i)
		{
		case 0:
			texture = TEXTURE_ASSET_ID::SCOREBOARD_1;
			break;
		case 1:
			texture = TEXTURE_ASSET_ID::SCOREBOARD_2;
			break;
		case 2:
			texture = TEXTURE_ASSET_ID::SCOREBOARD_3;
			break;
		case 3:
			texture = TEXTURE_ASSET_ID::SCOREBOARD_4;
			break;
		default:
			break;
		}
		auto entity = Entity();

		// Store a reference to the potentially re-used mesh object
		Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
		registry.meshPtrs.emplace(entity, &mesh);

		// Setting initial motion values
		Motion& motion = registry.motions.emplace(entity);
		motion.position = vec2(x, window_height_px / 8);
		motion.angle = 0.f;
		motion.velocity = { 0.f, 0.f };
		motion.scale.x = scale_x;
		motion.scale.y = scale_y;

		Board& b = registry.board.emplace(entity);
		b.x_pos_offset = x - (window_width_px / 2);

		registry.renderRequests.insert(
			entity,
			{ texture,
				EFFECT_ASSET_ID::TEXTURED, 
				GEOMETRY_BUFFER_ID::SPRITE });
		
		displayScoreNumber(x, i, {scale_x, scale_y});
		x += spacing;
	}
}

void BoardSystem::displayScoreNumber(float x_pos, int player, vec2 boardScale) {
	GameBoard& g = registry.gameBoard.get(board_controller);
	unsigned int number;
	int digit;
	vec2 number_scale = boardScale / 1.2f;
	float number_spacing = number_scale.x  * 0.2f;
	TEXTURE_ASSET_ID texture;
	number = g.player_points[player];
	if (number < 10) {
		texture = getTexture(number);
		drawNumber(x_pos + (number_spacing * 1.5), texture, number_scale);
	}
	if (number >= 10) {
		digit = number%10;
		texture = getTexture(digit);
		drawNumber(x_pos + (number_spacing * 1.5), texture, number_scale);
		number /= 10;
		digit = number%10;
		texture = getTexture(digit);
		drawNumber(x_pos - (number_spacing * 0.5), texture, number_scale);
	}
}

TEXTURE_ASSET_ID BoardSystem::getTexture(int digit) {
	TEXTURE_ASSET_ID texture = TEXTURE_ASSET_ID::NUMBER_0;
	switch (digit)
	{
	case 0:
		texture = TEXTURE_ASSET_ID::NUMBER_0;
		break;
	case 1:
		texture = TEXTURE_ASSET_ID::NUMBER_1;
		break;
	case 2:
		texture = TEXTURE_ASSET_ID::NUMBER_2;
		break;
	case 3:
		texture = TEXTURE_ASSET_ID::NUMBER_3;
		break;
	case 4:
		texture = TEXTURE_ASSET_ID::NUMBER_4;
		break;
	case 5:
		texture = TEXTURE_ASSET_ID::NUMBER_5;
		break;
	case 6:
		texture = TEXTURE_ASSET_ID::NUMBER_6;
		break;
	case 7:
		texture = TEXTURE_ASSET_ID::NUMBER_7;
		break;
	case 8:
		texture = TEXTURE_ASSET_ID::NUMBER_8;
		break;
	case 9:
		texture = TEXTURE_ASSET_ID::NUMBER_9;
		break;
	default:
		break;
	}
	return texture;
}

void BoardSystem::drawNumber(float x_pos, TEXTURE_ASSET_ID texture, vec2 numberScale) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = vec2(x_pos, window_height_px / 8);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = numberScale;
	motion.scale.x *= 0.5;

	ScoreNumber& sn = registry.scoreNumber.emplace(entity);
	sn.x_pos_offset = x_pos - (window_width_px / 2);

	registry.renderRequests.insert(
		entity,
		{ texture,
			EFFECT_ASSET_ID::TEXTURED, 
			GEOMETRY_BUFFER_ID::SPRITE });
}

Entity BoardSystem::drawNumberOnly(vec2 pos, TEXTURE_ASSET_ID texture) {
	float scale_x = (window_width_px / 9.f) / 1.2f;
	float scale_y = (window_height_px / 8.f) / 1.2f;
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale.x = scale_x;
	motion.scale.y = scale_y;
	motion.scale.x *= 0.5;

	registry.renderRequests.insert(
		entity,
		{ texture,
			EFFECT_ASSET_ID::TEXTURED, 
			GEOMETRY_BUFFER_ID::SPRITE });
	return entity;
}


void BoardSystem::removeScoreboard() {
	while (registry.board.entities.size() > 0)
		registry.remove_all_components_of(registry.board.entities.back());

	while (registry.scoreNumber.entities.size() > 0)
		registry.remove_all_components_of(registry.scoreNumber.entities.back());
}

void BoardSystem::scoreboardMovementUpdate(float x_pos) {
	Camera& camera = registry.camera.components[0];
	for (Entity board : registry.board.entities) {
		Board& b = registry.board.get(board);
		Motion& b_m = registry.motions.get(board);
		b_m.position.x = x_pos + b.x_pos_offset;
	}
	for (Entity numbers : registry.scoreNumber.entities) {
		ScoreNumber& s = registry.scoreNumber.get(numbers);
		Motion& n_m = registry.motions.get(numbers);
		n_m.position.x = x_pos + s.x_pos_offset;
	}
}
Entity BoardSystem::createDialogue(TEXTURE_ASSET_ID dialogueType, int nextFrame) {
	float scaleOfDialogue = window_width_px > window_height_px ? window_height_px / 4 : window_width_px / 5;

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::DIALOGUE);
	// Reserve an entity for dialogue box, and for NPC sprite
	auto entity = Entity();

	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motionOne = registry.motions.emplace(entity);
	motionOne.scale = mesh.original_size * scaleOfDialogue;
	motionOne.scale.x *= 3.5f;

	// place dialogue at default bottom centre of screen
	motionOne.position.x = window_width_px / 2.0f;
	motionOne.position.y = window_height_px - window_height_px / 20 - motionOne.scale.y / 2;

	// Set sprite attributes to dialogue
	Sprite& sprite = registry.sprites.emplace(entity);
	sprite.animation_speed = 0;

	// set dialogue component so we can refer to it later
	Dialogue& dialogue = registry.dialogue.emplace(entity);
	dialogue.nextDialogueFrame = nextFrame;
	// set the type of dialogue based on asset passed in
	if (dialogueType == TEXTURE_ASSET_ID::INTRO_SPRITESHEET) {
		dialogue.dialogueType = 0;
	}
	else if (dialogueType == TEXTURE_ASSET_ID::ENDING_SPRITESHEET) {
		dialogue.dialogueType = 1;
		Camera& camera = registry.camera.components[0];
		float mouseShiftX = -camera.x * window_width_px * 0.5f;
		motionOne.position.x += mouseShiftX;
	}
	else if (dialogueType == TEXTURE_ASSET_ID::TUTORIAL_SPRITESHEET) {
		dialogue.dialogueType = 2;
		sprite.num_frames = 8;
	}
	if (dialogueType == TEXTURE_ASSET_ID::INTRO_SPRITESHEET || dialogueType == TEXTURE_ASSET_ID::ENDING_SPRITESHEET) {
		registry.renderRequests.insert(
			entity,
			{ dialogueType,
				EFFECT_ASSET_ID::DIALOGUE,
				GEOMETRY_BUFFER_ID::DIALOGUE });
	}
	// tutorial specifically uses a diff geometry & effect
	else if (dialogueType == TEXTURE_ASSET_ID::TUTORIAL_SPRITESHEET) {
		registry.renderRequests.insert(
			entity,
			{ dialogueType,
				EFFECT_ASSET_ID::TUTORIAL,
				GEOMETRY_BUFFER_ID::TUTORIAL });
	}
	return entity;
}
Entity BoardSystem::createDialogueSprite(Motion& dialogueMotion) {
	float scaleOfNPC = window_width_px > window_height_px ? window_height_px / 6 : window_width_px / 9;
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::DIALOGUE);
	// Reserve an entity for NPC sprite
	auto entity = Entity();
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motionOne = registry.motions.emplace(entity);
	motionOne.scale = mesh.original_size * scaleOfNPC;
	motionOne.scale.y *= 1.3f;

	// place NPC sprite to the right, slightly above dialogue
	motionOne.position.x = dialogueMotion.position.x + dialogueMotion.scale.x/2.f + motionOne.scale.x/2;
	motionOne.position.y = dialogueMotion.position.y - motionOne.scale.y / 4;

	// Set sprite attributes to dialogue sprite
	Sprite& sprite = registry.sprites.emplace(entity);
	sprite.animation_speed = 0;
	// NPC has mustache... see if shop will be using sprite w/ mustache, else just use this one
	sprite.frame.x = 1;

	// set dialogue component so we can refer to it later
	Dialogue& dialogue = registry.dialogue.emplace(entity);

	// create render requests for dialogue box & NPC
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TOADY_SPRITESHEET,
			EFFECT_ASSET_ID::DIALOGUE,
			GEOMETRY_BUFFER_ID::DIALOGUE });

	return entity;
}

BoardSystem::~BoardSystem() {
	// Destroy music components
	if (tile_pass_ring != nullptr)
		Mix_FreeChunk(this->tile_pass_ring);
	if (dice_big_roll_sound != nullptr)
		Mix_FreeChunk(dice_big_roll_sound);
	Mix_CloseAudio();
}
