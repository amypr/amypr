// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include "board_system.hpp"
#include "platformer_system.hpp"
#include "tank_system.hpp"
#include "components.hpp"

// stlib
#include <cassert>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>

#include "physics_system.hpp"
#include "drawing_game.hpp"
#include "menu.hpp"

// Game configuration
const size_t MAX_TURTLES = 15;
const size_t TURTLE_DELAY_MS = 2000 * 3;

const size_t MAX_FISH = 5;
const size_t AI_DELAY_MS = 2000 * 3;

int map1[22][39] = {
 {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
 {2,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,1,0,0,0,0,0,0,2,1,1,1,0,0,0,0,0,0,0,2,0,0,1,0,0,0,0,0,1,1,1,1,1,1,1,2},
 {2,0,0,0,1,0,0,0,0,0,0,2,0,0,1,0,0,1,0,0,0,0,2,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,2},
 {2,0,0,0,1,1,1,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,1,1,0,0,0,0,0,0,2},
 {2,0,0,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,2,2,0,0,2},
 {2,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,2,0,0,0,2},
 {2,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2},
 {2,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,0,0,0,2},
 {2,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,1,0,0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
 {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
};

int map2[22][39] = {
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
    {2,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,2,1,1,1,0,0,0,0,0,0,0,2,0,0,1,0,0,0,0,0,1,1,1,1,1,1,1,2},
    {2,0,0,0,0,0,0,0,0,0,0,2,0,0,1,0,0,1,0,0,0,0,2,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,2},
    {2,0,0,0,0,1,1,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,1,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,1,1,1,0,0,0,0,0,0,2},
    {2,0,0,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,2,2,0,0,2},
    {2,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,2,0,0,0,2},
    {2,0,0,0,0,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2},
    {2,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,0,0,0,2},
    {2,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,1,0,0,0,0,1,0,0,0,1,1,1,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,3,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
    {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
};

// create the run to the temple world
WorldSystem::WorldSystem()
	: points(0)
	, next_turtle_spawn(0.f)
	, next_wall_spawn(0.f) {
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem() {
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (salmon_dead_sound != nullptr)
		Mix_FreeChunk(salmon_dead_sound);
	if (salmon_eat_sound != nullptr)
		Mix_FreeChunk(salmon_eat_sound);
	if (dice_pickup_sound != nullptr)
		Mix_FreeChunk(dice_pickup_sound);
	if (dice_roll_sound != nullptr)
		Mix_FreeChunk(dice_roll_sound);
	if (jump_1_sound != nullptr)
		Mix_FreeChunk(jump_1_sound);
	if(paint_1_sound != nullptr)
		Mix_FreeChunk(paint_1_sound);
    if(cannon_fire_sound != nullptr)
        Mix_FreeChunk(cannon_fire_sound);
	if (tank_music != nullptr)
		Mix_FreeChunk(tank_music);
    if (tank_hit_sound != nullptr)
        Mix_FreeChunk(tank_hit_sound);
    if (tank_on_fire_sound != nullptr)
        Mix_FreeChunk(tank_on_fire_sound);
	
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace {
	void glfw_err_cb(int error, const char* desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow* WorldSystem::create_window(int width, int height) {
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(width, height, "Salmon Game Assignment", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	auto cursor_btn_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->mouse_button_callback(_0, _1, _2); };
	// auto cursor_btn_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, double _3, double _4) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->mouse_button_menu(_0, _1, _2, _3, _4); };
	glfwSetMouseButtonCallback(window, cursor_btn_redirect);
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	salmon_dead_sound = Mix_LoadWAV(audio_path("salmon_dead.wav").c_str());
	salmon_eat_sound = Mix_LoadWAV(audio_path("salmon_eat.wav").c_str());
	dice_roll_sound = Mix_LoadWAV(audio_path("dice_roll.wav").c_str());
	dice_pickup_sound = Mix_LoadWAV(audio_path("dice_pickup.wav").c_str());
	jump_1_sound = Mix_LoadWAV(audio_path("jump_1.wav").c_str());
	paint_1_sound = Mix_LoadWAV(audio_path("paint_1.wav").c_str());
    cannon_fire_sound = Mix_LoadWAV(audio_path("cannon_fire.wav").c_str());
	tank_music = Mix_LoadWAV(audio_path("tank_music.wav").c_str());
    tank_hit_sound = Mix_LoadWAV(audio_path("tank_hit.wav").c_str());
    tank_on_fire_sound = Mix_LoadWAV(audio_path("tank_on_fire.wav").c_str());
	

	if (background_music == nullptr || salmon_dead_sound == nullptr || salmon_eat_sound == nullptr || dice_roll_sound == nullptr || dice_pickup_sound == nullptr || jump_1_sound == nullptr || cannon_fire_sound == nullptr || tank_music == nullptr || tank_hit_sound == nullptr || tank_on_fire_sound == nullptr) {
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n %s\n %s\n %s\n \n make sure the data directory is present",
			audio_path("music.wav").c_str(),
			audio_path("salmon_dead.wav").c_str(),
			audio_path("salmon_eat.wav").c_str(),
			audio_path("dice_roll.wav").c_str(),
			audio_path("dice_pickup.wav").c_str(),
			audio_path("jump_1.wav").c_str(),
			audio_path("paint_1.wav").c_str(),
			audio_path("cannon_fire.wav").c_str(),
			audio_path("tank_music.wav").c_str(),
            audio_path("tank_hit.wav").c_str(),
            audio_path("tank_on_fire.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem* renderer_arg, StartSystem& startSys, MenuSystem& menuSys) {
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);
	fprintf(stderr, "Loaded music\n");

	this->startSystem = startSys;
	this->menuSystem = menuSys;

	// Set all states to default
	restart_game();
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update) {
	// Get the screen dimensions
	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);

	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "";
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

	
	for (Entity entity : registry.dice.entities) {
		// progress timer
		GameDie& dice = registry.dice.get(entity);
		if(!dice.boom){
			continue;
		}
		dice.timer_ms -= elapsed_ms_since_last_update;
		// set explode finished
		if (dice.timer_ms < 0) {
			dice.boom = 2;
			dice.timer_ms = 0.0f;
		}
	}

	// Removing out of screen entities
	auto& motions_registry = registry.motions;
    
    TankGame& t = registry.tankGame.get(board_controller);
    if (t.loaded && t.active) {
        elapsed_ms = elapsed_ms_since_last_update;
        auto& tank_registry = registry.playerTanks;
        float health_tank1 = 0;
        float health_tank2 = 0;
        
        if (tank_registry.components.size() > 0) {
            for (uint i = 0; i < tank_registry.components.size(); i++) {
                PlayerTank& tank = tank_registry.components[i];
                tank.fire_interval += elapsed_ms;
                if (tank.id == 1) {
                    health_tank1 = tank.health;
                } else if (tank.id == 2) {
                    health_tank2 = tank.health;
                }
            }
        }
        
        if (t.level == 2 && registry.tankings.size() <= 0) {
            Entity entity = createTanking(renderer, {0,0});
            registry.colors.get(entity) = {1,0,0};
            Motion& motion = registry.motions.get(entity);
            motion.scale *= 1.2;
            motion.position = vec2((renderer->window_width_px / 39.0f) * 2.5f, (renderer->window_height_px / 22.0f) * 6.5);
        }

        if (t.level == 1) {
            title_ss << "Points1: " << registry.tankGamePoints.components[0].points << "  HP1: " << health_tank1 << "  Points2: " << registry.tankGamePoints.components[1].points << "  HP2: " << health_tank2;
        } else if (t.level == 2) {
            title_ss << "HP1: " << floor(health_tank1) << "  HP2: " << floor(health_tank2) << "  Tanking: " << registry.tankings.components[0].health;
        }
        glfwSetWindowTitle(window, title_ss.str().c_str());
       
        // Spawning new enemy tank
        next_turtle_spawn -= elapsed_ms_since_last_update * current_speed;
        while (registry.aiTanks.components.size() <= 3) {
            next_turtle_spawn -= ((AI_DELAY_MS / 2) + uniform_dist(rng) * (AI_DELAY_MS / 2));
            // Create enemy tank
            Entity entity = createAITank(renderer, {0,0});
            Motion& motion = registry.motions.get(entity);
			Physics& phyc = registry.physics.get(entity);
			phyc.mass = 50;
            if (uniform_dist(rng) < 0.5) {
                motion.position = vec2((renderer->window_width_px / 39.0f) * 28.5, (renderer->window_height_px / 22.0f) * 4.5);
            } else {
                motion.position = vec2((renderer->window_width_px / 39.0f) * 4.5, (renderer->window_height_px / 22.0f) * 17.5);
            }
                if (uniform_dist(rng) < 0.25) {
                    motion.velocity = vec2(-30.f, 0.f);
                } else if (0.3 <= uniform_dist(rng) < 0.6) {
                    motion.velocity = vec2(0.f , 30.f);
                }
                else if (0.5 <= uniform_dist(rng) < 0.75) {
                    motion.velocity = vec2(0.f, -30.f);
                }
                else {
                    motion.velocity = vec2(30.f, 0.f);
                }
            
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // the player who destroyed a total of 8 100-level AI tanks will win the game with 5 points added
        // however, the player can also win the game with 10 points added if the player destroys the other player
        // and 5 more 100-level AI tanks
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        
        GameBoard& b = registry.gameBoard.get(board_controller);
        if (registry.tankGamePoints.components[0].points > 700) {
            b.player_points[0] += 10;
            t.active = false;
        } else if (registry.tankGamePoints.components[1].points > 700) {
            b.player_points[1] += 10;
            t.active = false;
        }

        // Establish walls
        next_wall_spawn -= elapsed_ms_since_last_update * 1000000;
        if (registry.walls.components.size() <= MAX_FISH && next_wall_spawn < 0.f) {
            if (t.level == 1) {
                for (int i = 0; i < 22; i++) {
                 for (int j = 0; j < 39; j++) {
                  if (map1[i][j] == 1) {
                   Entity entity = createWall(renderer, { 0,0 });
                   Motion& motion = registry.motions.get(entity);
				   vec2 scale = vec2(renderer->window_width_px / 39.0f, renderer->window_height_px / 22.0f);
				   motion.position = vec2(scale.x * j + scale.x * 0.5f, scale.y * i + scale.y * 0.5f);
				   motion.scale = scale;
                   motion.velocity = vec2(0.f, 0.f);
                  }
                  else if (map1[i][j] == 2) {
                      Entity entity = createHardWall(renderer, { 0,0 });
                      Motion& motion = registry.motions.get(entity);
					  vec2 scale = vec2(renderer->window_width_px / 39.0f, renderer->window_height_px / 22.0f);
					  motion.position = vec2(scale.x * j + scale.x * 0.5f, scale.y * i + scale.y * 0.5f);
					  motion.scale = scale;
                      motion.velocity = vec2(0.f, 0.f);
                  }
                  else if (map1[i][j] == 3){
                      Entity entity = createHome(renderer, { 0,0 });
                      Motion& motion = registry.motions.get(entity);
					  vec2 scale = vec2(renderer->window_width_px / 39.0f, renderer->window_height_px / 22.0f);
					  motion.position = vec2(scale.x * j + scale.x * 0.5f, scale.y * i + scale.y * 0.5f);
					  motion.scale = { 3 * scale.x, 3 * scale.y };
                      motion.velocity = vec2(0.f, 0.f);
                  }
                 }
                }
            } else {
                for (int i = 0; i < 22; i++) {
                 for (int j = 0; j < 39; j++) {
                  if (map2[i][j] == 1) {
                   Entity entity = createWall(renderer, { 0,0 });
                   Motion& motion = registry.motions.get(entity);
				   vec2 scale = vec2(renderer->window_width_px / 39.0f, renderer->window_height_px / 22.0f);
				   motion.position = vec2(scale.x * j + scale.x * 0.5f, scale.y * i + scale.y * 0.5f);
				   motion.scale = scale;
                   motion.velocity = vec2(0.f, 0.f);
                  }
                  else if (map2[i][j] == 2) {
                      Entity entity = createHardWall(renderer, { 0,0 });
                      Motion& motion = registry.motions.get(entity);
					  vec2 scale = vec2(renderer->window_width_px / 39.0f, renderer->window_height_px / 22.0f);
					  motion.position = vec2(scale.x * j + scale.x * 0.5f, scale.y * i + scale.y * 0.5f);
					  motion.scale = scale;
                      motion.velocity = vec2(0.f, 0.f);
                  }
                  else if (map2[i][j] == 3){
                      Entity entity = createHome(renderer, { 0,0 });
                      Motion& motion = registry.motions.get(entity);
					  vec2 scale = vec2(renderer->window_width_px / 39.0f, renderer->window_height_px / 22.0f);
					  motion.position = vec2(scale.x * j + scale.x * 0.5f, scale.y * i + scale.y * 0.5f);
					  motion.scale = { 3 * scale.x, 3 * scale.y };;
                      motion.velocity = vec2(0.f, 0.f);
                  }
                 }
                }
            }
        }
    }
    }

	// check stun timers for characters in platformer
	// this will be empty if platformer is inactive
	// code modified from template assignment
	float min_counter_ms = 2000.f;
	for (Entity entity : registry.stunTimers.entities) {
		// progress timer
		StunTimer& counter = registry.stunTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if (counter.counter_ms < min_counter_ms) {
			min_counter_ms = counter.counter_ms;
		}

		// remove stun components once the stun timer expired
		if (counter.counter_ms < 0) {
			registry.stunTimers.remove(entity);
		}

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	/*for (int i = (int)motions_registry.components.size()-1; i>=0; --i) {
		Motion& motion = motions_registry.components[i];
		if (motion.position.x + abs(motion.scale.x) < 0.f) {
			registry.remove_all_components_of(motions_registry.entities[i]);
		}
	}*/

	// get time since last update for key press references
	elapsed_ms = elapsed_ms_since_last_update;

	assert(registry.screenStates.components.size() <= 1);
	ScreenState& screen = registry.screenStates.components[0];

	return true;
}
return false;
}

// Reset the world state to its initial state
void WorldSystem::restart_game() {
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	GameBoard& b = registry.gameBoard.get(board_controller);
	Platformer& p = registry.platformer.get(board_controller);
    TankGame& t = registry.tankGame.get(board_controller);
	DrawingGame& d = registry.drawingGame.get(board_controller);
	StartScreen& ss = registry.startScreen.get(board_controller);
	// do drawing game when merged with the fixed version of drawing TODO
	b.active = false;
	p.active = false;
    t.active = false;
	d.active = false;

	ss.restarted = true;
	ss.active = true;
	// Reset the game speed

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
	// while (registry.motions.entities.size() > 0)
	//     registry.remove_all_components_of(registry.motions.entities.back());

	// registry.remove_all_components_of(registry.gameBoard.entities.back());
	// registry.gameBoard.components.clear();
	// registry.gameBoard.remove(board_controller);

	// // Debugging for memory/component leaks

	registry.list_all_components();
}

// Compute collisions between entities
void WorldSystem::handle_collisions() {
	// Loop over all collisions detected by the physics system
	auto& collisionsRegistry = registry.collisions; // TODO: @Tim, is the reference here needed?
	for (uint i = 0; i < collisionsRegistry.components.size(); i++) {
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;

		// For now, we are only interested in collisions that involve the player
		if (registry.platformerPhysics.has(entity)) {
			/// only considering those affected by platformer physics (for platformer)
			Platformer& platformer = registry.platformer.get(board_controller);
			if (platformer.loaded && platformer.active) {
				// if a player-platformer collision, it will only be in this registry if it's a collision where player is above platform
				if (registry.platforms.has(entity_other)) {
					Motion& motion = registry.motions.get(entity);
					Motion& otherMotion = registry.motions.get(entity_other);
				
					// stop descent, set position to above platform
					motion.velocity.y = 0;
					motion.position.y = otherMotion.position.y - motion.scale.y;
				}
				else if (registry.goals.has(entity_other)) {
					GameBoard& g = registry.gameBoard.get(board_controller);
					
					if (entity == registry.sprites.entities[0]) {
						// add points to player 1
						if (g.active_item[0] == TEXTURE_ASSET_ID::INV_TREASURE_1) {
							g.player_points[0] += 10;
						} else {
							g.player_points[0] += 5;
						}
					}
					else if (entity == registry.sprites.entities[1]) {
						// add points to player 2
						if (g.active_item[1] == TEXTURE_ASSET_ID::INV_TREASURE_1) {
							g.player_points[1] += 10;
						} else {
							g.player_points[1] += 5;
						}
					}
					else if (entity == registry.sprites.entities[2]) {
						// add points to player 3 / ai 1
						g.player_points[2] += 5;
					}
					else if (entity == registry.sprites.entities[3]) {
						// add points to player 4 / ai 2
						g.player_points[3] += 5;
					}

					for (int i = 0; i < g.players; i++) {
						if (g.active_item[i] == TEXTURE_ASSET_ID::INV_TREASURE_1) {
							g.active_item[i] = TEXTURE_ASSET_ID::BOARD_HELP; //dummy to avoid using any other items once done
						}
					}
					platformer.active = false;
					g.active = true;
					g.turn = 0;
					g.stage++;
				}
				else if (registry.traps.has(entity_other)) {
					if (!registry.stunTimers.has(entity)) {
						Motion& motion = registry.motions.get(entity);

						// stop movement of character and set character to "stunned" state
						motion.velocity *= 0;
						registry.stunTimers.emplace(entity);
					}
					// remove trap after it's been triggered
					registry.remove_all_components_of(entity_other);
				}
			}


		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}

// Should the game be over ?
bool WorldSystem::is_over() const {
	return bool(glfwWindowShouldClose(window));
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod) {

	Menu& menu = registry.menu.get(board_controller);
	StartScreen ss = registry.startScreen.get(board_controller);
	GameBoard& g = registry.gameBoard.get(board_controller);

	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE) {
		if (menu.paused) {
			menuSystem.deconstructMenu();
		}
		// cannot pause when:
		// on start screen
		else if (!ss.active) {
			menu.paused = !menu.paused;
		}
	}
	// prevent actions if currently paused, on start screen, or intro dialogue/help screen is open
	if (!menu.paused && !ss.active && g.introSeen && registry.helpScreen.size() == 0) {
		if ((action == GLFW_PRESS || action == GLFW_REPEAT || action == GLFW_RELEASE) &&
			((key == GLFW_KEY_D || key == GLFW_KEY_A || key == GLFW_KEY_W) ||
				(key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT || key == GLFW_KEY_UP ||
				key == GLFW_KEY_DOWN || key == GLFW_KEY_PERIOD) ||
             (key == GLFW_KEY_A || key == GLFW_KEY_D || key == GLFW_KEY_V || key == GLFW_KEY_W ||
             key == GLFW_KEY_S || key == GLFW_KEY_SPACE))) {
			auto& playersRegistry = registry.playerTanks;
            
            TankGame& t = registry.tankGame.get(board_controller);
            GameBoard& g = registry.gameBoard.get(board_controller);
			// only use these set of controls if on platformer mode
			Platformer& p = registry.platformer.get(board_controller);
			if (p.loaded && p.active) {
				// base velocity on window size
				int width, height;
				glfwGetWindowSize(window, &width, &height);
				float horizontalVelocity = width / 7.f;
				float verticalVelocity = height / 2.f;
				for (uint i = 0; i < playersRegistry.components.size(); i++) {
					Entity player = playersRegistry.entities[i];
					if (!registry.sprites.has(player)) {
						continue;
					}
					// todo project: can repurpose to check if player stunned
					if (registry.stunTimers.has(player)) {
						continue;
					}
					// get player motion + sprite
					Motion& motion = registry.motions.get(player);
					Sprite& sprite = registry.sprites.get(player);

					// player 1 controls
					if (player == registry.sprites.entities[0]) {
						if (key == GLFW_KEY_D) {
							motion.velocity.x = horizontalVelocity;

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
						else if (key == GLFW_KEY_A) {
							motion.velocity.x = -horizontalVelocity;

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
						else if (key == GLFW_KEY_W) {
							// don't affect movement if already in motion (may need to change when collision implemented)
							if (motion.velocity.y != 0) {
								continue;
							}
							else if (action != GLFW_REPEAT) {
								motion.velocity.y = -verticalVelocity;
								Mix_PlayChannel(-1, jump_1_sound, 0);
							}
						}
					}

					// player 2 controls
					else if (player == registry.sprites.entities[1]) {
						if (key == GLFW_KEY_RIGHT) {
							motion.velocity.x = horizontalVelocity;

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
						else if (key == GLFW_KEY_LEFT) {
							motion.velocity.x = -horizontalVelocity;

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
						else if (key == GLFW_KEY_UP) {
							// don't affect movement if already in motion (may need to change when collision implemented)
							if (motion.velocity.y != 0) {
								continue;
							}
							else if (action != GLFW_REPEAT) {
								motion.velocity.y = -verticalVelocity;
								Mix_PlayChannel(-1, jump_1_sound, 0);
							}
						}
					}
				}
			}
            if (t.loaded && t.active) {
                auto& tank_registry = registry.playerTanks;
                if (tank_registry.components.size() > 0) {
                    for (uint i = 0; i < tank_registry.components.size(); i++) {
                        PlayerTank& tank = tank_registry.components[i];
                        Entity entity = tank_registry.entities[i];
                        Motion& motion = registry.motions.get(entity);
                        
                        if (tank.id == 1) {
                            if (key == GLFW_KEY_RIGHT) {
                                if (action == GLFW_RELEASE) {
                                    motion.velocity.x = 0;
                                }
                                else {
                                    motion.velocity.x = (renderer->window_width_px / 39.0f)*2;
                                    motion.angle = atan2(0, -1);
                                }
                                motion.velocity.y = 0;
                            }
                            if (key == GLFW_KEY_LEFT) {
                                if (action == GLFW_RELEASE) {
                                    motion.velocity.x = 0;
                                }
                                else {
                                    motion.velocity.x = -(renderer->window_width_px / 39.0f) * 2;
                                    motion.angle = atan2(0, 1);
                                }
                                motion.velocity.y = 0;
                            }

                            if (key == GLFW_KEY_UP) {
                                if (action == GLFW_RELEASE) {
                                    motion.velocity.y = 0;
                                }
                                else {
                                    motion.velocity.y = -(renderer->window_height_px / 22.0f) * 2;
                                    motion.angle = atan2(1, 0);
                                }
                                motion.velocity.x = 0;
                            }
                            if (key == GLFW_KEY_DOWN) {
                                if (action == GLFW_RELEASE) {
                                    motion.velocity.y = 0;
                                }
                                else {
                                    motion.velocity.y = (renderer->window_height_px / 22.0f) * 2;
                                    motion.angle = atan2(-1, 0);
                                }
                                motion.velocity.x = 0;
                            }
                            if (key == GLFW_KEY_PERIOD) {
                                if (action == GLFW_PRESS) {
                                    float interval = 1500;
                                    float bullet_speed_factor_y = 22.0f;
                                    float bullet_speed_factor_x = 39.0f;
                                    if (g.active_item[0] == TEXTURE_ASSET_ID::INV_ENHANCED_CANNON) {
                                        interval = 1000;
                                        bullet_speed_factor_y = 20.0f;
                                        bullet_speed_factor_x = 36.0f;
                                    }
                                    if (tank.fire_interval >= interval) {
                                        // std::cout << registry.players.get(player_tank).fire_interval;
                                        Mix_PlayChannel(-1, cannon_fire_sound, 0);
                                        vec2 player_position = motion.position;
                                        Entity player_bullet;
                                        if (g.active_item[0] == TEXTURE_ASSET_ID::INV_HEAVY_DAMAGE) {
                                            player_bullet = createPlayerBullet(player_position, {18,18});
                                        } else {
                                            player_bullet = createPlayerBullet(player_position, {15,15});
                                        }
                                        PlayerBullet& bullet = registry.playerBullets.get(player_bullet);
                                        
                                        if (g.active_item[0] == TEXTURE_ASSET_ID::INV_ENHANCED_CANNON) {
                                            registry.colors.get(player_bullet) = {0.3,0.2,0.5};
                                        }
                                        bullet.shot_by = 1;
                                        if (g.active_item[0] == TEXTURE_ASSET_ID::INV_HEAVY_DAMAGE) {
                                            bullet.damage += 50;
                                        }
                                        // registry.motions.get(player_bullet).velocity = {100, 0};
                                        std::cout << (motion.angle);
                                        // player tank is facing up
                                        if (motion.angle <= 1.58 && motion.angle >= 1.57) {
                                            registry.motions.get(player_bullet).velocity = {0,-(renderer->window_height_px / bullet_speed_factor_y) * 4};
                                            registry.motions.get(player_bullet).position.y -= abs(motion.scale.x/2);
                                        // player tank is facing down
                                        } else if (motion.angle >= -1.58 && motion.angle <= -1.57) {
                                            registry.motions.get(player_bullet).velocity = {0,(renderer->window_height_px / bullet_speed_factor_y) * 4};
                                            registry.motions.get(player_bullet).position.y += abs(motion.scale.x/2);
                                        // player tank is facing right
                                        } else if (motion.angle == 0) {
                                            registry.motions.get(player_bullet).velocity = {-(renderer->window_width_px / bullet_speed_factor_x) * 4,0};
                                            registry.motions.get(player_bullet).position.x -= abs(motion.scale.x / 2);
                                        // player tank is facing left
                                        } else if (motion.angle >= 3.14 && motion.angle <= 3.15) {
                                            registry.motions.get(player_bullet).velocity = { (renderer->window_width_px / bullet_speed_factor_x) * 4,0};
                                            registry.motions.get(player_bullet).position.x += abs(motion.scale.x / 2);
                                        }
                                        tank.fire_interval = 0;
                                    }
                                }
                            }
                        } else if (tank.id == 2) {
                            if (key == GLFW_KEY_D) {
                                if (action == GLFW_RELEASE) {
                                    motion.velocity.x = 0;
                                }
                                else {
                                    motion.velocity.x = (renderer->window_width_px / 39.0f) * 2;
                                    motion.angle = atan2(0, -1);
                                }
                                motion.velocity.y = 0;
                            }
                            if (key == GLFW_KEY_A) {
                                if (action == GLFW_RELEASE) {
                                    motion.velocity.x = 0;
                                }
                                else {
                                    motion.velocity.x = -(renderer->window_width_px / 39.0f) * 2;
                                    motion.angle = atan2(0, 1);
                                }
                                motion.velocity.y = 0;
                            }

                            if (key == GLFW_KEY_W) {
                                if (action == GLFW_RELEASE) {
                                    motion.velocity.y = 0;
                                }
                                else {
                                    motion.velocity.y = -(renderer->window_height_px / 22.0f) * 2;
                                    motion.angle = atan2(1, 0);
                                }
                                motion.velocity.x = 0;
                            }
                            if (key == GLFW_KEY_S) {
                                if (action == GLFW_RELEASE) {
                                    motion.velocity.y = 0;
                                }
                                else {
                                    motion.velocity.y = (renderer->window_height_px / 22.0f) * 2;
                                    motion.angle = atan2(-1, 0);
                                }
                                motion.velocity.x = 0;
                            }
                            if (key == GLFW_KEY_SPACE) {
                                if (action == GLFW_PRESS) {
                                    float interval = 1500;
                                    float bullet_speed_factor_y = 22.0f;
                                    float bullet_speed_factor_x = 39.0f;
                                    if (g.active_item[1] == TEXTURE_ASSET_ID::INV_ENHANCED_CANNON) {
                                        interval = 1000;
                                        bullet_speed_factor_y = 20.0f;
                                        bullet_speed_factor_x = 36.0f;
                                    }
                                    if (tank.fire_interval >= interval) {
                                        // std::cout << registry.players.get(player_tank).fire_interval;
                                        Mix_PlayChannel(-1, cannon_fire_sound, 0);
                                        vec2 player_position = motion.position;
                                        Entity player_bullet;
                                        if (g.active_item[1] == TEXTURE_ASSET_ID::INV_HEAVY_DAMAGE) {
                                            player_bullet = createPlayerBullet(player_position, {18,18});
                                        } else {
                                            player_bullet = createPlayerBullet(player_position, {15,15});
                                        }
                                        PlayerBullet& bullet = registry.playerBullets.get(player_bullet);
                                        if (g.active_item[1] == TEXTURE_ASSET_ID::INV_ENHANCED_CANNON) {
                                            registry.colors.get(player_bullet) = {0.3,0.2,0.5};
                                        }
                                        bullet.shot_by = 2;
                                        if (g.active_item[1] == TEXTURE_ASSET_ID::INV_HEAVY_DAMAGE) {
                                            bullet.damage += 50;
                                        }
                                        // registry.motions.get(player_bullet).velocity = {100, 0};
                                        std::cout << (motion.angle);
                                        // player tank is facing up
                                        if (motion.angle <= 1.58 && motion.angle >= 1.57) {
                                            registry.motions.get(player_bullet).velocity = {0,-(renderer->window_height_px / bullet_speed_factor_y) * 4};
                                            registry.motions.get(player_bullet).position.y -= abs(motion.scale.x / 2);
                                        // player tank is facing down
                                        } else if (motion.angle >= -1.58 && motion.angle <= -1.57) {
                                            registry.motions.get(player_bullet).velocity = {0,(renderer->window_height_px / bullet_speed_factor_y) * 4 };
                                            registry.motions.get(player_bullet).position.y += abs(motion.scale.x / 2);
                                        // player tank is facing right
                                        } else if (motion.angle == 0) {
                                            registry.motions.get(player_bullet).velocity = {-(renderer->window_width_px / bullet_speed_factor_x) * 4,0};
                                            registry.motions.get(player_bullet).position.x -= abs(motion.scale.x / 2);
                                        // player tank is facing left
                                        } else if (motion.angle >= 3.14 && motion.angle <= 3.15) {
                                            registry.motions.get(player_bullet).velocity = { (renderer->window_width_px / bullet_speed_factor_x) * 4,0};
                                            registry.motions.get(player_bullet).position.x += abs(motion.scale.x / 2);
                                        }
                                        tank.fire_interval = 0;
                                    }
                                }
                            }
                        }
                    }
                }
                
                if (key == GLFW_KEY_V) {
                        if (action == GLFW_RELEASE)
                            debugging.in_debug_mode = false;
                        else
                            debugging.in_debug_mode = true;
                }
                for (Entity entity : registry.lightUps.entities) {
                    LightUp& counter = registry.lightUps.get(entity);
                    counter.counter_ms -= 3000;
                    std::cout << counter.counter_ms;
                    if (counter.counter_ms < 0){
                        registry.lightUps.remove(entity);
                    }
                }
            }

		}

		if (action == GLFW_RELEASE &&
			((key == GLFW_KEY_D || key == GLFW_KEY_A || key == GLFW_KEY_W) ||
				(key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT || key == GLFW_KEY_UP))) {
			auto& playersRegistry = registry.playerTanks;

			// only use these set of controls if on platformer mode
			Platformer& p = registry.platformer.get(board_controller);
			if (p.loaded && p.active) {
				for (uint i = 0; i < playersRegistry.components.size(); i++) {
					Entity player = playersRegistry.entities[i];
					// these controls are specific to sprites from platformer game, prevent control of any other "player" entities
					if (!registry.sprites.has(player)) {
						continue;
					}

					// todo project: can repurpose to check if player stunned
					if (registry.stunTimers.has(player)) {
						continue;
					}
					// get player motion + sprite
					Motion& motion = registry.motions.get(player);
					Sprite& sprite = registry.sprites.get(player);

					// player 1 controls
					if (player == registry.sprites.entities[0]) {
						if (key == GLFW_KEY_D) {
							// only stop motion if key released = most recent key pressed i.e. don't stop moving right if you lift the left key
							if (motion.velocity.x > 0) {
								motion.velocity.x = 0;
								sprite.frame.x = 0;
							}

						}
						else if (key == GLFW_KEY_A) {
							if (motion.velocity.x < 0) {
								motion.velocity.x = 0;
								sprite.frame.x = 0;
							}
						}
					}
					else {
						if (key == GLFW_KEY_RIGHT) {
							if (motion.velocity.x > 0) {
								motion.velocity.x = 0;
								sprite.frame.x = 0;
							}

						}
						else if (key == GLFW_KEY_LEFT) {
							if (motion.velocity.x < 0) {
								motion.velocity.x = 0;
								sprite.frame.x = 0;
							}
						}
					}

				}
			}
		}

		// switch btwn active and inactive platformer
		//if (action == GLFW_RELEASE && key == GLFW_KEY_Y) {
		//	Platformer& p = registry.platformer.get(board_controller);
		//	p.active = !p.active;
		//}
		// Debugging
		if (key == GLFW_KEY_Q) {
			if (action == GLFW_RELEASE)
				debugging.in_debug_mode = false;
			else
				debugging.in_debug_mode = true;
		}

		// GameBoard controls
		// debug game board stages
		// if (key == GLFW_KEY_B) {
		// 	if (action == GLFW_RELEASE){
		// 	g.stage -= 1;
		// 	}
		// }

		// if (key == GLFW_KEY_N) {
		// 	if (action == GLFW_RELEASE) {
		// 		g.stage += 1;
		// 	}
		// }

		if (key == GLFW_KEY_Q && g.active && (g.stage == 0 || g.stage == 10 || g.stage == 2) && !g.inv_active) { // ROll
			if (action == GLFW_RELEASE) {
				if(g.stage == 0){
					g.stage = 9;
				}else {
					bool allow = true;
					for (auto& dice: registry.dice.components){
						if(dice.controlled || dice.rolling){
							allow = false;
						}
					}
					if(allow){
						while (registry.dice.entities.size() > 0){
							registry.remove_all_components_of(registry.dice.entities.back());
						}		
						g.stage = 0;
					}
				}
			}
		}

		// if (key == GLFW_KEY_S  && g.active && (g.stage == 0 || g.stage == 1)) { // ITEM select
			
		// 	if (action == GLFW_RELEASE) {
		// 		if(g.stage == 0){
		// 			g.stage = 1;
		// 		}else{
		// 			g.stage = 0;
		// 		}
		// 	}
		// }

		if (key == GLFW_KEY_E  && g.active && (g.stage == 0 || g.stage == 3)) { // Activate Edge scroll
			
			if (action == GLFW_RELEASE) {
				if(g.stage == 0){
					g.stage = 3;
				}else{
					g.stage = 0;
				}
				
			}
		}

		if (key == GLFW_KEY_I  && g.active && g.stage == 0 && g.active_item[g.turn] == TEXTURE_ASSET_ID::BOARD_HELP) { // Activate inventory
			if (action == GLFW_RELEASE) {
				if(g.inv_active && g.inv_loaded){
					while (registry.items.entities.size() > 0) {
						registry.remove_all_components_of(registry.items.entities.back());
					}
					while (registry.shopBackground.entities.size() > 0) {
						registry.remove_all_components_of(registry.shopBackground.entities.back());
					};
					g.inv_loaded = false;
					g.inv_active = false;
				}else{
					g.inv_active = true;
				}
				
			}
		}
		

		Camera& camera = registry.camera.components[0];
		if (action == GLFW_PRESS && key == GLFW_KEY_Z) {
			if (camera.zoom < 1) {
				camera.zoom += 0.1;
			}
			else {
				camera.zoom += 1;
			}
			camera.screen_scale = static_cast<float>(camera.camera_fb_width) / (camera.camera_width / camera.zoom);
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_O) {
			if (camera.zoom <= 1) {
				camera.zoom -= 0.1;
			}
			else if (camera.zoom <= 0.1) {
				camera.zoom = 0.1;
			}
			else {
				camera.zoom -= 1;
			}
			camera.screen_scale = static_cast<float>(camera.camera_fb_width) / (camera.camera_width / camera.zoom);
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_L) {
			camera.x -= 1;
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_J) {
			camera.x += 1;
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_P) {
			camera.y -= 1;
		}
		if (action == GLFW_PRESS && key == GLFW_KEY_K) {
			camera.y += 1;
		}
		if (key == GLFW_KEY_C) {
			DrawingGame& drawingGame = registry.drawingGame.components[0];
			if (action == GLFW_RELEASE) {
				drawingGame.active = !drawingGame.active;
			}
		}
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_position) {
	//(vec2)mouse_position; // dummy to avoid compiler warning
	Menu& menu = registry.menu.get(board_controller);
	if (menu.paused) {
		return;
	}
	DrawingGame& drawingGame = registry.drawingGame.get(board_controller);
	auto& board = registry.gameBoard.get(board_controller);
	if (board.active == true) {
		// Get the screen dimensions
		int screen_width, screen_height;
		glfwGetFramebufferSize(window, &screen_width, &screen_height);
		Camera& camera = registry.camera.components[0];
		float mouseShiftX = -camera.x * screen_width * 0.5f;
		int diceIteration = 1;
		float offset = screen_width * 0.05f;
		for (Entity e : registry.dice.entities) {
			GameDie& d = registry.dice.get(e);
			if (registry.motions.has(e) && d.controlled == true) {
				Motion& m = registry.motions.get(e);
				float angle = atan2(mouse_position.y - m.position.y, (mouse_position.x + mouseShiftX + offset * diceIteration) - m.position.x);
				float angle2 = atan2(mouse_position.y - m.position.y, mouse_position.x + mouseShiftX - m.position.x);
				d.angular_velocity = max(m.angle - angle2, 1.0f); // max() prevents degenerate case where no rotation occurs
				m.angle = angle;
				vec2 pos = vec2(mouse_position.x + mouseShiftX + offset * diceIteration, mouse_position.y);
				d.velocity = vec2(m.position.x, m.position.y) - pos;
				m.position = pos;
				diceIteration++;
			}
		}

		// game board edge scrolling request
		// int screen_width = this->renderer->window_width_px;
		if (fmod(mouse_position.x, screen_width) <= screen_width * 0.05f) {
			board.xAxisScroll = 0.05f;
		}
		else if (fmod(mouse_position.x, screen_width) >= screen_width * 0.95f) {
			board.xAxisScroll = -0.05f;
		}
		else {
			board.xAxisScroll = 0.0f;
		}
	}
	if (drawingGame.active && registry.mouseHold.components.size() > 0) {
		MouseHold& mh = registry.mouseHold.components[0];
		if (mh.isMouseHolded) {
			vec2 dot_scale = { 20, 20};
			mh.points.push_back(mouse_position);
			drawingSystem.drawLine(mouse_position, dot_scale);
		}
	}
}


void WorldSystem::mouse_button_callback(int button, int action, int mods)
{
	Menu& menu = registry.menu.get(board_controller);
	StartScreen& startScreen = registry.startScreen.get(board_controller);
	GameBoard& g = registry.gameBoard.get(board_controller);
	DrawingGame& drawingGame = registry.drawingGame.get(board_controller);
	Platformer& platformer = registry.platformer.get(board_controller);
	TankGame& tankGame = registry.tankGame.get(board_controller);
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// remove help screen from view if there is one active + player clicks anywhere on screen
	// this also prevents player from clicking over things while the help screen is active
	if (registry.helpScreen.size() != 0) {
		if (button == GLFW_MOUSE_BUTTON_1) {
			if (action == GLFW_RELEASE) {
				while (registry.helpScreen.entities.size() > 0) {
					registry.remove_all_components_of(registry.helpScreen.entities.back());
				}
				// deactivate menu aka stop game from being paused if solo help screen
				// won't remove entire pause menu if help spawned from help button
				if (menu.menuType == 1) {
					menuSystem.deconstructMenu();
				}
				
			}
		}
	}
	// if menu paused, check to see if button is clicked
	else if (menu.paused) {
		if (button == GLFW_MOUSE_BUTTON_1) {
			if (action == GLFW_RELEASE) {
				for (int i = 0; i < registry.buttons.components.size(); i++) {
					Entity entity_i = registry.buttons.entities[i];
					Motion motion_i = registry.motions.get(entity_i);
					float new_xpos;
					if (xpos < width/2) {
						new_xpos = -(width / 2 - xpos);
					}
					else {
						new_xpos = xpos - width / 2;
					}
					if (motion_i.position.x + new_xpos < motion_i.position.x + motion_i.scale.x / 2 &&
						motion_i.position.x + new_xpos > motion_i.position.x - motion_i.scale.x / 2 &&
						ypos < motion_i.position.y + motion_i.scale.y / 2 &&
						ypos > motion_i.position.y - motion_i.scale.y / 2) {
						Buttons button = registry.buttons.get(entity_i);
						if (button.button_type == 0) {
							if (g.active) {
								menuSystem.helpScreens(TEXTURE_ASSET_ID::BOARD_HELP);
							} else if (drawingGame.active) {
								menuSystem.helpScreens(TEXTURE_ASSET_ID::DRAWING_HELP);
							} else if (platformer.active) {
								menuSystem.helpScreens(TEXTURE_ASSET_ID::PLATFORMER_HELP);
							} else if (tankGame.active) {
                                if (tankGame.level == 1) {
                                    menuSystem.helpScreens(TEXTURE_ASSET_ID::TANK_HELP_1);
                                } else if (tankGame.level == 2) {
                                    menuSystem.helpScreens(TEXTURE_ASSET_ID::TANK_HELP_2);
                                }
							}
						}
						else if (button.button_type == 1) {
							if (g.active && g.stage == 0) {
								startSystem.serialize(g);
								printf("restarting game!!!!!!!!!!!!!!");
								restart_game();
							}
							return;
						}
						else if (button.button_type == 2) {
							restart_game();
							return;
						}
					}
				}
			}
		}
	}
	// if dialogue box is present (and menu + help are NOT)
	else if (registry.dialogue.size() != 0) {
		if (button == GLFW_MOUSE_BUTTON_1) {
			if (action == GLFW_RELEASE) {
				// only 1 dialogue box should be active at one time
				Entity entity = registry.dialogue.entities[0];
				Sprite& dialogueSprite = registry.sprites.get(entity);
				Dialogue& dialogue = registry.dialogue.get(entity);

				// dialogue = intro
				if (dialogue.dialogueType == 0) {
					// cycle through sprite sheet (aka go to next dialogue)
					dialogueSprite.frame.x++;
					dialogue.nextDialogueFrame++;
					// when frame exceeds the num of frames, dialogue has finished and boxes should be removed
					if (dialogueSprite.frame.x == dialogueSprite.num_frames) {
						while (registry.dialogue.entities.size() > 0) {
							registry.remove_all_components_of(registry.dialogue.entities.back());
						}
						g.introSeen = true;
					}	
				}
				// dialogue = end
				else if (dialogue.dialogueType == 1) {
					// proceed to specific frame ("Player X wins!")
					if (dialogueSprite.frame.x == 0) {
						dialogueSprite.frame.x = dialogue.nextDialogueFrame;
					}
					// already on ("Player X wins!") frame, click closes dialogue and ends game
					else {
						restart_game();
					}
				}
				// dialogue = tutorial
				else if (dialogue.dialogueType == 2) {
					// allow advance ALWAYS unless a key is active when player needs to transition to next key instruction (aka edge scrolling, die, or inventory?)
					// lets the player quickly skip through tutorial dialogue if they've seen it already (no current option to skip tutorial entirely)
					// but players can test out the key commands when appropriate
					// no keys are active if current stage is = 0, and if inventory screen is not active
					if (dialogueSprite.frame.x == 1 && g.inv_active) {
						return;
					}
					else if (g.stage == 0 || (dialogueSprite.frame.x == 2 && g.stage == 3) || (dialogueSprite.frame.x == 3 && g.stage == 10)) {
						dialogueSprite.frame.x++;
						dialogue.nextDialogueFrame++;
						// remove dialogue if done with all frames available
						if (dialogueSprite.frame.x == dialogueSprite.num_frames) {
							while (registry.dialogue.entities.size() > 0) {
								registry.remove_all_components_of(registry.dialogue.entities.back());
							}
							g.tutorialSeen = true;
							// open up help for board game so players learn controls
							menuSystem.helpScreens(TEXTURE_ASSET_ID::BOARD_HELP);
						}
					}
					
				}
			
			}
		}
	}
	// if start screen active, check to see if button is clicked
	else if (startScreen.active) {
		if (button == GLFW_MOUSE_BUTTON_1) {
			if (action == GLFW_RELEASE) {
				for (int i = 0; i < registry.buttons.components.size(); i++) {
					Entity entity_i = registry.buttons.entities[i];
					Motion motion_i = registry.motions.get(entity_i);
					if (xpos < motion_i.position.x + motion_i.scale.x / 2 &&
						xpos > motion_i.position.x - motion_i.scale.x / 2 &&
						ypos < motion_i.position.y + motion_i.scale.y / 2 &&
						ypos > motion_i.position.y - motion_i.scale.y / 2) {
						Buttons button = registry.buttons.get(entity_i);
						GameBoard& g = registry.gameBoard.get(board_controller);
						if (button.button_type == 3) {
							startSystem.buildLevelSelectScreen();
						}else if (button.button_type == 4) {
							if (!startSystem.loadSave()) {
								// load was unsuccessful, don't do anything
								return;
							}
							startSystem.deconstructScreen();
							startScreen.active = false;
						}else if ((button.button_type == 5 || button.button_type == 6) && startScreen.level_select){
							g.level = button.button_type == 5 ? "beach_1.json" : "jungle_1.json";
							g.stage = -99;
							startSystem.deconstructScreen();
							startScreen.active = false;
						}
						
					}
				}
			}

		}		
	}
	// if start screen not active + not on pause menu, the only other mouse click we care about is the dice roll
	else {
		if (g.active) {
			if (button == GLFW_MOUSE_BUTTON_1) {
				if (action == GLFW_PRESS ) {
					if (registry.items.size() != 0) {
						for (int i = 0; i < registry.items.components.size(); i++) {
							Entity entity_i = registry.items.entities[i];
							Motion motion_i = registry.motions.get(entity_i);
							Items& item = registry.items.get(entity_i);
							float camera_x_pos = menuSystem.cameraPositionX();
							float new_xpos;
							TEXTURE_ASSET_ID item_to_use = TEXTURE_ASSET_ID::INV_DICE;
							std::vector<TEXTURE_ASSET_ID>::iterator location;
							std::vector<TEXTURE_ASSET_ID> vector = g.playerInv[g.turn];
							TooManyItems& tooManyItems = registry.tooManyItems.get(board_controller);
							if (xpos < camera_x_pos / 2) {
								new_xpos = camera_x_pos - ((width / 2) - xpos);
							}
							else {
								new_xpos = (xpos - width / 2) + camera_x_pos;
							}
							if (new_xpos < motion_i.position.x + motion_i.scale.x / 2 &&
								new_xpos > motion_i.position.x - motion_i.scale.x / 2 &&
								ypos < motion_i.position.y + motion_i.scale.y / 2 &&
								ypos > motion_i.position.y - motion_i.scale.y / 2) {
								bool inventory_stage = g.inv_active && g.inv_loaded;
								switch(item.item_type){
									case TEXTURE_ASSET_ID::DICE_ITEM:
										if (g.playerInv[g.turn].size() < 9) {
											if (g.player_points[g.turn] >= 4) {
												g.player_points[g.turn] -= 4;
												g.playerInv[g.turn].push_back(TEXTURE_ASSET_ID::INV_DICE);
											} else {
												item.tooExpensive = true;
												return;
											}
										} else {
											tooManyItems.too_many = true;
											return;
										}
										break;
									case TEXTURE_ASSET_ID::INV_DICE:
										item_to_use = TEXTURE_ASSET_ID::INV_DICE;
										location = std::find(vector.begin(), vector.end(), TEXTURE_ASSET_ID::INV_DICE);
										break;
									case TEXTURE_ASSET_ID::HEART_ITEM:
										if (g.playerInv[g.turn].size() < 9) {
											if (g.player_points[g.turn] >= 4) {
												g.player_points[g.turn] -= 4;
												g.playerInv[g.turn].push_back(TEXTURE_ASSET_ID::INV_HEART);
											} else {
												item.tooExpensive = true;
												return;
											}
										} else {
											tooManyItems.too_many = true;
											return;
										}
										break;
									case TEXTURE_ASSET_ID::INV_HEART:
										item_to_use = TEXTURE_ASSET_ID::INV_HEART;
										location = std::find(vector.begin(), vector.end(), TEXTURE_ASSET_ID::INV_HEART);
										break;
									case TEXTURE_ASSET_ID::LIGHTENING_ITEM:
										if (g.playerInv[g.turn].size() < 9) {
											if (g.player_points[g.turn] >= 4) {
												g.player_points[g.turn] -= 4;
												g.playerInv[g.turn].push_back(TEXTURE_ASSET_ID::INV_LIGHTENING);
											} else {
												item.tooExpensive = true;
												return;
											}
										} else {
											tooManyItems.too_many = true;
											return;
										}
										break;
									case TEXTURE_ASSET_ID::INV_LIGHTENING:
										item_to_use = TEXTURE_ASSET_ID::INV_LIGHTENING;
										location = std::find(vector.begin(), vector.end(), TEXTURE_ASSET_ID::INV_LIGHTENING);
										break;
									case TEXTURE_ASSET_ID::POISON_ITEM:
										if (g.playerInv[g.turn].size() < 9) {
											if (g.player_points[g.turn] >= 3) {
												g.player_points[g.turn] -= 3;
												g.playerInv[g.turn].push_back(TEXTURE_ASSET_ID::INV_POISON);
											} else {
												item.tooExpensive = true;
												return;
											}
										} else {
											tooManyItems.too_many = true;
											return;
										}
										break;
									case TEXTURE_ASSET_ID::INV_POISON:
										item_to_use = TEXTURE_ASSET_ID::INV_POISON;
										location = std::find(vector.begin(), vector.end(), TEXTURE_ASSET_ID::INV_POISON);
										break;
									case TEXTURE_ASSET_ID::TRANSPORT_ITEM:
										if (g.playerInv[g.turn].size() < 9) {
											if (g.player_points[g.turn] >= 2) {
												g.player_points[g.turn] -= 2;
												g.playerInv[g.turn].push_back(TEXTURE_ASSET_ID::INV_TRANSPORT);
											} else {
												item.tooExpensive = true;
												return;
											}
										} else {
											tooManyItems.too_many = true;
											return;
										}
										break;
									case TEXTURE_ASSET_ID::INV_TRANSPORT:
										item_to_use= TEXTURE_ASSET_ID::INV_TRANSPORT;
										location = std::find(vector.begin(), vector.end(), TEXTURE_ASSET_ID::INV_TRANSPORT);
										break;
									case TEXTURE_ASSET_ID::TREASURE_1_ITEM:
										if (g.playerInv[g.turn].size() < 9) {
											if (g.player_points[g.turn] >= 2) {
												g.player_points[g.turn] -= 2;
												g.playerInv[g.turn].push_back(TEXTURE_ASSET_ID::INV_TREASURE_1);
											} else {
												item.tooExpensive = true;
												return;
											}
										} else {
											tooManyItems.too_many = true;
											return;
										}
										break;
									case TEXTURE_ASSET_ID::INV_TREASURE_1:
										item_to_use = TEXTURE_ASSET_ID::INV_TREASURE_1;
										location = std::find(vector.begin(), vector.end(), TEXTURE_ASSET_ID::INV_TREASURE_1);
										break;
									case TEXTURE_ASSET_ID::TREASURE_2_ITEM:
										if (g.playerInv[g.turn].size() < 9) {
											if (g.player_points[g.turn] >= 5) {
												g.player_points[g.turn] -= 5;
												g.playerInv[g.turn].push_back(TEXTURE_ASSET_ID::INV_TREASURE_2);
											} else {
												item.tooExpensive = true;
												return;
											}
										} else {
											tooManyItems.too_many = true;
											return;
										}
										break;
									case TEXTURE_ASSET_ID::INV_TREASURE_2:
										item_to_use = TEXTURE_ASSET_ID::INV_TREASURE_2;
										location = std::find(vector.begin(), vector.end(), TEXTURE_ASSET_ID::INV_TREASURE_2);
										break;
                                    case TEXTURE_ASSET_ID::HEAVY_DAMAGE_ITEM:
                                        if (g.playerInv[g.turn].size() < 9) {
                                            if (g.player_points[g.turn] >= 2) {
                                                g.player_points[g.turn] -= 2;
                                                g.playerInv[g.turn].push_back(TEXTURE_ASSET_ID::INV_HEAVY_DAMAGE);
                                            } else {
                                                item.tooExpensive = true;
                                                return;
                                            }
                                        } else {
                                            tooManyItems.too_many = true;
                                            return;
                                        }
                                        break;
                                    case TEXTURE_ASSET_ID::INV_HEAVY_DAMAGE:
                                        item_to_use = TEXTURE_ASSET_ID::INV_HEAVY_DAMAGE;
                                        location = std::find(vector.begin(), vector.end(), TEXTURE_ASSET_ID::INV_HEAVY_DAMAGE);
                                        break;
									case TEXTURE_ASSET_ID::HP_BUFF_ITEM:
										if (g.playerInv[g.turn].size() < 9) {
											if (g.player_points[g.turn] >= 2) {
												g.player_points[g.turn] -= 2;
												g.playerInv[g.turn].push_back(TEXTURE_ASSET_ID::INV_HP_BUFF);
											}
											else {
												item.tooExpensive = true;
												return;
											}
										}
										else {
											tooManyItems.too_many = true;
											return;
										}
										break;
									case TEXTURE_ASSET_ID::INV_HP_BUFF:
										item_to_use = TEXTURE_ASSET_ID::INV_HP_BUFF;
										location = std::find(vector.begin(), vector.end(), TEXTURE_ASSET_ID::INV_HP_BUFF);
										break;
                                    case TEXTURE_ASSET_ID::ENHANCED_CANNON_ITEM:
                                        if (g.playerInv[g.turn].size() < 9) {
                                            if (g.player_points[g.turn] >= 3) {
                                                g.player_points[g.turn] -= 3;
                                                g.playerInv[g.turn].push_back(TEXTURE_ASSET_ID::INV_ENHANCED_CANNON);
                                            } else {
                                                item.tooExpensive = true;
                                                return;
                                            }
                                        } else {
                                            tooManyItems.too_many = true;
                                            return;
                                        }
                                        break;
                                    case TEXTURE_ASSET_ID::INV_ENHANCED_CANNON:
                                        item_to_use = TEXTURE_ASSET_ID::INV_ENHANCED_CANNON;
                                        location = std::find(vector.begin(), vector.end(), TEXTURE_ASSET_ID::INV_ENHANCED_CANNON);
                                        break;
									case TEXTURE_ASSET_ID::TOO_MANY_ITEMS:
										return;
									default:
										break;
								}
								while (registry.items.entities.size() > 0) {
									registry.remove_all_components_of(registry.items.entities.back());
								}
								while (registry.shopBackground.entities.size() > 0) {
									registry.remove_all_components_of(registry.shopBackground.entities.back());
								}
								while (registry.notEnoughMoneyBanner.entities.size() > 0) {
									registry.remove_all_components_of(registry.notEnoughMoneyBanner.entities.back());
								}
								if (inventory_stage) {
									if (g.active_item[g.turn] == TEXTURE_ASSET_ID::BOARD_HELP) { // dummy to avoid getting multiple items
										g.active_item[g.turn] = item_to_use;
										if (location != vector.end()) {
											int inventory_index = location - vector.begin();
											g.playerInv[g.turn].erase(g.playerInv[g.turn].begin() + inventory_index);
											g.stage = 2;
										}
									}
									g.inv_active = false;
									g.inv_loaded = false;
									// g.stage = 0;
								} else {
									tooManyItems.active = false;
									tooManyItems.too_many = false;
									g.stage = 15;
								}
							}
						}
					}
					if(g.stage == 10 && !g.aiFlag[g.turn]){
						for (int i = 0; i < registry.dice.components.size(); i++) {
							GameDie& d = registry.dice.components.at(i);
							if (d.value == -1) { // only let the player pick the dice up once
								d.controlled = true;
								d.rolling = false;
								Mix_PlayChannel(-1, dice_pickup_sound, 0);
							}
						}
					}else if (g.stage == 11){
						g.stage += 1;
					}
				}
				else if (action == GLFW_RELEASE) {
				// don't let player ACTUALLY roll dice if in tutorial
					if(g.stage == 10 && !g.aiFlag[g.turn] && g.tutorialSeen){
						for (int i = 0; i < registry.dice.components.size(); i++) {
							GameDie& d = registry.dice.components.at(i);
							if (d.controlled) {
								Mix_PlayChannel(-1, this->dice_roll_sound, 0);
							}
							d.controlled = false;
							d.rolling = true;
						}
					}
				}
			}
		}
		if (drawingGame.active) {
			if (button == GLFW_MOUSE_BUTTON_1) {
				if (action == GLFW_PRESS || action == GLFW_REPEAT) {
					if (registry.mouseHold.entities.size() > 0) {
						MouseHold& mh = registry.mouseHold.components[0];
						mh.isMouseHolded = true;
					}
					Mix_PlayChannel(-1, paint_1_sound, 0);
				}
				if (action == GLFW_RELEASE) {
					if (registry.mouseHold.entities.size() > 0) {
						MouseHold& mh = registry.mouseHold.components[0];
						mh.isMouseHolded = false;
					}
				}
			}
		}
	}
}
