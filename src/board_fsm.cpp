#include "board_fsm.hpp"
#include "board_system.hpp"
#include "components.hpp"


namespace Board_FSM 
{
	BOARD_AI_STATE process(int player_id, BOARD_AI_STATE pState, GameBoard* g, int data, void* Boardsystem) {
		BOARD_AI_STATE state = pState;
		// BoardSystem* sys = (BoardSystem*) Boardsystem;
		switch (pState)
		{
		case BOARD_AI_STATE::TURN_END:
			break;
		case BOARD_AI_STATE::TURN_START:
			state = beginTurn(player_id,g);
			break;
		case BOARD_AI_STATE::SELECTING_ITEM:
			state = selectItem(player_id, g);
			break;
		case BOARD_AI_STATE::ROLLING:
			state = roll(player_id, g,data);
			break;
		case BOARD_AI_STATE::MOVING:
			break;
		case BOARD_AI_STATE::REACT_TO_TILE:
			state = arrive(player_id, g, data);
			break;
		case BOARD_AI_STATE::REACT_TO_RAND_EVENT:
			state = random_event(player_id,g, data, Boardsystem);
			break;
		case BOARD_AI_STATE::CHOOSE_PLAYER_SWAP:
			state = choose_swap(player_id, g);
			break;
		case BOARD_AI_STATE::CHOOSE_PLAYER_STEAL:
			state = choose_steal(player_id, g, data);
			break;
		default:
			break;
		}
		g->playerStates[player_id] = state;
		return state;
	}

	BOARD_AI_STATE random_event(int player_id, GameBoard* g, int value, void* Boardsystem ){
		BOARD_AI_STATE target;
		
		int p2 = -1;
		switch(value){
			case 1: //steal
				printf("ai stealing \n");
				target = choose_steal(player_id,g,5);
				
				p2 = (int)target - (int)BOARD_AI_STATE::SELECT_NONE -1;
				if(p2>=0) {
					printf("%d stealing from %d \n",player_id ,p2);
					pSteal(player_id,(int)target - (int)BOARD_AI_STATE::SELECT_NONE,5,g, Boardsystem);
				}
				break;
			case 2: // swap
				printf("ai swapping \n");
				target = choose_swap(player_id,g);
				p2 = (int)target - (int)BOARD_AI_STATE::SELECT_NONE -1;
				
				if(p2>=0) {
					printf("%d swapping from %d \n", player_id,p2);
					pSwap(player_id,(int)target- (int)BOARD_AI_STATE::SELECT_NONE,g);
				}
				break;				
		}
		return BOARD_AI_STATE::TURN_END;
	}

	BOARD_AI_STATE beginTurn(int player_id, GameBoard* g) {
		if (g->playerInv[player_id].size() > 0) {
			// maybe choose to select item with random prob
			g->stage = 2;
			return BOARD_AI_STATE::SELECTING_ITEM;
		}
		else { // Player has no items and doesn't care about edge scrolling, only can roll
			g->stage = 9;
			return BOARD_AI_STATE::ROLLING;
		}
	}

	BOARD_AI_STATE selectItem(int player_id, GameBoard* g) {
		if (g->stage == 2 && g->active_item[g->turn] == TEXTURE_ASSET_ID::BOARD_HELP) {
			for (TEXTURE_ASSET_ID item : g->playerInv[g->turn]) {
				if (item == TEXTURE_ASSET_ID::INV_DICE || item == TEXTURE_ASSET_ID::INV_POISON ||
					item == TEXTURE_ASSET_ID::INV_TRANSPORT) {
					if (pTransport(g)) {
						g->active_item[g->turn] = item;
						return BOARD_AI_STATE::ROLLING;
					}
				}
				if (item == TEXTURE_ASSET_ID::INV_HEART || item == TEXTURE_ASSET_ID::INV_LIGHTENING ||
					item == TEXTURE_ASSET_ID::INV_TREASURE_1 || item == TEXTURE_ASSET_ID::INV_TREASURE_2) {
					if (pCoins(g)) {
						g->active_item[g->turn] = item;
						return BOARD_AI_STATE::ROLLING;
					}
				}
			}
			return BOARD_AI_STATE::ROLLING;
		} else if (g->stage == 2) {
			return BOARD_AI_STATE::ROLLING;
		} else {
			if (g->playerInv[g->turn].size() >= 9) {
				removeItemsInShop();
				return BOARD_AI_STATE::TURN_END;
			}
			TEXTURE_ASSET_ID defaultItem = TEXTURE_ASSET_ID::INV_HEART;
			for (int i = 0; i < (int)registry.items.components.size(); i++) {
				Items& item = registry.items.components[i];
				if (item.item_type == TEXTURE_ASSET_ID::EXIT_BUTTON) {
					continue;
				}
				switch(item.item_type) {
					case TEXTURE_ASSET_ID::DICE_ITEM:
						if (pTransport(g)) {
							g->playerInv[g->turn].push_back(TEXTURE_ASSET_ID::INV_DICE);
							removeItemsInShop();
							return BOARD_AI_STATE::TURN_END;
						}
						defaultItem = TEXTURE_ASSET_ID::INV_DICE;
						break;
					case TEXTURE_ASSET_ID::HEART_ITEM:
						if (pCoins(g)) {
							g->playerInv[g->turn].push_back(TEXTURE_ASSET_ID::INV_HEART);
							removeItemsInShop();
							return BOARD_AI_STATE::TURN_END;
						}
						defaultItem = TEXTURE_ASSET_ID::INV_HEART;
						break;
					case TEXTURE_ASSET_ID::LIGHTENING_ITEM:
						if (pCoins(g)) {
							g->playerInv[g->turn].push_back(TEXTURE_ASSET_ID::INV_LIGHTENING);
							removeItemsInShop();
							return BOARD_AI_STATE::TURN_END;
						}
						defaultItem = TEXTURE_ASSET_ID::INV_LIGHTENING;
						break;
					case TEXTURE_ASSET_ID::POISON_ITEM:
						if (pTransport(g)) {
							g->playerInv[g->turn].push_back(TEXTURE_ASSET_ID::INV_POISON);
							removeItemsInShop();
							return BOARD_AI_STATE::TURN_END;
						}
						defaultItem = TEXTURE_ASSET_ID::INV_POISON;
						break;
					case TEXTURE_ASSET_ID::TRANSPORT_ITEM:
						if (pTransport(g)) {
							g->playerInv[g->turn].push_back(TEXTURE_ASSET_ID::INV_TRANSPORT);
							removeItemsInShop();
							return BOARD_AI_STATE::TURN_END;
						}
						defaultItem = TEXTURE_ASSET_ID::INV_TRANSPORT;
						break;
					case TEXTURE_ASSET_ID::TREASURE_1_ITEM:
						if (pCoins(g)) {
							g->playerInv[g->turn].push_back(TEXTURE_ASSET_ID::INV_TREASURE_1);
							removeItemsInShop();
							return BOARD_AI_STATE::TURN_END;
						}
						defaultItem = TEXTURE_ASSET_ID::INV_TREASURE_1;
						break;
					case TEXTURE_ASSET_ID::TREASURE_2_ITEM:
						if (pCoins(g)) {
							g->playerInv[g->turn].push_back(TEXTURE_ASSET_ID::INV_TREASURE_2);
							removeItemsInShop();
							return BOARD_AI_STATE::TURN_END;
						}
						defaultItem = TEXTURE_ASSET_ID::INV_TREASURE_2;
						break;
					default:
						break;
				}
			}
			// TODO once items exist
			g->playerInv[g->turn].push_back(defaultItem);
			removeItemsInShop();
			return BOARD_AI_STATE::TURN_END; // change to end turn
		}
	}

	BOARD_AI_STATE roll(int player_id, GameBoard* g, int strength) {
		int diceIteration = 0;
		for (Entity e : registry.dice.entities) {
			GameDie& d = registry.dice.get(e);
			if (registry.motions.has(e) ) {
				Motion& m = registry.motions.get(e);
				d.angular_velocity = 3.0f;
				vec2 pos = m.position;
				d.velocity.x = (vec2(strength)).x;
				m.position = pos;
				diceIteration++;
				d.controlled = false;
				d.rolling = true;
			}
		}
		return BOARD_AI_STATE::MOVING;
	}
	BOARD_AI_STATE arrive(int player_id, GameBoard* g, int value) {
		printf("arriving");
		if (g->config[(int)std::get<1>(g->player_positions[g->turn]).x - 1].type == GAMETILE_TYPE::SHOP) {
			selectItem(player_id, g);
		}
		// TODO when items exist
		return BOARD_AI_STATE::TURN_END;
	}
	BOARD_AI_STATE choose_swap(int player_id, GameBoard* g) {
		// choose player that is furthest ahead, select none if all behind 
		int maxDist = 0;
		int maxDistId = -1;
		for (int p = 0; p < g->players; p++) {
			if (p != player_id) {
				if (std::get<1>(g->player_positions[p]).x > maxDist) {
					maxDist = std::get<1>(g->player_positions[p]).x;
					maxDistId = p;
				}
			}
		}
		if (maxDistId != -1) {
			return BOARD_AI_STATE::SELECT_NONE;
		}
		else {
			switch (maxDistId)
			{
			case 0:
				return BOARD_AI_STATE::SELECT_PLAYER_1;
				break;
			case 1:
				return BOARD_AI_STATE::SELECT_PLAYER_2;
				break;
			case 2:
				return BOARD_AI_STATE::SELECT_PLAYER_3;
				break;
			case 3:
				return BOARD_AI_STATE::SELECT_PLAYER_4;
				break;
			default:
				return BOARD_AI_STATE::SELECT_NONE;
				break;
			}
			
		}
		
	}
	BOARD_AI_STATE choose_steal(int player_id, GameBoard* g, int value) {
		// choose randomly from eligible players with slightly more preference to the players ahead
		
		std::vector<int> bag {};
		int points = g->player_points[player_id];
		for (int p = 0; p < g->players; p++) {
			if (p != player_id) {
				if (g->player_points[p] >= value) {
					// can be drawn to steal from
					bag.push_back(p);
					// check if player is ahead of stealer, if so then increase odds of stealing from them
					if(g->player_points[p] > points){
						bag.push_back(p);
					}
				}
			}
		}

		if(bag.size() > 0){
			// draw
			int val = std::uniform_int_distribution<int>{0, ((int)bag.size() - 1)}(gen);
			int target = bag.at(val);
			switch (target)
			{
			case 0:
				return BOARD_AI_STATE::SELECT_PLAYER_1;
				break;
			case 1:
				return BOARD_AI_STATE::SELECT_PLAYER_2;
				break;
			case 2:
				return BOARD_AI_STATE::SELECT_PLAYER_3;
				break;
			case 3:
				return BOARD_AI_STATE::SELECT_PLAYER_4;
				break;		
			}
		}
		return BOARD_AI_STATE::SELECT_NONE;
	}

	void pSwap(int p1, int p2, GameBoard* g){
		// swap positions, both in tuple locations, and motion
		if(p1 != p2){
			vec2 p1_mPos = registry.motions.get(std::get<0>(g->player_positions[p1])).position;
			vec2 p1_pos = std::get<1>(g->player_positions[p1]);
			int p1_pos2 = std::get<2>(g->player_positions[p1]);
			// set p1 to p2
			*(&std::get<2>(g->player_positions[p1])) = std::get<2>(g->player_positions[p2]);
			*(&std::get<1>(g->player_positions[p1])) = std::get<1>(g->player_positions[p2]);
			registry.motions.get(std::get<0>(g->player_positions[p1])).position.x = registry.motions.get(std::get<0>(g->player_positions[p2])).position.x;
			// set p2 to cached p1
			*(&std::get<1>(g->player_positions[p2])) = p1_pos;
			*(&std::get<2>(g->player_positions[p2])) = p1_pos2;
			registry.motions.get(std::get<0>(g->player_positions[p2])).position.x = p1_mPos.x;
		}
	}

	void pSteal(int p1, int p2, int value, GameBoard* g, void* b){
	// steal points, subtract points from p1 and add to p2
		BoardSystem* sys = (BoardSystem*) b;
		if(p1 != p2){
			g->player_points[p1] -= value;
			g->player_points[p2] += value;
			sys->removeScoreboard();
			sys->displayScoreboard();
		}
	}

	bool pCoins(GameBoard* g){
	// determine if the ai player needs treasure
		if (g->player_points[g->turn] == 0) {
			return true;
		}
		bool lt1 = false;
		bool lt2 = false;
		bool lt3 = false;
		for (int i = 0; i < g->players; i++) {
			if (i == g->turn) {
				continue;
			}
			if (g->player_points[g->turn] < g->player_points[i]){
				lt1 = (i == 0) ? true : false;
				lt2 = (i == 1) ? true : false;
				lt3 = (i == 2 || i == 3) ? true : false;
			}
		}
		if (lt1 && lt2 && lt3) {
			return true;
		}
		if (lt1 && lt2) {
			return true;
		}
		if (lt1 && lt3) {
			return true;
		}
		if (lt2 && lt3) {
			return true;
		}
		return false;
	}

	bool pTransport(GameBoard* g){
	// determine if the ai player needs treasure
		bool lt1 = false;
		bool lt2 = false;
		bool lt3 = false;
		for (int i = 0; i < g->players; i++) {
			if (i == g->turn) {
				continue;
			}
			if (std::get<1>(g->player_positions[g->turn]).x < std::get<1>(g->player_positions[i]).x){
				lt1 = (i == 0) ? true : false;
				lt2 = (i == 1) ? true : false;
				lt3 = (i == 2 || i == 3) ? true : false;
			}
		}
		if (lt1 && lt2 && lt3) {
			return true;
		}
		if (lt1 && lt2) {
			return true;
		}
		if (lt1 && lt3) {
			return true;
		}
		if (lt2 && lt3) {
			return true;
		}
		return false;
	}

	void removeItemsInShop(){
	// remove shop for ai
		while (registry.items.entities.size() > 0) {
			registry.remove_all_components_of(registry.items.entities.back());
		}
		while (registry.shopBackground.entities.size() > 0) {
			registry.remove_all_components_of(registry.shopBackground.entities.back());
		}
		while (registry.notEnoughMoneyBanner.entities.size() > 0) {
			registry.remove_all_components_of(registry.notEnoughMoneyBanner.entities.back());
		}
	}
}



