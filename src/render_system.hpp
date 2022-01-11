#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;
	

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SALMON, mesh_path("circle.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::BOARD_DIE, mesh_path("dice1.obj")),
          std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SALMON, mesh_path("tank.obj")),
          std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::TEMPLE, mesh_path("temple.obj")),
          std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::COIN, mesh_path("coin.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SAND, mesh_path("sand2.obj")),
		  std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::LEAF, mesh_path("leaf.obj"))
		  // specify meshes of other assets here
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("wall.png"),
			textures_path("tank.png"),
			textures_path("female 1 spritesheet.png"),
			textures_path("male 1 spritesheet.png"),
			textures_path("female 2 spritesheet.png"),
			textures_path("male 2 spritesheet.png"),
			textures_path("NeutralTile.png"),
			textures_path("MovementGainTile.png"),
			textures_path("MovementLossTile.png"),
			textures_path("RandomTile.png"),
			textures_path("ShopTile.png"),
			textures_path("TreasureGainTile.png"),
			textures_path("TreasureLossTile.png"),
			textures_path("Beach Background.png"),
			textures_path("Jungle background.png"),
			textures_path("platform.png"),
			textures_path("goal.png"),
			textures_path("drawing background.png"),
			textures_path("Beach_Background_0000_water.png"),
			textures_path("Beach_Background_0001_Close_Sand.png"),
			textures_path("Beach_Background_0002_trees.png"),
			textures_path("Beach_Background_0003_hills.png"),
			textures_path("Beach_Background_0004_sky.png"),
			textures_path("number_0.png"),
			textures_path("number_1.png"),
			textures_path("number_2.png"),
			textures_path("number_3.png"),
			textures_path("number_4.png"),
			textures_path("number_5.png"),
			textures_path("number_6.png"),
			textures_path("number_7.png"),
			textures_path("number_8.png"),
			textures_path("number_9.png"),
			textures_path("scoreboard_1.png"),
			textures_path("scoreboard_2.png"),
			textures_path("scoreboard_3.png"),
			textures_path("scoreboard_4.png"),
			textures_path("exit_button.png"),
			textures_path("help_button.png"),
			textures_path("save_button.png"),
			textures_path("new_game_button.png"),
			textures_path("load_button.png"),
			textures_path("logo.png"),
			textures_path("start_screen_background.png"),
			textures_path("player_1_writing.png"),
			textures_path("player_2_writing.png"),
			textures_path("drawing_heart.png"),
			textures_path("drawing_square.png"),
			textures_path("drawing_triangle.png"),
			textures_path("platformer_help.png"),
			textures_path("drawing_help.png"),
			textures_path("board_help.png"),
			textures_path("tank_help1.png"),
            textures_path("tank_help2.png"),
			textures_path("dice_item.png"),
			textures_path("heart_item.png"),
			textures_path("lightening_item.png"),
			textures_path("poison_item.png"),
			textures_path("transport_item.png"),
			textures_path("treasure_1_item.png"),
			textures_path("treasure_2_item.png"),
            textures_path("heavy_damage_item.png"),
            textures_path("enhanced_cannon_item.png"),
			textures_path("shop_background.png"),
			textures_path("inv_dice.png"),
			textures_path("inv_heart.png"),
			textures_path("inv_lightening.png"),
			textures_path("inv_poison.png"),
			textures_path("inv_transport.png"),
			textures_path("inv_treasure_1.png"),
			textures_path("inv_treasure_2.png"),
            textures_path("inv_heavy_damage.png"),
            textures_path("inv_enhanced_cannon.png"),
			textures_path("not_enough_treasure.png"),
			textures_path("intro spritesheet.png"),
			textures_path("ending spritesheet.png"),
			textures_path("too_many_items.png"),
			textures_path("toady spritesheet.png"),
			textures_path("trap.png"),
			textures_path("Jungle-background_0000_Foreground-floor_grass.png"),
			textures_path("Jungle-background_0001_big-grasses.png"),
			textures_path("Jungle-background_0002_upper-vine.png"),
			textures_path("Jungle-background_0003_Trees.png"),
			textures_path("Jungle-background_0004_vine.png"),
			textures_path("Jungle-background_0005_jungle-cannopy_sky.png"),
			textures_path("beach_button.png"),
			textures_path("jungle_button.png"),
			textures_path("beach_text.png"),
			textures_path("jungle_text.png"),
			textures_path("tank_player_1.jpg"),
			textures_path("tank_player_2.jpg"),
			textures_path("home.jpg"),
			textures_path("wall2.jpg"),
            textures_path("wall_broken.png"),
			textures_path("tutorial spritesheet.png"),
            textures_path("lightening.png"),
            textures_path("drawing_pentagon.png"),
            textures_path("drawing_star.png"),
			textures_path("HP_buff_item.jpg"),
			textures_path("inv_HP_buff.jpg"),
            textures_path("lamp.png")
	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("character"),
		shader_path("coloured"),
		shader_path("dialogue"),
		shader_path("dice"),
		shader_path("gold"),
		shader_path("menu_background"),
		shader_path("pebble"),
		shader_path("salmon"),
		shader_path("text"),
		shader_path("textured"),
		shader_path("water"),
		shader_path("sand"),
		shader_path("tutorial")
	};


	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<GLuint, geometry_count> normal_buffers;
	std::array<Mesh, geometry_count> meshes;

public:
	int window_width_px;
	int window_height_px;
	// Initialize the window
	bool init(int width, int height, GLFWwindow* window);
	void step(float elapsed_ms);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);
	// template <class T>
	// void bindVBOandIBOTemple(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices, std::vector<glm::vec3> normals);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the water
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	mat3 createProjectionMatrix();

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection);
	void drawToScreen();

	// Window handle
	GLFWwindow* window;
	float screen_scale;  // Screen to pixel coordinates scale factor (for apple
						 // retina display?)

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;
	Entity camera_state_entity;
	Entity background_colour_state_entity;
	// Entity scoreboard_state_entity;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path,const std::string& gs_path, GLuint& out_program, bool gs);
