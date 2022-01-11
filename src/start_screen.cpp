#include "start_screen.hpp"
#include <vector>;

void StartSystem::init(RenderSystem* renderer, int window_width_px, int window_height_px) {
	this->window_width_px = window_width_px;
	this->window_height_px = window_height_px;
	this->renderer = renderer;
	// change path to the save file depending on OS (only windows follows diff convention)
#if _WIN32
	// different path to src
	this->pathForSaveFile = "save.txt";
#else
	this->pathForSaveFile = "../src/save.txt";
#endif
	registry.startScreen.emplace(board_controller);
	buildScreen();
}

void StartSystem::buildScreen() {
	float window_fourth = window_height_px / 4;

	background = createBackground(renderer, vec2(window_width_px / 2, window_height_px / 2));
	logo = createLogo(renderer, vec2(window_width_px / 2, window_fourth));
	new_button = createButton(renderer, vec2(window_width_px / 2, 2 * window_fourth), TEXTURE_ASSET_ID::NEW_GAME_BUTTON);
	load_button = createButton(renderer, vec2(window_width_px / 2, window_fourth + 2 * window_fourth), TEXTURE_ASSET_ID::LOAD_GAME_BUTTON);
}

void StartSystem::deconstructScreen() {
	while (registry.buttons.entities.size() > 0) {
		registry.remove_all_components_of(registry.buttons.entities.back());
	}
	while (registry.startScreenBackground.entities.size() > 0) {
		registry.remove_all_components_of(registry.startScreenBackground.entities.back());
	}
}

void StartSystem::buildLevelSelectScreen() {
	while (registry.buttons.entities.size() > 0) {
		registry.remove_all_components_of(registry.buttons.entities.back());
	}
	float window_fourth = window_height_px / 4;
	level1_button = createButton(renderer, vec2((window_width_px / 2.0f) - ( window_width_px * 0.125f), window_fourth + 2 * window_fourth), TEXTURE_ASSET_ID::LEVEL_ONE_BUTTON);
	level2_button = createButton(renderer, vec2((window_width_px / 2.0f) + ( window_width_px * 0.125f), window_fourth + 2 * window_fourth), TEXTURE_ASSET_ID::LEVEL_TWO_BUTTON);
	float scale1y = registry.motions.get(level1_button).scale.y / 2.0f;
	float scale2y = registry.motions.get(level2_button).scale.y / 2.0f;
	// add text
	makeLabel(renderer,  vec2((window_width_px / 2.0f) - ( window_width_px * 0.125f), window_fourth + 2 * window_fourth - scale1y), TEXTURE_ASSET_ID::BEACH_TEXT);
	makeLabel(renderer, vec2((window_width_px / 2.0f) + ( window_width_px * 0.125f), window_fourth + 2 * window_fourth - scale2y), TEXTURE_ASSET_ID::JUNGLE_TEXT);



	StartScreen& startScreen = registry.startScreen.get(board_controller);
	startScreen.level_select = true; 
}

Entity StartSystem::makeLabel(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID texture){
	// scale label based on window height or width (larger of the two is used to scale)
	float labelScale = window_width_px > window_height_px ? window_width_px / 8.f : window_height_px / 5.f;
	labelScale *=  0.25f;
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = vec2(position.x, position.y -+ (mesh.original_size.x * labelScale * 0.5f)  );

	motion.scale = vec2(labelScale * 2.0f, labelScale);
	

	registry.startScreenBackground.emplace(entity);
	// Set sprite attributes to character

	registry.renderRequests.insert(
		entity,
		{ texture,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity StartSystem::createButton(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID buttonType)
{	
	// scale buttons based on window height or width (the smaller of the 2 is used to scale)
	float buttonScale = window_width_px > window_height_px ? window_height_px / 7.f : window_width_px / 6.f;
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;

	motion.scale = mesh.original_size * buttonScale;
	motion.scale.x *= 2.f;

	Buttons& button = registry.buttons.emplace(entity);
	// Set sprite attributes to character
	switch (buttonType)
	{
	case TEXTURE_ASSET_ID::HELP_BUTTON:
		button.button_type = 0;
		break;
	case TEXTURE_ASSET_ID::SAVE_BUTTON:
		button.button_type = 1;
		break;
	case TEXTURE_ASSET_ID::EXIT_BUTTON:
		button.button_type = 2;
		break;
	case TEXTURE_ASSET_ID::NEW_GAME_BUTTON:
		button.button_type = 3;
		break;
	case TEXTURE_ASSET_ID::LOAD_GAME_BUTTON:
		button.button_type = 4;
		break;
	case TEXTURE_ASSET_ID::LEVEL_ONE_BUTTON:
		button.button_type = 5;
		break;
	case TEXTURE_ASSET_ID::LEVEL_TWO_BUTTON:
		button.button_type = 6;
		break;
	default:
		break;
	}
	registry.renderRequests.insert(
		entity,
		{ buttonType,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity StartSystem::createBackground(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;

	motion.scale.x = mesh.original_size.x * window_width_px;
	motion.scale.y = mesh.original_size.y * window_height_px;

	registry.startScreenBackground.emplace(entity);
	// Set sprite attributes to character

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::START_SCREEN_BACKGROUND,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity StartSystem::createLogo(RenderSystem* renderer, vec2 position)
{
	// scale logo based on window height or width (larger of the two is used to scale)
	float logoScale = window_width_px > window_height_px ? window_width_px / 8.f : window_height_px / 5.f;

	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.position = position;

	motion.scale = mesh.original_size * logoScale;
	motion.scale.x *= 2.f;

	registry.startScreenBackground.emplace(entity);
	// Set sprite attributes to character

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LOGO,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

void StartSystem::serialize(const GameBoard& g)
{
	// implement points later, fix file path for os TODO
	std::ostringstream oss;

	if (!(oss << g))
	{
		throw std::runtime_error("Failed to serialize image");
	}

	std::ofstream saveFile;
	saveFile.open(pathForSaveFile, std::ios::out);
	if (!saveFile) {
		printf("File not created!");
	}
	else {
		printf("File created successfully!");
		saveFile << oss.str() << std::endl;
		saveFile.flush();
		saveFile.close();

		// below code is for testing purposes, will print save to console
		std::string line;
		std::ifstream myfile(pathForSaveFile);
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				std::cout << line << '\n';
			}
			myfile.close();
		}
	}
}

bool StartSystem::loadSave()
{
	GameBoard g = GameBoard();
	std::string line;
	std::ifstream myfile(pathForSaveFile);
	std::string delimiter = ", ";
	size_t pos = 0;
	std::string token;

	if (myfile.is_open())
	{
		int i = 0;
		while (getline(myfile, line))
		{
			switch (i) {
			case 0: {
				// read in basic gameboard values
				while ((pos = line.find(delimiter)) != std::string::npos) {
					token = line.substr(0, pos);
					// size: 14
					// std::cout << token << std::endl;
					readBasicGameBoard(g, token, ": ", 0);
					line.erase(0, pos + delimiter.length());
				}
				break;
			}
			case 1: {
				// read in backgrounds
				delimiter = ": ";
				pos = line.find(delimiter);
				// ignore "backgrounds: "
				line.erase(0, pos + delimiter.length());
				// separate line by background
				delimiter = "{";
				while ((pos = line.find(delimiter)) != std::string::npos) {
					token = line.substr(0, pos);
					// helper function for reading in background
					// background components are separated by "; "
					readGameBoardBackground(g, token, "; ");
					line.erase(0, pos + delimiter.length());
				}
				break;
			}
			case 2: {
				// read in gametiles
				delimiter = ": ";
				pos = line.find(delimiter);
				token = line.substr(pos + delimiter.size(), -1);
				delimiter = ", ";
				std::string token_2;
				int j = 0;
				int enum_value;
				while ((pos = token.find(delimiter)) != std::string::npos) {
					token_2 = token.substr(0, pos);
					// helper function for reading in game tiles
					enum_value = std::stoi(token_2);
					GAMETILE_TYPE val = static_cast<GAMETILE_TYPE>(enum_value);
					g.config.push_back({enum_value}); //{ val };
					token.erase(0, pos + delimiter.length());
					j++;
				}
				break;
			}
			case 3: {
				// read in active_item
				// remove "active_item: " from line
				delimiter = ": ";
				pos = line.find(delimiter);
				line = line.substr(pos + delimiter.size(), -1);

				delimiter = ", ";
				int j = 0;
				// insert texture asset id into active_item
				while ((pos = line.find(delimiter)) != std::string::npos) {
					token = line.substr(0, pos);
					g.active_item[j] = static_cast<TEXTURE_ASSET_ID>(std::stoi(token));
					line.erase(0, pos + delimiter.length());
					j++;
				}

				break;
			}
			case 4: {
				// read in is_poisoned
				// remove "is_poisoned: " from line
				delimiter = ": ";
				pos = line.find(delimiter);
				line = line.substr(pos + delimiter.size(), -1);

				delimiter = ", ";
				int j = 0;
				// insert bool val into is_poisoned
				while ((pos = line.find(delimiter)) != std::string::npos) {
					token = line.substr(0, pos);
					if (std::stoi(token) == 1) {
						g.is_poisoned[j] = true;
					}
					else {
						g.is_poisoned[j] = false;
					}
					line.erase(0, pos + delimiter.length());
					j++;
				}

				break;
			}
			case 5: {
				// read in player_points
				// remove "player_points: " from line
				delimiter = ": ";
				pos = line.find(delimiter);
				line = line.substr(pos + delimiter.size(), -1);

				delimiter = ", ";
				int j = 0;
				// insert point into player_points
				while ((pos = line.find(delimiter)) != std::string::npos) {
					token = line.substr(0, pos);
					g.player_points[j] = std::stoul(token);
					line.erase(0, pos + delimiter.length());
					j++;
				}

				break;
			}
			case 6: {
				// read in player_inventory
				// remove "player_inventory: " from line
				delimiter = ": ";
				pos = line.find(delimiter);
				line = line.substr(pos + delimiter.size(), -1);

				// delimiter to separate list of player inventories into single inventory
				delimiter = "{";
				// these are to parse the token into smaller tokens, which will be a single item
				std::string token_2;
				std::string delimiter_2 = ", ";
				size_t pos_2 = 0;
				int j = 0;
				while ((pos = line.find(delimiter)) != std::string::npos) {
					// token = a single player's inventory (vector)
					token = line.substr(0, pos);
					std::vector<TEXTURE_ASSET_ID> inventory;
					while ((pos_2 = token.find(delimiter_2)) != std::string::npos) {
						// token_2 = a single item
						token_2 = token.substr(0, pos_2);
						// add item to inventory
						inventory.push_back(static_cast<TEXTURE_ASSET_ID>(std::stoi(token_2)));
						token.erase(0, pos_2 + delimiter_2.length());
					}
					// add inventory to array of player inventories
					g.playerInv[j] = inventory;
					line.erase(0, pos + delimiter.length());
					j++;
				}

				break;
			}
			case 7: {
				// read in player_positions
				delimiter = "{";
				pos = line.find(delimiter);
				token = line.substr(pos + delimiter.size(), -1);
				delimiter = ";";
				std::string token_2;
				std::string token_3;
				int player_num = 0;
				while ((pos = token.find(delimiter)) != std::string::npos) {
					token_2 = token.substr(0, pos);
					delimiter = ": ";
					pos = token_2.find(delimiter);
					token_2 = token_2.substr(pos + delimiter.size(), -1);
					delimiter = ", ";
					while ((pos = token_2.find(delimiter)) != std::string::npos) {
						token_3 = token_2.substr(0, pos);
						readBasicGameBoard(g, token_3, ": ", player_num);
						token_2.erase(0, pos + delimiter.length());
					}
					delimiter = ";";
					// helper function for reading in

					pos = token.find(delimiter);
					token.erase(0, pos + delimiter.length());
					player_num++;
				}
				break;
			}
			default:
				break;
			}
			i++;
		}
		myfile.close();
	}
	else if (!myfile.is_open() || myfile.peek() == std::ifstream::traits_type::eof()) {
		// file couldn't open or file is empty, so prevent game from starting b/c load was unsuccessful
		return false;
	}
	GameBoard& new_g = registry.gameBoard.get(board_controller);
	new_g = g;
	// if it reaches here, file was successfully read
	return true;
}

void StartSystem::readBasicGameBoard(GameBoard& g, std::string token, std::string delimiter, int player) {
	size_t pos = 0;
	std::string component_type;
	std::string component;
	pos = token.find(delimiter);

	component_type = token.substr(0, pos);
	component = token.substr(pos + delimiter.length(), -1);

	if (component_type == "size") {
		g.size = std::stoi(component);
	}
	else if (component_type == "player") {
		// g.players = std::stoi(component);
	}
	else if (component_type == "turn") {
		g.turn = std::stoi(component);
	}
	else if (component_type == "roll") {
		g.roll = std::stoi(component);
	}
	else if (component_type == "xAxisScroll") {
		g.xAxisScroll = std::stof(component);
	}
	else if (component_type == "active") {
		g.active = true;
	}
	else if (component_type == "loaded") {
		g.loaded = false;
	}
	else if (component_type == "introSeen") {
		if (std::stoi(component) == 1) {
			g.introSeen = true;
		}
		else {
			g.introSeen = false;
		}
	}
	else if (component_type == "tutorialSeen") {
		if (std::stoi(component) == 1) {
			g.tutorialSeen = true;
		}
		else {
			g.tutorialSeen = false;
		}
	}
	else if (component_type == "stage") {
		g.stage = std::stoi(component);
	}
	else if (component_type == "level") {
		g.level = component;
	}
	else if (component_type == "active_background") {
		g.active_background = std::stoi(component);
	}
	else if (component_type == "start_position_x") {
		g.start_position.x = std::stof(component);
	}
	else if (component_type == "start_position_y") {
		g.start_position.y = std::stof(component);
	}
	else if (component_type == "position_x") {
		std::get<1>(g.player_positions[player]).x = std::stof(component);
	}
	else if (component_type == "position_y") {
		std::get<1>(g.player_positions[player]).y = std::stof(component);
	}
	else if (component_type == "current_tile") {
		std::get<2>(g.player_positions[player]) = std::stoi(component);
	}
}

void StartSystem::readGameBoardBackground(GameBoard& g, std::string token, std::string delimiter) {
	size_t pos = 0;
	std::string component;

	// create a background
	SceneBackGround bg = SceneBackGround();
	std::vector<SceneBackGroundComponent> bgElements{};

	while ((pos = token.find(delimiter)) != std::string::npos) {
		// separate background into component tokens
		component = token.substr(0, pos);
		// process component and add it to bgElements
		readBackGroundComponent(bgElements, component, ", ");
		// remove processed component from token
		token.erase(0, pos + delimiter.length());
	}
	// add elements to bg
	bg.elements = bgElements;
	// add background to gameboard
	g.backgrounds.push_back(bg);
}
void StartSystem::readBackGroundComponent(std::vector<SceneBackGroundComponent>& vector, std::string component, std::string delimiter) {
	size_t pos = 0;
	// 2nd pos to track inside of attribute
	size_t pos_2 = 0;
	std::string component_attribute;
	std::string attribute_value;
	std::string attribute;
	std::string temp_delimiter;
	// remove leading "component: "
	temp_delimiter = ": ";
	pos = component.find(temp_delimiter);
	component.erase(0, pos + temp_delimiter.length());

	// create component
	SceneBackGroundComponent sbc = SceneBackGroundComponent();
	while ((pos = component.find(delimiter)) != std::string::npos) {
		// separate component into attributes
		attribute = component.substr(0, pos);

		// process attribute (divide into attribute + value)
		pos_2 = attribute.find(temp_delimiter);
		component_attribute = attribute.substr(0, pos_2);
		attribute_value = attribute.substr(pos_2 + temp_delimiter.length(), attribute.length());

		// add attribute to component
		if (component_attribute == "offset") {
			sbc.offset = std::stof(attribute_value);
		}
		else if (component_attribute == "startpos_x") {
			sbc.startpos.x = std::stof(attribute_value);
		}
		else if (component_attribute == "startpos_y") {
			sbc.startpos.y = std::stof(attribute_value);
		}
		else if (component_attribute == "scale_x") {
			sbc.scale.x = std::stof(attribute_value);
		}
		else if (component_attribute == "scale_y") {
			sbc.scale.y = std::stof(attribute_value);
		}
		else if (component_attribute == "texture_asset") {
			sbc.image = static_cast<TEXTURE_ASSET_ID>(std::stoi(attribute_value));
		}
		// remove processed attribute from component
		component.erase(0, pos + delimiter.length());
	}
	vector.push_back(sbc);
}

std::ostream& operator<<(std::ostream& os, const GameBoard& g)
{
	Entity player;
	os << "size: " << g.size << ", player: " << g.players << ", turn: " << g.turn << ", roll: " << g.roll << ", xAxisScroll: "
		<< g.xAxisScroll << ", active: " << g.active << ", loaded: " << g.loaded << ", introSeen: " << g.introSeen << ", tutorialSeen: " << g.tutorialSeen << ", stage: " << g.stage << ", level: " << g.level << ", active_background: " <<
		g.active_background << ", start_position_x: " << g.start_position.x << ", start_position_y: " << g.start_position.y << ",\nbackgrounds: "
		<< g.backgrounds << "\nGameTiles: ";
	
	// for (GameTile t : g.config) {
	std::vector<GameTile>::const_iterator it;
	for (it = g.config.begin(); it != g.config.end();) {
		os << (int)(*it).type << ", ";
		it++;
	}
	os << "\nactive_item: ";
	for (auto const& item : g.active_item) {
		os << (int)item << ", ";
	}

	os << "\nis_poisoned: ";
	for (bool poisoned : g.is_poisoned) {
		os << (int)poisoned << ", ";
	}
	os << "\nplayer_points: ";
	for (unsigned int point : g.player_points) {
		os << (int)point << ", ";
	}
	os << "\nplayer_inventory: ";
	for (std::vector<TEXTURE_ASSET_ID> inventory  : g.playerInv) {
		for (auto const& item : inventory)
		{
			os << (int) item << ", ";
		}
		os << "{";
	}
	os << "\nplayer_positions: {";
	for (int i = 0; i < g.players; i++) {
		os << "player_position: ";
		player = std::get<0>(g.player_positions[i]);
		os << "entity_id: " << player.operator unsigned int();
		os << ", position_x: " << std::get<1>(g.player_positions[i]).x;
		os << ", position_y: " << std::get<1>(g.player_positions[i]).y;
		os << ", current_tile: " << std::get<2>(g.player_positions[i]);
		os << ", ;";
	}
	os << "}";
	return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<SceneBackGround>& backgrounds)
{

	for (auto const& bg : backgrounds)
	{
		for (const SceneBackGroundComponent& sbc : bg.elements) {
			os << "component: ";
			os << sbc;
		}
		os << "{";
	}

	return os;
}

std::ostream& operator<<(std::ostream& os, const SceneBackGroundComponent& sbc)
{

	os << "offset: " << sbc.offset;
	os << ", startpos_x: " << sbc.startpos.x;
	os << ", startpos_y: " << sbc.startpos.y;
	os << ", scale_x: " << sbc.scale.x;
	os << ", scale_y: " << sbc.scale.y;
	os << ", texture_asset: " << (int)sbc.image << ", " << "; ";

	return os;
}