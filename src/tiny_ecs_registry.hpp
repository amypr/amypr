#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface*> registry_list;

public:
	// Manually created list of all components this game has
	// TODO: A1 add a LightUp component
	ComponentContainer<DeathTimer> deathTimers;
	ComponentContainer<StunTimer> stunTimers;
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<PlayerTank> playerTanks;
	ComponentContainer<Sprite> sprites;
	ComponentContainer<Mesh*> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<DrawingCharacterWriting> drawingCharacterWriting;
	ComponentContainer<Trap> traps;
	ComponentContainer<Platformer> platformer;
	ComponentContainer<Platform> platforms;
	ComponentContainer<PlatformerPhysics> platformerPhysics;
    ComponentContainer<TankGame> tankGame;
    ComponentContainer<AIBullet> aiBullets;
    ComponentContainer<PlayerBullet> playerBullets;
    ComponentContainer<Wall> walls;
    ComponentContainer<AITank> aiTanks;
    ComponentContainer<TankGamePoint> tankGamePoints;
    ComponentContainer <Ai_Info> ai_Info;
	ComponentContainer<Goal> goals;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<vec3> colors;
	ComponentContainer<GameBoard> gameBoard;
	ComponentContainer<GameTile> gameTiles;
	ComponentContainer<SceneBackGroundComponent> backgroundElements;
	ComponentContainer<GameDie> dice;
	ComponentContainer<Camera> camera;
	ComponentContainer<DrawingGame> drawingGame;
	ComponentContainer<BackgroundColour> backgroundColour;
	ComponentContainer<Drawing> drawing;
	ComponentContainer<NumberCharacters> numberCharacters;
	ComponentContainer<Board> board;
	ComponentContainer<ScoreNumber> scoreNumber;
	ComponentContainer<DrawingTimer> drawingTimer;
	ComponentContainer<Menu> menu;
	ComponentContainer<Buttons> buttons;
	ComponentContainer<StartScreen> startScreen;
	ComponentContainer<StartScreenBackground> startScreenBackground;
	ComponentContainer<MouseDraw> mouseDraw;
	ComponentContainer<MouseHold> mouseHold;
	ComponentContainer<HelpScreen> helpScreen;
	ComponentContainer<Items> items;
	ComponentContainer<ShopBackground> shopBackground;
	ComponentContainer<NotEnoughMoneyBanner> notEnoughMoneyBanner;
	ComponentContainer<Dialogue> dialogue;
	ComponentContainer<TooManyItems> tooManyItems;
	ComponentContainer<Temple> temple;
    ComponentContainer<LightUp> lightUps;
	ComponentContainer<HardWall> hardWall;
	ComponentContainer<TankHome> tankHome;
	ComponentContainer<Physics> physics;
    ComponentContainer<Tanking> tankings;
    ComponentContainer<KeyframeAnimation> keyframeAnimation;
    ComponentContainer<CoinInfo> coinInfo;
    ComponentContainer<Poisoned> poisoned;
    ComponentContainer<Coins> coins;
	ComponentContainer<Particle_Manager> particles;
	ComponentContainer<Light> light;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		// TODO: A1 add a LightUp component
		registry_list.push_back(&deathTimers);
		registry_list.push_back(&stunTimers);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&playerTanks);
		registry_list.push_back(&sprites);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&drawingCharacterWriting);
		registry_list.push_back(&traps);
		registry_list.push_back(&platformer);
		registry_list.push_back(&platforms);
		registry_list.push_back(&platformerPhysics);
        registry_list.push_back(&tankGame);
        registry_list.push_back(&aiBullets);
        registry_list.push_back(&playerBullets);
        registry_list.push_back(&walls);
        registry_list.push_back(&aiTanks);
        registry_list.push_back(&tankGamePoints);
        registry_list.push_back(&ai_Info);
		registry_list.push_back(&goals);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);
		registry_list.push_back(&gameBoard);
		registry_list.push_back(&gameTiles);
		registry_list.push_back(&backgroundElements);
		registry_list.push_back(&dice);
		registry_list.push_back(&camera);
		registry_list.push_back(&drawingGame);
		registry_list.push_back(&backgroundColour);
		registry_list.push_back(&drawing);
		registry_list.push_back(&numberCharacters);
		registry_list.push_back(&board);
		registry_list.push_back(&scoreNumber);
		registry_list.push_back(&drawingTimer);
		registry_list.push_back(&menu);
		registry_list.push_back(&buttons);
		registry_list.push_back(&startScreen);
		registry_list.push_back(&startScreenBackground);
		registry_list.push_back(&mouseDraw);
		registry_list.push_back(&mouseHold);
		registry_list.push_back(&helpScreen);
		registry_list.push_back(&items);
		registry_list.push_back(&shopBackground);
		registry_list.push_back(&notEnoughMoneyBanner);
		registry_list.push_back(&dialogue);
		registry_list.push_back(&tooManyItems);
		registry_list.push_back(&temple);
        registry_list.push_back(&lightUps);
		registry_list.push_back(&hardWall);
		registry_list.push_back(&tankHome);
		registry_list.push_back(&physics);
        registry_list.push_back(&tankings);
        registry_list.push_back(&keyframeAnimation);
        registry_list.push_back(&coinInfo);
        registry_list.push_back(&poisoned);
        registry_list.push_back(&coins);
		registry_list.push_back(&particles);
		registry_list.push_back(&light);
	}

	void clear_all_components() {
		for (ContainerInterface* reg : registry_list)
			reg->clear();
	}

	void list_all_components() {
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface* reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e) {
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface* reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e) {
		for (ContainerInterface* reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;
