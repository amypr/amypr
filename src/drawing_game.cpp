#include "drawing_game.hpp"

void DrawingSystem::step(float elapsed_ms_since_last_update)
{
	DrawingGame& drawingGame = registry.drawingGame.get(board_controller);
	GameBoard& g = registry.gameBoard.get(board_controller);
	if (!drawingGame.loaded && drawingGame.active) {
		buildDrawing();
	}
	else if (drawingGame.loaded && !drawingGame.active) {
		deconstructDrawing();
		g.active = true;
		g.turn = 0;
		g.stage++;
	}
	if (drawingGame.active && drawingGame.loaded) {
		if (!registry.drawingTimer.has(board_controller)) {
			registry.drawingTimer.emplace(board_controller);
		}
		DrawingTimer& counter = registry.drawingTimer.components[0];
		counter.counter_ms -= elapsed_ms_since_last_update;
		if (counter.counter_ms > 0 && counter.counter_ms <= 14000 && !drawingGame.switched) {
			deconstructDrawing();
			drawingGame.switched = true;
			// buildDrawing();
		}

		if (counter.counter_ms < 0) {
			drawingGame.active = false;
			// drawingGame.switched = false;
			if (registry.drawingTimer.has(board_controller)) {
				registry.drawingTimer.remove(board_controller);
			}
			// g.active = true;
			// g.turn = 0;
			// g.stage++;
		}
		if (registry.light.entities.size() > 0) {
			Entity light_entity = registry.light.entities[0];
			Light& light = registry.light.get(light_entity);
			Motion& light_motion = registry.motions.get(light_entity);
			light.angle += 0.03;//0.08;
			light_motion.angle = sin(light.angle);
		}
	}
}

void DrawingSystem::init(RenderSystem* renderer, int window_width_px, int window_height_px) {
	this->window_width_px = window_width_px;
	this->window_height_px = window_height_px;
	this->renderer = renderer;
	registry.drawingGame.emplace(board_controller);
	registry.tooManyItems.emplace(board_controller);
	registry.coinInfo.emplace(board_controller);
}

void DrawingSystem::buildDrawing() {
	DrawingGame& drawingGame = registry.drawingGame.get(board_controller);

	// sync players from world to platformer (todo project: allow for character selection)

	srand((unsigned int)time(NULL));
	float randomNumber = rand()%5;
	if (randomNumber == 0) {
		randomDrawing = createDrawing(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::DRAWING_TRIANGLE);
	} else if (randomNumber == 1) {
		// TODO FINISH SQUARE STUFF
		randomDrawing = createDrawing(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::DRAWING_SQUARE);
	} else if (randomNumber == 2) {
		randomDrawing = createDrawing(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::DRAWING_HEART);
	} else if (randomNumber == 3) {
		randomDrawing = createDrawing(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::DRAWING_PENTAGON);
	} else {
		randomDrawing = createDrawing(renderer, vec2(window_width_px / 2, window_height_px / 2), TEXTURE_ASSET_ID::DRAWING_STAR);
	}
	registry.mouseHold.emplace(randomDrawing);
	Motion& drawing_motion = registry.motions.get(randomDrawing);
	if (drawingGame.switched) {
		createPlayerWriting(renderer, {drawing_motion.position.x, drawing_motion.position.y + drawing_motion.scale.y}, TEXTURE_ASSET_ID::PLAYER_2_WRITING);
	} else {
		createPlayerWriting(renderer, {drawing_motion.position.x, drawing_motion.position.y + drawing_motion.scale.y}, TEXTURE_ASSET_ID::PLAYER_1_WRITING);
	}
	createLamp(renderer);
	
	// Get background colour
	BackgroundColour &bgc = registry.backgroundColour.components[0];
	bgc.blue = 1;
	bgc.green = 1;
	bgc.red = 1;
	// bgc.blue = 0.1;
	// bgc.green = 0.1;
	// bgc.red = 0.1;
	// bgc.alpha = 0.8;

	drawingGame.loaded = true;
}

void DrawingSystem::deconstructDrawing() {
	DrawingGame& drawingGame = registry.drawingGame.get(board_controller);
	MouseHold& mh = registry.mouseHold.components[0];
	Drawing& drawing = registry.drawing.components[0];
	GameBoard& g = registry.gameBoard.get(board_controller);
	TEXTURE_ASSET_ID treasure_add = TEXTURE_ASSET_ID::INV_TREASURE_1;
	int num_points = ceil(mh.points.size() / 50);
	std::vector<vec2> sample_points;
	float average_distances = 0;
	vec2 point_avg_sample = {0.f, 0.f };
	vec2 point_avg_reference = {0.f, 0.f};
	// vec2 subtract;
	// for (int i = 0; i < 5; i++) {
	// 	if (i == 0) {
	// 		sample_points = drawingGame.triangle_points;
	// 	} else if (i == 1) {
	// 		sample_points = drawingGame.square_points;
	// 	} else if (i == 2) {
	// 		sample_points = drawingGame.heart_points;
	// 	} else if (i == 3) {
	// 		sample_points = drawingGame.pentagon_points;
	// 	} else {
	// 		sample_points = drawingGame.star_points;
	// 	}
	// 	printf("\n\n{");
	// 	subtract = sample_points[0];
	// 	for (int j = 0; j < 50; j++) {
	// 		sample_points[j] -= subtract;
	// 		printf("{%f, %f}, ", sample_points[j].x, sample_points[j].y);
	// 	}
	// }
	
	if (mh.points.size() < 50) {
		average_distances = window_width_px;
	} else {
		int i = 1;
		for (vec2 point: mh.points) {
			if (i%num_points == 0) {
				sample_points.push_back(point);
			}
			i++;
		}
		if (sample_points.size() > 50) {
			int size = sample_points.size();
			// printf("\nPoint set size: %d", size);
			for (int remove = 0; remove < size - 50; remove++) {
				srand((unsigned int)time(NULL));
				int randomNumber = rand()%49;
				sample_points.erase(sample_points.begin() + randomNumber);
				// printf("\nPoint set size: %d", sample_points.size());
			}
		}
		// printf("\nPoint set size: %d", sample_points.size());
		for (int i = 0; i < 50; i++) {
			point_avg_sample += sample_points[i];
			if (drawing.drawingType == TEXTURE_ASSET_ID::DRAWING_TRIANGLE) {
				point_avg_reference += drawingGame.triangle_points[i];
			} else if (drawing.drawingType == TEXTURE_ASSET_ID::DRAWING_SQUARE) {
				point_avg_reference += drawingGame.square_points[i];
			} else if (drawing.drawingType == TEXTURE_ASSET_ID::DRAWING_HEART) {
				point_avg_reference += drawingGame.heart_points[i];
			} else if (drawing.drawingType == TEXTURE_ASSET_ID::DRAWING_PENTAGON) {
				point_avg_reference += drawingGame.pentagon_points[i];
			} else {
				point_avg_reference += drawingGame.star_points[i];
			}
		}
		point_avg_reference /= 50;
		point_avg_sample /= 50;
		float total_distances = 0;
		float closest_distance = window_width_px;
		for (int i = 0; i < 50; i++) {
			closest_distance = window_width_px;
			sample_points[i] -= point_avg_sample;
			sample_points[i] -= point_avg_reference;
			for (int j = 0; j < 50; j++) {
				vec2 ref_point;
				if (drawing.drawingType == TEXTURE_ASSET_ID::DRAWING_TRIANGLE) {
					ref_point = drawingGame.triangle_points[j];
				} else if (drawing.drawingType == TEXTURE_ASSET_ID::DRAWING_SQUARE) {
					ref_point = drawingGame.square_points[j];
				} else if (drawing.drawingType == TEXTURE_ASSET_ID::DRAWING_HEART) {
					ref_point = drawingGame.heart_points[j];
				} else if (drawing.drawingType == TEXTURE_ASSET_ID::DRAWING_PENTAGON) {
					ref_point = drawingGame.pentagon_points[j];
				} else {
					ref_point = drawingGame.star_points[j];
				}
				float temp_distance = sqrt(pow(sample_points[i].x - ref_point.x, 2) + pow(sample_points[i].y - ref_point.y, 2));
				if (closest_distance > temp_distance) {
					closest_distance = temp_distance;
				}
			}
			total_distances += closest_distance;
		}
		average_distances += (total_distances / 50);
		total_distances = 0;
		for (int i = 0; i < 50; i++) {
			closest_distance = window_width_px;
			vec2 ref_point;
			if (drawing.drawingType == TEXTURE_ASSET_ID::DRAWING_TRIANGLE) {
				ref_point = drawingGame.triangle_points[i];
			} else if (drawing.drawingType == TEXTURE_ASSET_ID::DRAWING_SQUARE) {
				ref_point = drawingGame.square_points[i];
			} else if (drawing.drawingType == TEXTURE_ASSET_ID::DRAWING_HEART) {
				ref_point = drawingGame.heart_points[i];
			} else if (drawing.drawingType == TEXTURE_ASSET_ID::DRAWING_PENTAGON) {
				ref_point = drawingGame.pentagon_points[i];
			} else {
				ref_point = drawingGame.star_points[i];
			}
			for (int j = 0; j < 50; j++) {
				float temp_distance = sqrt(pow(sample_points[j].x - ref_point.x, 2) + pow(sample_points[j].y - ref_point.y, 2));
				if (closest_distance > temp_distance) {
					closest_distance = temp_distance;
				}
			}
			total_distances += closest_distance;
		}
		average_distances += (total_distances / 50);
		printf("\nPlayer average distance: %f", average_distances);
	}
	if (drawingGame.switched) {
		float player_3_score = (uniform_dist(rng) > 0.8) ? uniform_dist(rng) * 200: window_width_px;
		float player_4_score = (uniform_dist(rng) > 0.8) ? uniform_dist(rng) * 200: window_width_px;
		if (drawingGame.player_1_score == average_distances &&
			drawingGame.player_1_score == player_3_score &&
			drawingGame.player_1_score == player_4_score) {
			for (int k = 0; k < g.players; k++) {
				if (g.active_item[k] == treasure_add) {
					g.player_points[k] += 4;
				} else {
					g.player_points[k] += 2;
				}
			}
		} else if (drawingGame.player_1_score < average_distances &&
					drawingGame.player_1_score < player_3_score &&
					drawingGame.player_1_score < player_4_score) {
			if (g.active_item[0] == treasure_add) {
				g.player_points[0] += 10;
			} else {
				g.player_points[0] += 5;
			}
		} else if (average_distances < drawingGame.player_1_score &&
					average_distances < player_3_score &&
					average_distances < player_4_score) {
			if (g.active_item[1] == treasure_add) {
				g.player_points[1] += 10;
			} else {
				g.player_points[1] += 5;
			}
		} else if (player_3_score < drawingGame.player_1_score &&
					player_3_score < average_distances &&
					player_3_score < player_4_score) {
			if (g.active_item[2] == treasure_add) {
				g.player_points[2] += 10;
			} else {
				g.player_points[2] += 5;
			}
		} else {
			if (g.active_item[3] == treasure_add) {
				g.player_points[3] += 10;
			} else {
				g.player_points[3] += 5;
			}
		}
		drawingGame.switched = false;
	} else {
		drawingGame.player_1_score = average_distances;
	}

	for (int i = 0; i < g.players; i++) {
		if (g.active_item[i] == TEXTURE_ASSET_ID::INV_TREASURE_1) {
			g.active_item[i] = TEXTURE_ASSET_ID::BOARD_HELP; //dummy to avoid using any other items once done
		}
	}

	while (registry.drawing.entities.size() > 0) {
		registry.remove_all_components_of(registry.drawing.entities.back());
	}
	while (registry.drawingCharacterWriting.entities.size() > 0) {
		registry.remove_all_components_of(registry.drawingCharacterWriting.entities.back());
	}
	while (registry.mouseDraw.entities.size() > 0) {
		registry.remove_all_components_of(registry.mouseDraw.entities.back());
	}
	while (registry.mouseHold.entities.size() > 0) {
		registry.remove_all_components_of(registry.mouseHold.entities.back());
	}
	while (registry.light.entities.size() > 0) {
		registry.remove_all_components_of(registry.light.entities.back());
	}

	// Get background colour
	BackgroundColour &bgc = registry.backgroundColour.components[0];
	bgc.blue = 1;
	bgc.green = 0;
	bgc.red = 0;
	drawingGame.loaded = false;
}

Entity DrawingSystem::createDrawing(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID drawingNumber)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;

	motion.scale = mesh.original_size * 500.f;
	motion.scale.x *= 1.5f;

	// Set sprite attributes to character
	Drawing& drawing = registry.drawing.emplace(entity);
	drawing.drawingType = drawingNumber;
	registry.renderRequests.insert(
		entity,
		{ drawingNumber,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity DrawingSystem::createLamp(RenderSystem* renderer)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = {window_width_px / 2, 0.0f};

	motion.scale = {window_width_px / 13, window_height_px / 3};

	// Set sprite attributes to character
	Light& light = registry.light.emplace(entity);
	light.angle = 0.f;

	// light.lower_left = ((light.lower_left * mesh.original_size) / light.original_size);// + motion.position;
	// light.lower_right = ((light.lower_right * mesh.original_size) / light.original_size);// + motion.position;
	// light.upper_left = ((light.upper_left * mesh.original_size) / light.original_size);// + motion.position;
	// light.upper_right = ((light.upper_right * mesh.original_size) / light.original_size);// + motion.position;
	// light.light_pos = //(((light.light_pos - (light.original_size / 2.f)) * motion.scale) / light.original_size) + motion.position;
	// registry.renderRequests.insert(
	// 	entity,
	// 	{ TEXTURE_ASSET_ID::LAMP,
	// 		EFFECT_ASSET_ID::TEXTURED,
	// 		GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity DrawingSystem::createPlayerWriting(RenderSystem* renderer, vec2 position, TEXTURE_ASSET_ID playerNum)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;

	motion.scale = mesh.original_size * 140.f;
	motion.scale.x *= 1.5f;

	// Set sprite attributes to character
	registry.drawingCharacterWriting.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ playerNum,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

void DrawingSystem::drawLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::PEBBLE,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.mouseDraw.emplace(entity);
}
