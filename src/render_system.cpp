// internal
#include "render_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"

void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection)
{
	Motion &motion = registry.motions.get(entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform2D;
	Transform transform3D;
	transform2D.translate(motion.position);
	if (registry.temple.has(entity)) {
		transform3D.rotateX(registry.temple.get(entity).rotationX);
		transform3D.rotateY(registry.temple.get(entity).rotationY);
	}
	if (registry.keyframeAnimation.has(entity) && registry.keyframeAnimation.get(entity).type == TEXTURE_ASSET_ID::TEXTURE_COUNT) {
		transform3D.rotateX(registry.keyframeAnimation.get(entity).kfRotations[1]);
		transform3D.rotateY(registry.keyframeAnimation.get(entity).kfRotations[0]);
	}
	if (registry.coins.has(entity)) {
		transform3D.rotateX(registry.coins.get(entity).rotationX);
		transform3D.rotateY(registry.coins.get(entity).rotationY);
	}
	transform2D.rotate(motion.angle);
	if (registry.temple.has(entity)) {
		transform3D.scale3D(vec3(motion.scale, motion.scale.x));
	} else if (registry.coins.has(entity) || (registry.keyframeAnimation.has(entity) && registry.keyframeAnimation.get(entity).type == TEXTURE_ASSET_ID::TEXTURE_COUNT)) {
		transform3D.scale3D(vec3(motion.scale, motion.scale.x));
	} else {
		transform2D.scale(motion.scale);
	}
	// !!! TODO A1: add rotation to the chain of transformations, mind the order
	// of transformations

	assert(registry.renderRequests.has(entity));
	const RenderRequest &render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];
	// const GLuint nbo = normal_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::CHARACTER || render_request.used_effect == EFFECT_ASSET_ID::DIALOGUE || render_request.used_effect == EFFECT_ASSET_ID::TUTORIAL) {
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(CharacterVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(CharacterVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();


		// set frame of sprite
		GLint frame_uloc = glGetUniformLocation(program, "frame");
		assert(frame_uloc >= 0);


		const vec2 frame = registry.sprites.get(entity).frame;
		glUniform2f(frame_uloc, frame.x, frame.y);
		
		if (render_request.used_effect == EFFECT_ASSET_ID::CHARACTER) {
			// is character stunned?
			GLint stun_uloc = glGetUniformLocation(program, "stun");
			assert(stun_uloc >= 0);
			int isStunned = registry.stunTimers.has(entity) ? 1 : 0;
			glUniform1i(stun_uloc, isStunned);
			gl_has_errors();
			
			// is character poisoned?
			GLint poisoned_uloc = glGetUniformLocation(program, "poisoned");
			assert(poisoned_uloc >= 0);
			int isPoisoned = registry.poisoned.has(entity) ? 1 : 0;
			glUniform1i(poisoned_uloc, isPoisoned);
			gl_has_errors();
		}
		gl_has_errors();

	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::SALMON || render_request.used_effect == EFFECT_ASSET_ID::PEBBLE)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)sizeof(vec3));
		gl_has_errors();

        if (render_request.used_effect == EFFECT_ASSET_ID::SALMON)
        {
            // Light up?
            GLint light_up_uloc = glGetUniformLocation(program, "light_up");
            assert(light_up_uloc >= 0);

            // !!! TODO A1: set the light_up shader variable using glUniform1i,
            // similar to the glUniform1f call below. The 1f or 1i specified the type, here a single int.
            if (registry.lightUps.has(entity)) {
                glUniform1i(light_up_uloc, 1);
            } else {
                glUniform1i(light_up_uloc, 0);
            }
        }
	} else if (render_request.used_effect == EFFECT_ASSET_ID::GOLD) {
		// glBindBuffer(GL_ARRAY_BUFFER, nbo);
		gl_has_errors();
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		GLint in_normal_loc = glGetAttribLocation(program, "in_normal");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)sizeof(vec3));
		gl_has_errors();

		glEnableVertexAttribArray(in_normal_loc);
		glVertexAttribPointer(in_normal_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)(2 * sizeof(vec3)));
		gl_has_errors();
		
		GLuint time_uloc = glGetUniformLocation(program, "time");
		glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
		gl_has_errors();
	}else if (render_request.used_effect == EFFECT_ASSET_ID::DICE) {
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)sizeof(vec3));
		GLuint time_uloc = glGetUniformLocation(program, "time");
		GLuint startTime_uloc = glGetUniformLocation(program, "startTime");
		GLuint endTime_uloc = glGetUniformLocation(program, "endTime");
		glUniform1f(time_uloc, (float)(glfwGetTime() * 1000.0f));
		
		
		gl_has_errors();

		
        GLint boom_uloc = glGetUniformLocation(program, "boom");
        assert(boom_uloc >= 0);
		GameDie& dice = registry.dice.get(entity);
		int val = 0;
		if(dice.boom == true){
			val = dice.timer_ms <= 0 ? 2 : 1;
			if(val == 2 && dice.end_time < 0.0){
				dice.end_time = (float)(glfwGetTime() * 1000.0f);
			}
		}else {
			dice.start_time = (float)(glfwGetTime() * 1000.0f);
		}
		glUniform1f(endTime_uloc, dice.end_time);
		glUniform1f(startTime_uloc, dice.start_time);
        glUniform1i(boom_uloc, val);
	}else if(render_request.used_effect == EFFECT_ASSET_ID::PARTICLE){

		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();


		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(ColoredVertex), (void *)sizeof(vec3));
		gl_has_errors();


		// setup instance vars
		Particle_Manager& manager = registry.particles.get(entity);
		int len = manager.particles.size();
		
		GLuint instanceVBO;
		glGenBuffers(1,&instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER,instanceVBO);
		glBufferData(GL_ARRAY_BUFFER,sizeof(Particle) * len, &manager.particles[0],GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		gl_has_errors();

		// not using glGetAttribLocation :https://stackoverflow.com/questions/15639957/glgetattriblocation-returns-1-when-retrieving-existing-shader-attribute
		GLint in_offset_loc = 6;//glGetAttribLocation(program, "aOffset");
		GLint in_lifespan_loc = 7;//glGetAttribLocation(program, "lifespan");
		GLint in_velocity_loc = 8;//glGetAttribLocation(program, "velocity");
		gl_has_errors();
		
		glEnableVertexAttribArray(in_offset_loc);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glVertexAttribPointer(in_offset_loc, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
		// glBindBuffer(GL_ARRAY_BUFFER, 0);	
		glVertexAttribDivisor(in_offset_loc, 1);  

		gl_has_errors();

		glEnableVertexAttribArray(in_lifespan_loc);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glVertexAttribPointer(in_lifespan_loc, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)(sizeof(vec2)));
		// glBindBuffer(GL_ARRAY_BUFFER, 0);	
		glVertexAttribDivisor(in_lifespan_loc, 1);  

		gl_has_errors();

		glEnableVertexAttribArray(in_velocity_loc);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glVertexAttribPointer(in_velocity_loc, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)(2*sizeof(vec2)));
		// glBindBuffer(GL_ARRAY_BUFFER, 0);	
		glVertexAttribDivisor(in_velocity_loc, 1);  

		gl_has_errors();
        
	}else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform2D.mat);
	if (registry.temple.has(entity) || (registry.keyframeAnimation.has(entity) && registry.keyframeAnimation.get(entity).type == TEXTURE_ASSET_ID::TEXTURE_COUNT)
		|| registry.coins.has(entity)) {
		GLuint transform3D_loc = glGetUniformLocation(currProgram, "transform3D");
		glUniformMatrix3fv(transform3D_loc, 1, GL_FALSE, (float *)&transform3D.mat);
	}
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	if(render_request.used_effect == EFFECT_ASSET_ID::PARTICLE){
		// instancing here
		int arrsize = registry.particles.get(entity).particles.size();
		// glDrawArraysInstanced(GL_TRIANGLES,0,num_triangles,arrsize);
		glDrawElementsInstanced(GL_TRIANGLES,num_indices,GL_UNSIGNED_SHORT,nullptr,arrsize);
	}else{
		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	}
	gl_has_errors();
}

// draw the intermediate texture to the screen, with some distortion to simulate
// water
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the water texture, sprite mesh, and program
    
//    if (registry.tankGame.components[0].loaded && registry.tankGame.components[0].active) {
//        glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::WATER]);
//    }

    // TODO: change to black
    glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::MENU_BACKGROUND]);

	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint menu_program = effects[(GLuint)EFFECT_ASSET_ID::MENU_BACKGROUND];
	// Set clock
	GLuint time_uloc = glGetUniformLocation(menu_program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(menu_program, "darken_screen_factor");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	ScreenState &screen = registry.screenStates.get(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	gl_has_errors();

	GLuint light_uloc = glGetUniformLocation(menu_program, "is_light");
	if (registry.light.components.size() > 0 && registry.helpScreen.components.size() <= 0) {
		Motion& light_motion = registry.motions.get(registry.light.entities[0]);
		Light& light = registry.light.components[0];
		glUniform1f(light_uloc, 1);
		
		Transform transform2D;
		transform2D.translate(light_motion.position);
		transform2D.rotate(light_motion.angle);
		transform2D.scale({-light_motion.scale.x, light_motion.scale.y});
		GLuint transform_loc = glGetUniformLocation(menu_program, "transform");
		glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform2D.mat);
		
		// mat3 projection = createProjectionMatrix();
		// GLuint projection_loc = glGetUniformLocation(menu_program, "projection");
		// glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
		
		GLuint upper_left_uloc = glGetUniformLocation(menu_program, "upper_left");
		GLuint upper_right_uloc = glGetUniformLocation(menu_program, "upper_right");
		GLuint lower_left_uloc = glGetUniformLocation(menu_program, "lower_left");
		GLuint lower_right_uloc = glGetUniformLocation(menu_program, "lower_right");
		GLuint light_pos_uloc = glGetUniformLocation(menu_program, "light_pos");
		glUniform2fv(upper_left_uloc, 1, (float *)&light.upper_left);
		glUniform2fv(upper_right_uloc, 1, (float *)&light.upper_right);
		glUniform2fv(lower_left_uloc, 1, (float *)&light.lower_left);
		glUniform2fv(lower_right_uloc, 1, (float *)&light.lower_right);
		glUniform2fv(light_pos_uloc, 1, (float *)&light.light_pos);
	} else {
		glUniform1f(light_uloc, 0);
	}
	gl_has_errors();

	
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(menu_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();

	// Get background colour
	BackgroundColour &bgc = registry.backgroundColour.components[0];

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(bgc.red, bgc.green, bgc.blue, bgc.alpha);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();
	mat3 projection_2D = createProjectionMatrix();
	// Draw all textured meshes that have a position and size component
	for (Entity entity : registry.renderRequests.entities)
	{
		if (!registry.motions.has(entity))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize
		drawTexturedMesh(entity, projection_2D);
	}

	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

mat3 RenderSystem::createProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	gl_has_errors();

	Camera &camera = registry.camera.components[0];
	float right = (float)w / camera.screen_scale;
	float bottom = (float)h / camera.screen_scale;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left) + camera.x;
	float ty = -(top + bottom) / (top - bottom) + camera.y;
	camera.pos.x = tx;
	camera.pos.y = ty;
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}

void RenderSystem::step(float elapsed_ms){
	// update particles
	for(Particle_Manager& m : registry.particles.components){
		for(Particle& p : m.particles){
			p.lifespan_time[0] = p.lifespan_time[0] - elapsed_ms;
			if(p.lifespan_time[0] <= 0.0){
				p.lifespan_time[0] = p.lifespan_time[1]; // reset timer
			}
			
			

		}
	}
}