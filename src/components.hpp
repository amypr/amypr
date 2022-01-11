#pragma once

#include "common.hpp"
#include <vector>
#include <map>
#include <unordered_map>
#include <stack>
#include <deque>
#include <queue>
#include <unordered_set>
#include "../ext/stb_image/stb_image.h"

// Player component
struct PlayerTank
{
    float health = 500;
    float fire_interval = 0;
    float id;
};
// todo project
struct Sprite {
	int num_frames = 5;
	// vector of frame num + direction faced (0 being right, 1 being left)
	vec2 frame = { 0, 0 };
	float animation_speed = 50.f;
	float time_last_update = 0;
};

struct MouseHold
{
	bool isMouseHolded = 0;
	std::vector<vec2> points;
};

struct DrawingCharacterWriting
{

};

struct NotEnoughMoneyBanner
{

};

struct Board
{
    float x_pos_offset;
};

struct ScoreNumber
{
    float x_pos_offset;
};


struct Platformer {
	// whether platformer should be active
	bool active = false;
	// whether platformer is already active
	bool loaded = false;
};

struct Platform {

};

// trap in platformer (will stun character if they collide with it)
struct Trap
{

};

// struct to indicate whether an entity will be affected by platformer physics
// only player or AI atm
struct PlatformerPhysics {
	bool onPlatform = false;
};

struct ShopBackground {

};

struct TankGame {
    // whether tank should be active
    bool active = false;
    // whether tank is already active
    bool loaded = false;
    int level;
};

struct AITank
{
    float last_check = 0;
    float health = 300;
};

struct Tanking
{
    float last_check = 0;
    float health = 2000;
};

struct Wall
{
    float health = 150;
};

struct HardWall {
	float health = 3000;
};

struct Physics {
	float mass;
    float density;
};

struct TankHome {
	float health = 200;
};


struct AIBullet
{
    float damage = 50;
};

struct PlayerBullet
{
    float damage = 100;
    unsigned int shot_by = 0;
};

struct TankGamePoint
{
    float points = 0;
};

struct Ai_Info {
    float time_counter = 400.f;
    bool in_progress = false;
};

struct Goal {

};

struct Light {
	float angle = 0;
	vec2 lower_left = {-3.015, 7.4};
	vec2 upper_left = {-1.205, 5.11};
	vec2 lower_right = {3.015, 7.4};
	vec2 upper_right = {1.205, 5.11};
	vec2 light_pos = {0, 7.4};
	vec2 original_size = {6.91, 17.32};
};

struct TooManyItems {
	bool too_many = false;
	bool active = false;
};

// 0 for help, 1 for save, 2 for exit, 3 for new game, 4 for load game
struct Buttons {
	int button_type = 0;
};

// MouseDraw
struct MouseDraw
{
	
};

struct Poisoned
{
	
};

struct Coins
{
	float rotationY = 0.0f;
	float rotationX = 0.0f;
};

struct Temple
{
	float rotationY = 0.0f;
	float rotationX = 0.0f;
};


struct Menu
{
	bool paused = 0;
	bool active = 0;
	// 0 = pause, 1 = help
	int menuType = 0;
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
	std::queue<vec2> path = {};
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other)
	{
		this->other = other;
	};
	
};

struct LightUp
{
    float counter_ms = 3000;
};

// Data structure for toggling debug mode
struct Debug {
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
};
extern Debug debugging;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// A timer that will be associated to dying salmon
struct DeathTimer
{
	float counter_ms = 3000;
};

// A timer that will be associated to stunned characters
struct StunTimer
{
	float counter_ms = 2000;
};

// A timer that will be associated to drawing game
struct DrawingTimer
{
	float counter_ms = 30000;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & salmon.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
	vec3 normal;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Single Vertex buffer element for character sprites (character.vs.glsl)
struct CharacterVertex 
{
	vec3 position;
	vec2 texcoord;
};

// Single Vertex buffer element for dialogue boxes (dialogue.vs.glsl)
struct DialogueVertex
{
	vec3 position;
	vec2 texcoord;
};

struct BackgroundColour
{
	float red;
	float green;
	float blue;
	float alpha = 1.f;
};

struct StartScreenBackground
{

};

struct HelpScreen
{

};

// struct to indicate an entity is a dialogue box
struct Dialogue
{	
	// 0 = introduction dialogue, 1 = winning/ending dialogue
	int dialogueType = 0;
	// variable here in case of branching dialogue/next frame is NOT the following number from current frame
	int nextDialogueFrame = 1;
};

struct Camera
{
	float zoom = 1;
	float x = 0;
	float y = 0;
	int camera_fb_width;
	float screen_scale;
	int camera_width;
	vec2 pos;
	int target;
};

struct NumberCharacter {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

struct NumberCharacters {
	std::map<char, NumberCharacter> numberCharacters;
};

struct Particle {
	vec2 position;
	vec2 lifespan_time; // (time_remaining,total time) ,particle "dies" after reaching 0
	vec2 velocity;
};

struct Particle_Manager{

	// std::vector<vec2> positions;
	// std::vector<float> lifespan_times; // particle "dies" after reaching 0
	// std::vector<vec2> velocities;
	std::vector<Particle> particles;
	// Particle_Manager(int x) : particles(x){}
};

// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size, std::vector<uint16_t>& out_uv_indices, std::vector<glm::vec3>& out_normals, std::vector<uint16_t>& out_normal_indices);
	vec2 original_size = {1,1};
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
	std::vector<uint16_t> uvs;
	std::vector<glm::vec3> normals;
	std::vector<uint16_t> normal_indices;
};

/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

// ONLY ADD ASSETS TO THE BOTTOM OF THE LIST
enum class TEXTURE_ASSET_ID { // should be in line with texture_paths in rendersystem header
	WALL = 0,
	AITank = WALL + 1,
	CHARACTER_ONE = AITank + 1,
	CHARACTER_TWO = CHARACTER_ONE + 1,
	CHARACTER_THREE = CHARACTER_TWO + 1,
	CHARACTER_FOUR = CHARACTER_THREE + 1,
	NEUTRAL_TILE = CHARACTER_FOUR + 1,
	MOVEMENTGAIN_TILE = NEUTRAL_TILE + 1,
	MOVEMENTLOSS_TILE = MOVEMENTGAIN_TILE + 1,
	RANDOM_TILE = MOVEMENTLOSS_TILE + 1,
	SHOP_TILE = RANDOM_TILE + 1,
	TREASUREGAIN_TILE = SHOP_TILE + 1,
	TREASURELOSS_TILE = TREASUREGAIN_TILE + 1,
	BEACH_BACKGROUND_FULL = TREASURELOSS_TILE + 1,
	JUNGLE_BACKGROUND_FULL = BEACH_BACKGROUND_FULL + 1,
	PLATFORM = JUNGLE_BACKGROUND_FULL + 1,
	GOAL = PLATFORM + 1,
	DRAWING_BACKGROUND = GOAL + 1,
	BEACH_BACKGROUND_0 = DRAWING_BACKGROUND + 1,
	BEACH_BACKGROUND_1 = BEACH_BACKGROUND_0 + 1,
	BEACH_BACKGROUND_2 = BEACH_BACKGROUND_1 + 1,
	BEACH_BACKGROUND_3 = BEACH_BACKGROUND_2 + 1,
	BEACH_BACKGROUND_4 = BEACH_BACKGROUND_3 + 1,
	NUMBER_0 = BEACH_BACKGROUND_4 + 1,
	NUMBER_1 = NUMBER_0 + 1,
	NUMBER_2 = NUMBER_1 + 1,
	NUMBER_3 = NUMBER_2 + 1,
	NUMBER_4 = NUMBER_3 + 1,
	NUMBER_5 = NUMBER_4 + 1,
	NUMBER_6 = NUMBER_5 + 1,
	NUMBER_7 = NUMBER_6 + 1,
	NUMBER_8 = NUMBER_7 + 1,
	NUMBER_9 = NUMBER_8 + 1,
	SCOREBOARD_1 = NUMBER_9 + 1,
	SCOREBOARD_2 = SCOREBOARD_1 + 1,
	SCOREBOARD_3 = SCOREBOARD_2 + 1,
	SCOREBOARD_4 = SCOREBOARD_3 + 1,
	EXIT_BUTTON = SCOREBOARD_4 + 1,
	HELP_BUTTON = EXIT_BUTTON + 1,
	SAVE_BUTTON = HELP_BUTTON + 1,
	NEW_GAME_BUTTON = SAVE_BUTTON + 1,
	LOAD_GAME_BUTTON = NEW_GAME_BUTTON + 1,
	LOGO = LOAD_GAME_BUTTON + 1,
	START_SCREEN_BACKGROUND = LOGO + 1,
	PLAYER_1_WRITING = START_SCREEN_BACKGROUND + 1,
	PLAYER_2_WRITING = PLAYER_1_WRITING + 1,
	DRAWING_HEART = PLAYER_2_WRITING + 1,
	DRAWING_SQUARE = DRAWING_HEART + 1,
	DRAWING_TRIANGLE = DRAWING_SQUARE + 1,
	PLATFORMER_HELP = DRAWING_TRIANGLE + 1,
	DRAWING_HELP = PLATFORMER_HELP + 1,
	BOARD_HELP = DRAWING_HELP + 1,
	TANK_HELP_1 = BOARD_HELP + 1,
	TANK_HELP_2 = TANK_HELP_1 + 1,
	DICE_ITEM = TANK_HELP_2 + 1,
	HEART_ITEM = DICE_ITEM + 1,
	LIGHTENING_ITEM = HEART_ITEM + 1,
	POISON_ITEM = LIGHTENING_ITEM + 1,
	TRANSPORT_ITEM = POISON_ITEM + 1,
	TREASURE_1_ITEM = TRANSPORT_ITEM + 1,
	TREASURE_2_ITEM = TREASURE_1_ITEM + 1,
	HEAVY_DAMAGE_ITEM = TREASURE_2_ITEM + 1,
	ENHANCED_CANNON_ITEM = HEAVY_DAMAGE_ITEM + 1,
	SHOP_BACKGROUND = ENHANCED_CANNON_ITEM + 1,
	INV_DICE = SHOP_BACKGROUND + 1,
	INV_HEART = INV_DICE + 1,
	INV_LIGHTENING = INV_HEART + 1,
	INV_POISON = INV_LIGHTENING + 1,
	INV_TRANSPORT = INV_POISON + 1,
	INV_TREASURE_1 = INV_TRANSPORT + 1,
	INV_TREASURE_2 = INV_TREASURE_1 + 1,
	INV_HEAVY_DAMAGE = INV_TREASURE_2 + 1,
	INV_ENHANCED_CANNON = INV_HEAVY_DAMAGE + 1,
	NOT_ENOUGH_TREASURE = INV_ENHANCED_CANNON + 1,
	INTRO_SPRITESHEET = NOT_ENOUGH_TREASURE + 1,
	ENDING_SPRITESHEET = INTRO_SPRITESHEET + 1,
	TOO_MANY_ITEMS = ENDING_SPRITESHEET + 1,
	TOADY_SPRITESHEET = TOO_MANY_ITEMS + 1,
	TRAP = TOADY_SPRITESHEET + 1,
	JUNGLE_BACKGROUND_0 = TRAP + 1,
	JUNGLE_BACKGROUND_1 = JUNGLE_BACKGROUND_0 + 1,
	JUNGLE_BACKGROUND_2 = JUNGLE_BACKGROUND_1 + 1,
	JUNGLE_BACKGROUND_3 = JUNGLE_BACKGROUND_2 + 1,
	JUNGLE_BACKGROUND_4 = JUNGLE_BACKGROUND_3 + 1,
	JUNGLE_BACKGROUND_5 = JUNGLE_BACKGROUND_4 + 1,
	LEVEL_ONE_BUTTON = JUNGLE_BACKGROUND_5 + 1,
	LEVEL_TWO_BUTTON = LEVEL_ONE_BUTTON + 1,
	BEACH_TEXT = LEVEL_TWO_BUTTON + 1,
	JUNGLE_TEXT = BEACH_TEXT + 1,
	NewTank = JUNGLE_TEXT + 1,
	NewTank2 = NewTank + 1,
	TANK_HOME = NewTank2 + 1,
	TANK_WALL2 = TANK_HOME + 1,
	TANK_BROKEN_WALL = TANK_WALL2 + 1,
	TUTORIAL_SPRITESHEET = TANK_BROKEN_WALL + 1,
	LIGHTENING = TUTORIAL_SPRITESHEET + 1,
	DRAWING_PENTAGON = LIGHTENING + 1,
	DRAWING_STAR = DRAWING_PENTAGON + 1,
	HP_BUFF_ITEM = DRAWING_STAR + 1,
	INV_HP_BUFF = HP_BUFF_ITEM + 1,
    LAMP = INV_HP_BUFF + 1,
	TEXTURE_COUNT = LAMP + 1
};

const TEXTURE_ASSET_ID playerAssets[4] = {TEXTURE_ASSET_ID::CHARACTER_ONE,TEXTURE_ASSET_ID::CHARACTER_TWO,TEXTURE_ASSET_ID::CHARACTER_THREE,TEXTURE_ASSET_ID::CHARACTER_FOUR};

const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	CHARACTER = 0,
	COLOURED = CHARACTER + 1,
	DIALOGUE = COLOURED + 1,
	DICE = DIALOGUE + 1,
	GOLD = DICE + 1,
	MENU_BACKGROUND = GOLD + 1,
	PEBBLE = MENU_BACKGROUND + 1,
	SALMON = PEBBLE + 1,
	TEXT = SALMON + 1,
	TEXTURED = TEXT + 1,
	WATER = TEXTURED + 1,
	PARTICLE = WATER + 1,
	TUTORIAL = PARTICLE + 1,
	EFFECT_COUNT = TUTORIAL + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

// set effects with gs shaders here
const std::unordered_set<int> gs_shader_support {(int)EFFECT_ASSET_ID::DICE};


enum class GEOMETRY_BUFFER_ID {
	SALMON = 0,
	SPRITE = SALMON + 1,
	PEBBLE = SPRITE + 1,
	DEBUG_LINE = PEBBLE + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	BOARD_DIE = SCREEN_TRIANGLE + 1,
	CHARACTER = BOARD_DIE + 1,
	DIALOGUE = CHARACTER + 1,
	TEMPLE = DIALOGUE + 1,
	COIN = TEMPLE + 1,
	SAND = COIN + 1,
	LEAF = SAND + 1,
	TUTORIAL = LEAF + 1,
	GEOMETRY_COUNT = TUTORIAL + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

enum class GAMETILE_TYPE {
	NEUTRAL = 0,
	GAIN_TREASURE = 1,
	LOSE_TREASURE = 2,
	GAIN_MOVEMENT = 3,
	LOSE_MOVEMENT = 4,
	SHOP = 5,
	RANDOM_EVENT = 6
};

// enum class ITEM_TYPE {
// 	DICE = 0,
// 	HEART = 1,
// 	LIGHTENING = 2,
// 	POISON = 3,
// 	TRANSPORT = 4,
// 	TREASURE_1 = 5,
// 	TREASURE_2 = 6
// };

struct GameTile {
	GameTile(){

	}
	GameTile(int i){
		type = static_cast<GAMETILE_TYPE>(i);
	}
	GAMETILE_TYPE type;
};

struct SceneBackGroundComponent {
	float offset; // how quickly image moves with camera, offset in [0,1] where 0 is static and 1 is moving with the camera
	vec2 startpos;
	vec2 scale;
	TEXTURE_ASSET_ID image; // texture asset id
};


struct SceneBackGround {
	std::vector<SceneBackGroundComponent> elements;
};

struct GameDie {
	int value = -1;
	bool rolling = false;
	bool controlled = false;
	float angular_velocity = 0;
	vec2 velocity = vec2(0);
	bool boom = false;
	float timer_ms = 3000.0f;
	float start_time = 0.0f;
	float end_time = -1.0f;
	bool boomed = false;
};


enum class BOARD_AI_STATE {
	TURN_END = 0,
	TURN_START = 1,
	SELECTING_ITEM = 2,
	ROLLING = 3,
	MOVING = 4,
	REACT_TO_TILE = 5,
	CHOOSE_PLAYER_SWAP = 6,
	CHOOSE_PLAYER_STEAL = 7,
	SELECT_NONE = 8,
	SELECT_PLAYER_1 = 9,
	SELECT_PLAYER_2 = 10,
	SELECT_PLAYER_3 = 11,
	SELECT_PLAYER_4 = 12,
	REACT_TO_RAND_EVENT = 13
};

// enum class BOARD_ITEM {
// 	DOUBLE_DICE = 0,
// 	SWAP_POSITION = 1,
// 	STEAL_POINT_5 = 2
// };

struct GameBoard {
	int size; // board size
	static const int players = 4; // how many players (can't change once game starts)
	bool aiFlag[players] = { false,false,true,true }; // used to show which players are AI or real
	BOARD_AI_STATE playerStates[players] = {BOARD_AI_STATE::TURN_END,BOARD_AI_STATE::TURN_END ,BOARD_AI_STATE::TURN_END ,BOARD_AI_STATE::TURN_END };
	std::vector<TEXTURE_ASSET_ID> playerInv[players];
	int turn = 0; // which player's turn it is
	int roll = -1; // dice roll outcome
	float xAxisScroll = 0.0f;
	bool active = false;
	bool loaded = false;
	// whether or not the inventory screen is active and loaded
    bool inv_active = false;
	bool inv_loaded = false;
	// whether or not players have gone through the introduction of game
	bool introSeen = false;
	bool tutorialSeen = false;
	int stage = -3;
	unsigned int player_points[players] = {0, 0, 0, 0};
	TEXTURE_ASSET_ID active_item[players] = {TEXTURE_ASSET_ID::BOARD_HELP, TEXTURE_ASSET_ID::BOARD_HELP, TEXTURE_ASSET_ID::BOARD_HELP, TEXTURE_ASSET_ID::BOARD_HELP}; // active item per player
	bool is_poisoned[players] = {false};
	std::vector<SceneBackGround> backgrounds;
	int active_background = 0;
	vec2 start_position; // board starting position
	std::vector<GameTile> config;
	std::tuple<Entity, vec2, int> player_positions[players]; // (player_entity_ID, position, current tile)
	std::string level = "beach_1.json"; // used to set the board level at the new game screen
};

struct DrawingGame {
	bool active = false;
	bool loaded = false;
	bool switched = false;
	std::vector<vec2> triangle_points = {{0.000000, 0.000000}, {-19.000000, 28.000000}, {-33.000000, 50.000000}, {-57.000000, 78.000000}, {-72.000000, 102.000000}, {-95.000000, 126.000000}, {-124.000000, 155.000000}, {-144.000000, 184.000000}, {-161.000000, 207.000000}, {-185.000000, 231.000000}, {-204.000000, 255.000000}, {-218.000000, 274.000000}, {-238.000000, 298.000000}, {-260.000000, 320.000000}, {-276.000000, 340.000000}, {-293.000000, 366.000000}, {-186.000000, 352.000000}, {-159.000000, 354.000000}, {-127.000000, 354.000000}, {-100.000000, 354.000000}, {-71.000000, 354.000000}, {-44.000000, 357.000000}, {-12.000000, 359.000000}, {17.000000, 359.000000}, {46.000000, 359.000000}, {78.000000, 359.000000}, {112.000000, 359.000000}, {147.000000, 361.000000}, {178.000000, 361.000000}, {207.000000, 361.000000}, {239.000000, 361.000000}, {270.000000, 361.000000}, {298.000000, 361.000000}, {325.000000, 366.000000}, {317.000000, 348.000000}, {297.000000, 324.000000}, {276.000000, 295.000000}, {254.000000, 269.000000}, {235.000000, 239.000000}, {212.000000, 215.000000}, {185.000000, 183.000000}, {163.000000, 153.000000}, {140.000000, 130.000000}, {123.000000, 111.000000}, {102.000000, 88.000000}, {85.000000, 68.000000}, {66.000000, 37.000000}, {46.000000, 16.000000}, {35.000000, 1.000000}, {24.000000, -13.000000}};
	std::vector<vec2> square_points = {{0.000000, 0.000000}, {0.000000, 36.000000}, {0.000000, 73.000000}, {0.000000, 115.000000}, {2.000000, 156.000000}, {2.000000, 195.000000}, {2.000000, 237.000000}, {2.000000, 277.000000}, {2.000000, 317.000000}, {10.000000, 342.000000}, {42.000000, 342.000000}, {88.000000, 354.000000}, {123.000000, 356.000000}, {162.000000, 356.000000}, {203.000000, 352.000000}, {243.000000, 350.000000}, {286.000000, 345.000000}, {329.000000, 340.000000}, {369.000000, 342.000000}, {411.000000, 342.000000}, {457.000000, 340.000000}, {499.000000, 338.000000}, {538.000000, 338.000000}, {578.000000, 338.000000}, {617.000000, 338.000000}, {634.000000, 315.000000}, {634.000000, 275.000000}, {634.000000, 234.000000}, {632.000000, 186.000000}, {632.000000, 150.000000}, {627.000000, 108.000000}, {627.000000, 67.000000}, {627.000000, 25.000000}, {624.000000, -16.000000}, {624.000000, -46.000000}, {608.000000, -52.000000}, {573.000000, -49.000000}, {533.000000, -52.000000}, {488.000000, -57.000000}, {380.000000, -55.000000}, {352.000000, -53.000000}, {314.000000, -53.000000}, {274.000000, -53.000000}, {235.000000, -53.000000}, {193.000000, -53.000000}, {152.000000, -53.000000}, {111.000000, -53.000000}, {69.000000, -53.000000}, {30.000000, -53.000000}, {2.000000, -53.000000}};
	std::vector<vec2> star_points = {{0.000000, 0.000000}, {-14.000000, 30.000000}, {-24.000000, 58.000000}, {-31.000000, 90.000000}, {-36.000000, 116.000000}, {-47.000000, 139.000000}, {-72.000000, 143.000000}, {-107.000000, 153.000000}, {-136.000000, 157.000000}, {-163.000000, 165.000000}, {-193.000000, 170.000000}, {-224.000000, 170.000000}, {-250.000000, 177.000000}, {-268.000000, 190.000000}, {-237.000000, 192.000000}, {-215.000000, 192.000000}, {-191.000000, 192.000000}, {-162.000000, 202.000000}, {-135.000000, 209.000000}, {-110.000000, 214.000000}, {-85.000000, 214.000000}, {-59.000000, 216.000000}, {-41.000000, 228.000000}, {-29.000000, 254.000000}, {-24.000000, 285.000000}, {-20.000000, 313.000000}, {-10.000000, 341.000000}, {-4.000000, 367.000000}, {10.000000, 377.000000}, {14.000000, 341.000000}, {24.000000, 302.000000}, {45.000000, 255.000000}, {56.000000, 221.000000}, {81.000000, 211.000000}, {111.000000, 204.000000}, {142.000000, 201.000000}, {178.000000, 196.000000}, {210.000000, 189.000000}, {248.000000, 187.000000}, {278.000000, 184.000000}, {260.000000, 175.000000}, {230.000000, 175.000000}, {198.000000, 164.000000}, {160.000000, 145.000000}, {129.000000, 143.000000}, {104.000000, 143.000000}, {76.000000, 136.000000}, {35.000000, 65.000000}, {23.000000, 32.000000}, {11.000000, -4.000000}};
	std::vector<vec2> heart_points = {{0.000000, 0.000000}, {-31.000000, -25.000000}, {-59.000000, -39.000000}, {-90.000000, -53.000000}, {-114.000000, -58.000000}, {-148.000000, -70.000000}, {-180.000000, -70.000000}, {-212.000000, -53.000000}, {-236.000000, -42.000000}, {-263.000000, -25.000000}, {-280.000000, 0.000000}, {-284.000000, 24.000000}, {-290.000000, 43.000000}, {-290.000000, 71.000000}, {-288.000000, 100.000000}, {-271.000000, 118.000000}, {-257.000000, 139.000000}, {-240.000000, 158.000000}, {-222.000000, 175.000000}, {-199.000000, 194.000000}, {-177.000000, 204.000000}, {-153.000000, 220.000000}, {-131.000000, 236.000000}, {-112.000000, 255.000000}, {-83.000000, 272.000000}, {-54.000000, 289.000000}, {-30.000000, 303.000000}, {-6.000000, 311.000000}, {15.000000, 318.000000}, {40.000000, 320.000000}, {70.000000, 310.000000}, {99.000000, 292.000000}, {132.000000, 260.000000}, {162.000000, 239.000000}, {192.000000, 226.000000}, {225.000000, 210.000000}, {257.000000, 184.000000}, {289.000000, 140.000000}, {318.000000, 111.000000}, {329.000000, 72.000000}, {331.000000, 42.000000}, {258.000000, -42.000000}, {223.000000, -52.000000}, {192.000000, -57.000000}, {164.000000, -57.000000}, {131.000000, -52.000000}, {106.000000, -30.000000}, {78.000000, -8.000000}, {54.000000, 4.000000}, {27.000000, 22.000000}};
	std::vector<vec2> pentagon_points = {{0.000000, 0.000000}, {-29.000000, 14.000000}, {-60.000000, 26.000000}, {-91.000000, 43.000000}, {-120.000000, 62.000000}, {-146.000000, 81.000000}, {-172.000000, 93.000000}, {-201.000000, 97.000000}, {-231.000000, 109.000000}, {-252.000000, 121.000000}, {-270.000000, 135.000000}, {-280.000000, 153.000000}, {-257.000000, 174.000000}, {-248.000000, 196.000000}, {-238.000000, 222.000000}, {-230.000000, 250.000000}, {-223.000000, 278.000000}, {-211.000000, 306.000000}, {-198.000000, 325.000000}, {-182.000000, 345.000000}, {-157.000000, 380.000000}, {-134.000000, 380.000000}, {-109.000000, 385.000000}, {-83.000000, 385.000000}, {-59.000000, 385.000000}, {-33.000000, 383.000000}, {-2.000000, 383.000000}, {25.000000, 381.000000}, {53.000000, 381.000000}, {83.000000, 378.000000}, {114.000000, 378.000000}, {140.000000, 376.000000}, {171.000000, 376.000000}, {201.000000, 376.000000}, {224.000000, 377.000000}, {254.000000, 334.000000}, {283.000000, 285.000000}, {300.000000, 251.000000}, {319.000000, 201.000000}, {340.000000, 154.000000}, {326.000000, 125.000000}, {290.000000, 116.000000}, {254.000000, 100.000000}, {215.000000, 77.000000}, {188.000000, 68.000000}, {154.000000, 49.000000}, {125.000000, 28.000000}, {94.000000, 17.000000}, {67.000000, 7.000000}, {48.000000, -3.000000}};
	
	// std::vector<vec2> triangle_points = {{1907.000000, 911.000000}, {1888.000000, 939.000000}, {1874.000000, 961.000000}, {1850.000000, 989.000000}, {1835.000000, 1013.000000}, {1812.000000, 1037.000000}, {1783.000000, 1066.000000}, {1763.000000, 1095.000000}, {1746.000000, 1118.000000}, {1722.000000, 1142.000000}, {1703.000000, 1166.000000}, {1689.000000, 1185.000000}, {1669.000000, 1209.000000}, {1647.000000, 1231.000000}, {1631.000000, 1251.000000}, {1614.000000, 1277.000000}, {1721.000000, 1263.000000}, {1748.000000, 1265.000000}, {1780.000000, 1265.000000}, {1807.000000, 1265.000000}, {1836.000000, 1265.000000}, {1863.000000, 1268.000000}, {1895.000000, 1270.000000}, {1924.000000, 1270.000000}, {1953.000000, 1270.000000}, {1985.000000, 1270.000000}, {2019.000000, 1270.000000}, {2054.000000, 1272.000000}, {2085.000000, 1272.000000}, {2114.000000, 1272.000000}, {2146.000000, 1272.000000}, {2177.000000, 1272.000000}, {2205.000000, 1272.000000}, {2232.000000, 1277.000000}, {2224.000000, 1259.000000}, {2204.000000, 1235.000000}, {2183.000000, 1206.000000}, {2161.000000, 1180.000000}, {2142.000000, 1150.000000}, {2119.000000, 1126.000000}, {2092.000000, 1094.000000}, {2070.000000, 1064.000000}, {2047.000000, 1041.000000}, {2030.000000, 1022.000000}, {2009.000000, 999.000000}, {1992.000000, 979.000000}, {1973.000000, 948.000000}, {1953.000000, 927.000000}, {1942.000000, 912.000000}, {1931.000000, 898.000000}};
	// std::vector<vec2> square_points = {{1614.000000, 932.000000}, {1614.000000, 968.000000}, {1614.000000, 1005.000000}, {1614.000000, 1047.000000}, {1616.000000, 1088.000000}, {1616.000000, 1127.000000}, {1616.000000, 1169.000000}, {1616.000000, 1209.000000}, {1616.000000, 1249.000000}, {1624.000000, 1274.000000}, {1656.000000, 1274.000000}, {1702.000000, 1286.000000}, {1737.000000, 1288.000000}, {1776.000000, 1288.000000}, {1817.000000, 1284.000000}, {1857.000000, 1282.000000}, {1900.000000, 1277.000000}, {1943.000000, 1272.000000}, {1983.000000, 1274.000000}, {2025.000000, 1274.000000}, {2071.000000, 1272.000000}, {2113.000000, 1270.000000}, {2152.000000, 1270.000000}, {2192.000000, 1270.000000}, {2231.000000, 1270.000000}, {2248.000000, 1247.000000}, {2248.000000, 1207.000000}, {2248.000000, 1166.000000}, {2246.000000, 1118.000000}, {2246.000000, 1082.000000}, {2241.000000, 1040.000000}, {2241.000000, 999.000000}, {2241.000000, 957.000000}, {2238.000000, 916.000000}, {2238.000000, 886.000000}, {2222.000000, 880.000000}, {2187.000000, 883.000000}, {2147.000000, 880.000000}, {2102.000000, 875.000000}, {1994.000000, 877.000000}, {1966.000000, 879.000000}, {1928.000000, 879.000000}, {1888.000000, 879.000000}, {1849.000000, 879.000000}, {1807.000000, 879.000000}, {1766.000000, 879.000000}, {1725.000000, 879.000000}, {1683.000000, 879.000000}, {1644.000000, 879.000000}, {1616.000000, 879.000000}};
	// std::vector<vec2> star_points = {{1912.000000, 906.000000}, {1898.000000, 936.000000}, {1888.000000, 964.000000}, {1881.000000, 996.000000}, {1876.000000, 1022.000000}, {1865.000000, 1045.000000}, {1840.000000, 1049.000000}, {1805.000000, 1059.000000}, {1776.000000, 1063.000000}, {1749.000000, 1071.000000}, {1719.000000, 1076.000000}, {1688.000000, 1076.000000}, {1662.000000, 1083.000000}, {1644.000000, 1096.000000}, {1675.000000, 1098.000000}, {1697.000000, 1098.000000}, {1721.000000, 1098.000000}, {1750.000000, 1108.000000}, {1777.000000, 1115.000000}, {1802.000000, 1120.000000}, {1827.000000, 1120.000000}, {1853.000000, 1122.000000}, {1871.000000, 1134.000000}, {1883.000000, 1160.000000}, {1888.000000, 1191.000000}, {1892.000000, 1219.000000}, {1902.000000, 1247.000000}, {1908.000000, 1273.000000}, {1922.000000, 1283.000000}, {1926.000000, 1247.000000}, {1936.000000, 1208.000000}, {1957.000000, 1161.000000}, {1968.000000, 1127.000000}, {1993.000000, 1117.000000}, {2023.000000, 1110.000000}, {2054.000000, 1107.000000}, {2090.000000, 1102.000000}, {2122.000000, 1095.000000}, {2160.000000, 1093.000000}, {2190.000000, 1090.000000}, {2172.000000, 1081.000000}, {2142.000000, 1081.000000}, {2110.000000, 1070.000000}, {2072.000000, 1051.000000}, {2041.000000, 1049.000000}, {2016.000000, 1049.000000}, {1988.000000, 1042.000000}, {1947.000000, 971.000000}, {1935.000000, 938.000000}, {1923.000000, 902.000000}};
	// std::vector<vec2> heart_points = {{1906.000000, 950.000000}, {1875.000000, 925.000000}, {1847.000000, 911.000000}, {1816.000000, 897.000000}, {1792.000000, 892.000000}, {1758.000000, 880.000000}, {1726.000000, 880.000000}, {1694.000000, 897.000000}, {1670.000000, 908.000000}, {1643.000000, 925.000000}, {1626.000000, 950.000000}, {1622.000000, 974.000000}, {1616.000000, 993.000000}, {1616.000000, 1021.000000}, {1618.000000, 1050.000000}, {1635.000000, 1068.000000}, {1649.000000, 1089.000000}, {1666.000000, 1108.000000}, {1684.000000, 1125.000000}, {1707.000000, 1144.000000}, {1729.000000, 1154.000000}, {1753.000000, 1170.000000}, {1775.000000, 1186.000000}, {1794.000000, 1205.000000}, {1823.000000, 1222.000000}, {1852.000000, 1239.000000}, {1876.000000, 1253.000000}, {1900.000000, 1261.000000}, {1921.000000, 1268.000000}, {1946.000000, 1270.000000}, {1976.000000, 1260.000000}, {2005.000000, 1242.000000}, {2038.000000, 1210.000000}, {2068.000000, 1189.000000}, {2098.000000, 1176.000000}, {2131.000000, 1160.000000}, {2163.000000, 1134.000000}, {2195.000000, 1090.000000}, {2224.000000, 1061.000000}, {2235.000000, 1022.000000}, {2237.000000, 992.000000}, {2164.000000, 908.000000}, {2129.000000, 898.000000}, {2098.000000, 893.000000}, {2070.000000, 893.000000}, {2037.000000, 898.000000}, {2012.000000, 920.000000}, {1984.000000, 942.000000}, {1960.000000, 954.000000}, {1933.000000, 972.000000}};
	// std::vector<vec2> pentagon_points = {{1885.000000, 896.000000}, {1856.000000, 910.000000}, {1825.000000, 922.000000}, {1794.000000, 939.000000}, {1765.000000, 958.000000}, {1739.000000, 977.000000}, {1713.000000, 989.000000}, {1684.000000, 993.000000}, {1654.000000, 1005.000000}, {1633.000000, 1017.000000}, {1615.000000, 1031.000000}, {1605.000000, 1049.000000}, {1628.000000, 1070.000000}, {1637.000000, 1092.000000}, {1647.000000, 1118.000000}, {1655.000000, 1146.000000}, {1662.000000, 1174.000000}, {1674.000000, 1202.000000}, {1687.000000, 1221.000000}, {1703.000000, 1241.000000}, {1728.000000, 1276.000000}, {1751.000000, 1276.000000}, {1776.000000, 1281.000000}, {1802.000000, 1281.000000}, {1826.000000, 1281.000000}, {1852.000000, 1279.000000}, {1883.000000, 1279.000000}, {1910.000000, 1277.000000}, {1938.000000, 1277.000000}, {1968.000000, 1274.000000}, {1999.000000, 1274.000000}, {2025.000000, 1272.000000}, {2056.000000, 1272.000000}, {2086.000000, 1272.000000}, {2109.000000, 1273.000000}, {2139.000000, 1230.000000}, {2168.000000, 1181.000000}, {2185.000000, 1147.000000}, {2204.000000, 1097.000000}, {2225.000000, 1050.000000}, {2211.000000, 1021.000000}, {2175.000000, 1012.000000}, {2139.000000, 996.000000}, {2100.000000, 973.000000}, {2073.000000, 964.000000}, {2039.000000, 945.000000}, {2010.000000, 924.000000}, {1979.000000, 913.000000}, {1952.000000, 903.000000}, {1933.000000, 893.000000}};
	float player_1_score = 0;
	// float player_2_score = 0;
};

struct StartScreen {
	bool active = true;
	bool restarted = false;
	bool level_select = false;
};

// Drawing
struct Drawing
{
	TEXTURE_ASSET_ID drawingType;
};

struct Items {
	TEXTURE_ASSET_ID item_type = TEXTURE_ASSET_ID::DICE_ITEM;
	bool outOfService = false;
	bool tooExpensive = false;
};

enum class BTPlayerState {
	Running,
	StartRun,
	In_jump_range,
	Jumping,
	Falling,
	Stunned,
	Goal_in_sight,
	Running_to_goal,
	DEBUG
};

enum class KFAnimationState {
	POSITION,
	COLOUR,
	ROTATION
};

struct CoinInfo {
	std::vector<int> numCoins;
	float coinDelayMs = 500.f;
	float coinDelayRemaining = 0;
	std::vector<float> startingPositions;
};

struct KeyframeAnimation {
	std::vector<vec2> kfPositions;
	std::vector<vec3> kfColours;
	std::vector<float> kfRotations;
	std::vector<float> kfScale;
	std::vector<float> kfAlpha;

	float lifeTimeMs = 10000.0f;
	float lifeRemaining = 0.0f;
	float larpTimeMs = 0.0f;
	float larpRemaining = 0.0f;

	bool active = false;
	TEXTURE_ASSET_ID type;
};
