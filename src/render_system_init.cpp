// internal
#include "render_system.hpp"

#include <array>
#include <fstream>

#include "../ext/stb_image/stb_image.h"

// This creates circular header inclusion, that is quite bad.
#include "tiny_ecs_registry.hpp"

// stlib
#include <iostream>
#include <sstream>
#include <unordered_set>

// World initialization
bool RenderSystem::init(int width, int height, GLFWwindow* window_arg)
{
	this->window = window_arg;
	this->window_height_px = height;
	this->window_width_px = width;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function pointers
	const int is_fine = gl3w_init();
	assert(is_fine == 0);

	// Create a frame buffer
	frame_buffer = 0;
	glGenFramebuffers(1, &frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();

	// For some high DPI displays (ex. Retina Display on Macbooks)
	// https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value
	int fb_width, fb_height;
	glfwGetFramebufferSize(window, &fb_width, &fb_height);
	registry.camera.emplace(camera_state_entity);
	Camera &camera = registry.camera.components[0];
	camera.camera_fb_width = fb_width;
	camera.camera_width = width;
	camera.screen_scale = static_cast<float>(fb_width) / width;
	(int)height; // dummy to avoid warning

	// Background colour
	registry.backgroundColour.emplace(background_colour_state_entity);
	BackgroundColour &bgc = registry.backgroundColour.components[0];
	bgc.red = 0;
	bgc.green = 0;
	bgc.blue = 1.0;

	// ASK(Camilo): Setup error callback. This can not be done in mac os, so do not enable
	// it unless you are on Linux or Windows. You will need to change the window creation
	// code to use OpenGL 4.3 (not suported on mac) and add additional .h and .cpp
	// glDebugMessageCallback((GLDEBUGPROC)errorCallback, nullptr);

	// We are not really using VAO's but without at least one bound we will crash in
	// some systems.
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	gl_has_errors();

	initScreenTexture();
    initializeGlTextures();
	initializeGlEffects();
	initializeGlGeometryBuffers();

	return true;
}

void RenderSystem::initializeGlTextures()
{
    glGenTextures((GLsizei)texture_gl_handles.size(), texture_gl_handles.data());

    for(uint i = 0; i < texture_paths.size(); i++)
    {
        const std::string& path = texture_paths[i];
        ivec2& dimensions = texture_dimensions[i];

        stbi_uc* data;
        data  = stbi_load(path.c_str(), &dimensions.x, &dimensions.y, NULL, 4);

        if (data == NULL)
        {
			const std::string message = "Could not load the file " + path + ".";
            fprintf(stderr, "%s", message.c_str());
            assert(false);
        }
        glBindTexture(GL_TEXTURE_2D, texture_gl_handles[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.x, dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		gl_has_errors();
        stbi_image_free(data);
    }
	gl_has_errors();
}

void RenderSystem::initializeGlEffects()
{
	for(uint i = 0; i < effect_paths.size(); i++)
	{
		const std::string vertex_shader_name = effect_paths[i] + ".vs.glsl";
		const std::string fragment_shader_name = effect_paths[i] + ".fs.glsl";
		const std::string geometry_shader_name = effect_paths[i] + ".gs.glsl";
		bool geometry_shader = !(gs_shader_support.find(i) == gs_shader_support.end());
		bool is_valid = loadEffectFromFile(vertex_shader_name, fragment_shader_name,geometry_shader_name, effects[i],geometry_shader);
		assert(is_valid && (GLuint)effects[i] != 0);
	}
}

// // One could merge the following two functions as a template function...
// template <class T>
// void RenderSystem::bindVBOandIBOTemple(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices, std::vector<glm::vec3> normals)
// {
// 	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(uint)gid]);
// 	glBufferData(GL_ARRAY_BUFFER,
// 		sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
// 	gl_has_errors();

// 	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers[(uint)gid]);
// 	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
// 		sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
// 	gl_has_errors();
	
// 	glBindBuffer(GL_ARRAY_BUFFER, normal_buffers[(uint)gid]);
// 	glBufferData(GL_ARRAY_BUFFER,
// 		sizeof(normals[0]) * normals.size(), normals.data(), GL_STATIC_DRAW);
// 	gl_has_errors();
// }

template <class T>
void RenderSystem::bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(uint)gid]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	gl_has_errors();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers[(uint)gid]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
	gl_has_errors();
}

void RenderSystem::initializeGlMeshes()
{
	for (uint i = 0; i < mesh_paths.size(); i++)
	{
		// Initialize meshes
		GEOMETRY_BUFFER_ID geom_index = mesh_paths[i].first;
		std::string name = mesh_paths[i].second;
		Mesh::loadFromOBJFile(name, 
			meshes[(int)geom_index].vertices,
			meshes[(int)geom_index].vertex_indices,
			meshes[(int)geom_index].original_size,
			meshes[(int)geom_index].uvs,
			meshes[(int)geom_index].normals,
			meshes[(int)geom_index].normal_indices);

		// bindVBOandIBOTemple(geom_index,
		// 	meshes[(int)geom_index].vertices, 
		// 	meshes[(int)geom_index].vertex_indices,
		// 	meshes[(int)geom_index].normals);
		bindVBOandIBO(geom_index,
			meshes[(int)geom_index].vertices, 
			meshes[(int)geom_index].vertex_indices);
	}
}

void RenderSystem::initializeGlGeometryBuffers()
{
	// Vertex Buffer creation.
	glGenBuffers((GLsizei)vertex_buffers.size(), vertex_buffers.data());
	// Index Buffer creation.
	glGenBuffers((GLsizei)index_buffers.size(), index_buffers.data());
	// Normal Buffer creation.
	glGenBuffers((GLsizei)normal_buffers.size(), normal_buffers.data());

	// Index and Vertex buffer data initialization.
	initializeGlMeshes();

	//////////////////////////
	// Initialize sprite
	// The position corresponds to the center of the texture.
	std::vector<TexturedVertex> textured_vertices(4);
	textured_vertices[0].position = { -1.f/2, +1.f/2, 0.f };
	textured_vertices[1].position = { +1.f/2, +1.f/2, 0.f };
	textured_vertices[2].position = { +1.f/2, -1.f/2, 0.f };
	textured_vertices[3].position = { -1.f/2, -1.f/2, 0.f };
	textured_vertices[0].texcoord = { 0.f, 1.f };
	textured_vertices[1].texcoord = { 1.f, 1.f };
	textured_vertices[2].texcoord = { 1.f, 0.f };
	textured_vertices[3].texcoord = { 0.f, 0.f };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::SPRITE, textured_vertices, textured_indices);


	/////////////////////////
	// Initialize character
	// todo project
	std::vector<CharacterVertex> character_vertices(4);
	character_vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
	character_vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
	character_vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
	character_vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };
	character_vertices[0].texcoord = { 0.f, 0.5f };
	character_vertices[1].texcoord = { 0.2f, 0.5f };
	character_vertices[2].texcoord = { 0.2f, 0.f };
	character_vertices[3].texcoord = { 0.f, 0.f };

	const std::vector<uint16_t> character_indices = {  0, 3, 1, 1, 3, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::CHARACTER, character_vertices, character_indices);

	/////////////////////////
	// Initialize dialogue box
	std::vector<DialogueVertex> dialogue_vertices(4);
	dialogue_vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
	dialogue_vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
	dialogue_vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
	dialogue_vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };
	dialogue_vertices[0].texcoord = { 0.f, 1.f };
	dialogue_vertices[1].texcoord = { 0.2f, 1.f };
	dialogue_vertices[2].texcoord = { 0.2f, 0.f };
	dialogue_vertices[3].texcoord = { 0.f, 0.f };

	const std::vector<uint16_t> dialogue_indices = { 0, 3, 1, 1, 3, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::DIALOGUE, dialogue_vertices, dialogue_indices);

	/////////////////////////
	// Initialize tutorial dialogue
	std::vector<DialogueVertex> tutorial_vertices(4);
	tutorial_vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
	tutorial_vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
	tutorial_vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
	tutorial_vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };
	tutorial_vertices[0].texcoord = { 0.f, 1.f };
	tutorial_vertices[1].texcoord = { 0.125f, 1.f };
	tutorial_vertices[2].texcoord = { 0.125f, 0.f };
	tutorial_vertices[3].texcoord = { 0.f, 0.f };

	const std::vector<uint16_t> tutorial_indices = { 0, 3, 1, 1, 3, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::TUTORIAL, tutorial_vertices, tutorial_indices);

	////////////////////////
	// Initialize pebble
	std::vector<ColoredVertex> pebble_vertices;
	std::vector<uint16_t> pebble_indices;
	constexpr float z = -0.1f;
	constexpr int NUM_TRIANGLES = 62;

	for (int i = 0; i < NUM_TRIANGLES; i++) {
		const float t = float(i) * M_PI * 2.f / float(NUM_TRIANGLES - 1);
		pebble_vertices.push_back({});
		pebble_vertices.back().position = { 0.5 * cos(t), 0.5 * sin(t), z };
		pebble_vertices.back().color = { 0.8, 0.8, 0.8 };
	}
	pebble_vertices.push_back({});
	pebble_vertices.back().position = { 0, 0, 0 };
	pebble_vertices.back().color = { 0.8, 0.8, 0.8 };
	for (int i = 0; i < NUM_TRIANGLES; i++) {
		pebble_indices.push_back((uint16_t)i);
		pebble_indices.push_back((uint16_t)((i + 1) % NUM_TRIANGLES));
		pebble_indices.push_back((uint16_t)NUM_TRIANGLES);
	}
	int geom_index = (int)GEOMETRY_BUFFER_ID::PEBBLE;
	meshes[geom_index].vertices = pebble_vertices;
	meshes[geom_index].vertex_indices = pebble_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::PEBBLE, meshes[geom_index].vertices, meshes[geom_index].vertex_indices);

	//////////////////////////////////
	// Initialize debug line
	std::vector<ColoredVertex> line_vertices;
	std::vector<uint16_t> line_indices;

	constexpr float depth = 0.5f;
	constexpr vec3 red = { 0.8,0.1,0.1 };

	// Corner points
	line_vertices = {
		{{-0.5,-0.5, depth}, red},
		{{-0.5, 0.5, depth}, red},
		{{ 0.5, 0.5, depth}, red},
		{{ 0.5,-0.5, depth}, red},
	};

	// Two triangles
	line_indices = {0, 1, 3, 1, 2, 3};
	
	geom_index = (int)GEOMETRY_BUFFER_ID::DEBUG_LINE;
	meshes[geom_index].vertices = line_vertices;
	meshes[geom_index].vertex_indices = line_indices;
	bindVBOandIBO(GEOMETRY_BUFFER_ID::DEBUG_LINE, line_vertices, line_indices);

	///////////////////////////////////////////////////////
	// Initialize screen triangle (yes, triangle, not quad; its more efficient).
	std::vector<vec3> screen_vertices(3);
	screen_vertices[0] = { -1, -6, 0.f };
	screen_vertices[1] = { 6, -1, 0.f };
	screen_vertices[2] = { -1, 6, 0.f };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> screen_indices = { 0, 1, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE, screen_vertices, screen_indices);
}

RenderSystem::~RenderSystem()
{
	// Don't need to free gl resources since they last for as long as the program,
	// but it's polite to clean after yourself.
	glDeleteBuffers((GLsizei)vertex_buffers.size(), vertex_buffers.data());
	glDeleteBuffers((GLsizei)index_buffers.size(), index_buffers.data());
	glDeleteTextures((GLsizei)texture_gl_handles.size(), texture_gl_handles.data());
	glDeleteTextures(1, &off_screen_render_buffer_color);
	glDeleteRenderbuffers(1, &off_screen_render_buffer_depth);
	gl_has_errors();

	for(uint i = 0; i < effect_count; i++) {
		glDeleteProgram(effects[i]);
	}
	// delete allocated resources
	glDeleteFramebuffers(1, &frame_buffer);
	gl_has_errors();

	// remove all entities created by the render system
	while (registry.renderRequests.entities.size() > 0)
	    registry.remove_all_components_of(registry.renderRequests.entities.back());
}

// Initialize the screen texture from a standard sprite
bool RenderSystem::initScreenTexture()
{
	registry.screenStates.emplace(screen_state_entity);

	int width, height;
	glfwGetFramebufferSize(const_cast<GLFWwindow*>(window), &width, &height);

	glGenTextures(1, &off_screen_render_buffer_color);
	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl_has_errors();

	glGenRenderbuffers(1, &off_screen_render_buffer_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, off_screen_render_buffer_depth);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, off_screen_render_buffer_color, 0);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, off_screen_render_buffer_depth);
	gl_has_errors();

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	return true;
}

bool gl_compile_shader(GLuint shader)
{
	glCompileShader(shader);
	gl_has_errors();
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint log_len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
		std::vector<char> log(log_len);
		glGetShaderInfoLog(shader, log_len, &log_len, log.data());
		glDeleteShader(shader);

		gl_has_errors();

		fprintf(stderr, "GLSL: %s", log.data());
		return false;
	}

	return true;
}

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path,const std::string& gs_path, GLuint& out_program, bool gs)
{
	// Opening files

	std::ifstream vs_is(vs_path);
	std::ifstream fs_is(fs_path);
	std::ifstream gs_is(gs_path);
	if (!vs_is.good() || !fs_is.good() || (gs && !gs_is.good()))
	{
		if(gs){
			fprintf(stderr, "Failed to load shader files %s, %s, %s", vs_path.c_str(), fs_path.c_str(),gs_path.c_str() );
		}else{
			fprintf(stderr, "Failed to load shader files %s, %s", vs_path.c_str(), fs_path.c_str());
		}
		assert(false);
		return false;
	}

	// Reading sources
	std::stringstream vs_ss, fs_ss, gs_ss;
	vs_ss << vs_is.rdbuf();
	fs_ss << fs_is.rdbuf();
	std::string vs_str = vs_ss.str();
	std::string fs_str = fs_ss.str();
	std::string gs_str;
	const char* vs_src = vs_str.c_str();
	const char* fs_src = fs_str.c_str();
	GLsizei vs_len = (GLsizei)vs_str.size();
	GLsizei fs_len = (GLsizei)fs_str.size();

	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vs_src, &vs_len);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fs_src, &fs_len);

	GLuint geometry;
	if(gs){
		gs_ss << gs_is.rdbuf();
		gs_str = gs_ss.str();
		const char* gs_src = gs_str.c_str();
		GLsizei gs_len = (GLsizei)gs_str.size();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gs_src, &gs_len);
	}
	gl_has_errors();

	// Compiling
	if (!gl_compile_shader(vertex))
	{
		fprintf(stderr, "Vertex compilation failed");
		assert(false);
		return false;
	}
	if (!gl_compile_shader(fragment))
	{
		fprintf(stderr, "Vertex compilation failed");
		assert(false);
		return false;
	}
	if(gs){
		if(!gl_compile_shader(geometry)){
			fprintf(stderr, "Geometry compilation failed, did you mean to use one?");
			assert(false);
			return false;
		}
	}
	

	// Linking
	out_program = glCreateProgram();
	glAttachShader(out_program, vertex);
	glAttachShader(out_program, fragment);
	if(gs){
		glAttachShader(out_program,geometry);
	}
	glLinkProgram(out_program);
	gl_has_errors();


	{
		GLint is_linked = GL_FALSE;
		glGetProgramiv(out_program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint log_len;
			glGetProgramiv(out_program, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetProgramInfoLog(out_program, log_len, &log_len, log.data());
			gl_has_errors();

			fprintf(stderr, "Link error: %s", log.data());
			assert(false);
			return false;
		}
	}

	glValidateProgram(out_program);

	// No need to carry this around. Keeping these objects is only useful if we recycle
	// the same shaders over and over, which we don't, so no need and this is simpler.
	glDetachShader(out_program, vertex);
	glDetachShader(out_program, fragment);
	if(gs){
		glDetachShader(out_program, geometry);
		glDeleteShader(geometry);
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	gl_has_errors();

	return true;
}

