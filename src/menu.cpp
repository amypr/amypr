#include "menu.hpp"

void MenuSystem::init(RenderSystem* renderer, StartSystem& startSys, int window_width_px, int window_height_px) {
	this->window_width_px = window_width_px;
	this->window_height_px = window_height_px;
	this->renderer = renderer;
	this->startSystem = startSys;
	registry.menu.emplace(board_controller);
}

void MenuSystem::buildMenu() {
	Menu& menu = registry.menu.get(board_controller);
    float window_third = window_height_px / 3.33f;
	float distanceBtwnButtons = window_height_px / 5.333f;
	float camerapos = cameraPositionX();
	if (menu.menuType == 1) {
		GameBoard& g = registry.gameBoard.get(board_controller);
		DrawingGame& drawingGame = registry.drawingGame.get(board_controller);
		Platformer& platformer = registry.platformer.get(board_controller);
		TankGame& tankGame = registry.tankGame.get(board_controller);
		if (g.active) {
			helpScreens(TEXTURE_ASSET_ID::BOARD_HELP);
		}
		else if (drawingGame.active) {
			helpScreens(TEXTURE_ASSET_ID::DRAWING_HELP);
		}
		else if (platformer.active) {
			helpScreens(TEXTURE_ASSET_ID::PLATFORMER_HELP);
		}
		else if (tankGame.active) {
            if (tankGame.level == 1) {
                helpScreens(TEXTURE_ASSET_ID::TANK_HELP_1);
            } else if (tankGame.level == 2) {
                helpScreens(TEXTURE_ASSET_ID::TANK_HELP_2);
            }
		}
	}
	else if (menu.menuType == 0) {
		help_button = startSystem.createButton(renderer, vec2(camerapos, distanceBtwnButtons), TEXTURE_ASSET_ID::HELP_BUTTON);
		save_and_exit_button = startSystem.createButton(renderer, vec2(camerapos, distanceBtwnButtons + window_third), TEXTURE_ASSET_ID::SAVE_BUTTON);
		exit_button = startSystem.createButton(renderer, vec2(camerapos, distanceBtwnButtons + 2 * window_third), TEXTURE_ASSET_ID::EXIT_BUTTON);
	}
	
    // BackgroundColour &bgc = registry.backgroundColour.components[0];
    // bgc.red = .5;
    // bgc.green = .5;
    // bgc.blue = .5;
	menu.active = true;
}

void MenuSystem::deconstructMenu() {
	Menu& menu = registry.menu.get(board_controller);
	while (registry.buttons.entities.size() > 0) {
		registry.remove_all_components_of(registry.buttons.entities.back());
	}
	while (registry.helpScreen.entities.size() > 0) {
		registry.remove_all_components_of(registry.helpScreen.entities.back());
	}
	menu.active = false;
	// reset type to default PAUSE menu
	// menuType only = help if @ start of games
	menu.menuType = 0;
	// ensure that menu is no longer paused once menu is gone
	menu.paused = false;
}

float MenuSystem::cameraPositionX(){
	Camera& camera = registry.camera.components[0];
	float camerapos =  ((-camera.x * window_width_px/2.0f) + window_width_px / 2.0f);
	return camerapos;
}

void MenuSystem::helpScreens(TEXTURE_ASSET_ID helpScreensType) {
	createHelpScreens(renderer, helpScreensType);
}

Entity MenuSystem::createHelpScreens(RenderSystem* renderer, TEXTURE_ASSET_ID helpScreensType)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	if (helpScreensType == TEXTURE_ASSET_ID::BOARD_HELP) {
		motion.position.x = cameraPositionX();
		motion.position.y = window_height_px / 2;
	} else {
		motion.position = {window_width_px / 2, window_height_px / 2};
	}

	if (helpScreensType == TEXTURE_ASSET_ID::TANK_HELP_1 || helpScreensType == TEXTURE_ASSET_ID::TANK_HELP_2) {
		motion.scale.x = window_width_px / 2;
		motion.scale.y = window_height_px;
	}
	else {
		motion.scale.x = window_width_px;
		motion.scale.y = window_height_px;
	}

	registry.helpScreen.emplace(entity);
	
	registry.renderRequests.insert(
		entity,
		{ helpScreensType,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}
