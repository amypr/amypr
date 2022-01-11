#pragma once

#include "components.hpp"
#include "tiny_ecs_registry.hpp"
#include <random>
// turn start select, choose to roll now or use an item

// at shop screen, choose to purchase item and what kind of item

// react to random event (question mark space), which player to swap positions with, which player to steal points from, etc

// enum of states


namespace Board_FSM {
	static std::default_random_engine gen;
	// Locates appropreate state and passes current input
	BOARD_AI_STATE process(int player_id, BOARD_AI_STATE pState, GameBoard* g, int data, void* Boardsystem);
	BOARD_AI_STATE beginTurn(int player_id, GameBoard* g);
	BOARD_AI_STATE selectItem(int player_id, GameBoard* g);
	BOARD_AI_STATE roll(int player_id, GameBoard* g, int strength);
	BOARD_AI_STATE arrive(int player_id, GameBoard* g, int value);
	BOARD_AI_STATE choose_swap(int player_id, GameBoard* g);
	BOARD_AI_STATE choose_steal(int player_id, GameBoard* g, int value);
	BOARD_AI_STATE random_event(int player_id, GameBoard* g, int value, void* Boardsystem);
	void pSwap(int p1, int p2, GameBoard* g);
	void pSteal(int p1, int p2, int value, GameBoard* g, void* b);
	bool pCoins(GameBoard* g);
	bool pTransport(GameBoard* g);
	void removeItemsInShop();
};