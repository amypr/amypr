#include "tank_system.hpp"
#include "components.hpp"

void TankSystem::step(float elapsed_ms)
{
	TankGame& t = registry.tankGame.get(board_controller);
	GameBoard& g = registry.gameBoard.get(board_controller);
	if (!t.loaded && t.active) {
        buildTankGame();
	}
	else if (t.loaded && !t.active) {
        deconstructTankGame();
        g.active = true;
		g.turn = 0;
		g.stage++;
	}
    // this is empty for commit
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
}

void TankSystem::init(RenderSystem* renderer, int window_width_px, int window_height_px) {
	this->window_width_px = window_width_px;
	this->window_height_px = window_height_px;
	this->renderer = renderer;
	registry.tankGame.emplace(board_controller);
}

void TankSystem::buildTankGame() {
	TankGame& t = registry.tankGame.get(board_controller);
    GameBoard& g = registry.gameBoard.get(board_controller);
    BackgroundColour& bgc = registry.backgroundColour.components[0];
    bgc.blue = 0;
    bgc.green = 0;
    bgc.red = 0;
    vec2 scale = vec2(renderer->window_width_px/39.f, renderer->window_height_px/22.f);
    Entity player_tank1 = createPlayer1(renderer, { 34 * scale.x, 20 * scale.y });
    Entity player_tank2 = createPlayer2(renderer, { 3 * scale.x, 2 * scale.y });
    
    if (g.active_item[0] == TEXTURE_ASSET_ID::INV_HP_BUFF) {
        registry.playerTanks.get(player_tank1).health += 100;
    }
    if (g.active_item[1] == TEXTURE_ASSET_ID::INV_HP_BUFF) {
        registry.playerTanks.get(player_tank2).health += 100;
    }
    
    createPoints();
    createPoints();
    registry.colors.insert(player_tank1, {0.5f, 0.5f, 0.5f});
    registry.colors.insert(player_tank2, {0.5f, 0.5f, 0.5f});
	
	t.loaded = true;
}

void TankSystem::deconstructTankGame() {
	TankGame& t = registry.tankGame.get(board_controller);
    while (registry.playerTanks.entities.size() > 0) {
        registry.remove_all_components_of(registry.playerTanks.entities.back());
    }
    
    while (registry.walls.entities.size() > 0) {
        registry.remove_all_components_of(registry.walls.entities.back());
    }

	while (registry.aiTanks.entities.size() > 0) {
		registry.remove_all_components_of(registry.aiTanks.entities.back());
	}
		
	while (registry.playerBullets.entities.size() > 0) {
		registry.remove_all_components_of(registry.playerBullets.entities.back());
	}
    
    while (registry.aiBullets.entities.size() > 0) {
        registry.remove_all_components_of(registry.aiBullets.entities.back());
    }
    
    while (registry.tankings.entities.size() > 0) {
        registry.remove_all_components_of(registry.tankings.entities.back());
    }
    
    while (registry.tankGamePoints.entities.size() > 0) {
        registry.remove_all_components_of(registry.tankGamePoints.entities.back());
    }
    
    while (registry.hardWall.entities.size() > 0) {
        registry.remove_all_components_of(registry.hardWall.entities.back());
    }
    
    while (registry.tankHome.entities.size() > 0) {
        registry.remove_all_components_of(registry.tankHome.entities.back());
    }
    
    BackgroundColour& bgc = registry.backgroundColour.components[0];
    bgc.blue = 1;
    bgc.green = 0;
    bgc.red = 0;
	t.loaded = false;
    
}

Entity createPlayer1(RenderSystem* renderer, vec2 pos)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);
    vec2 scale = vec2(2*(renderer->window_width_px/39.0f), 2*(renderer->window_height_px/22.0f));
    
    Motion& motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = vec2(scale.x * 0.75f,scale.x * 0.5f);//mesh.original_size * 0.5f;
    motion.scale.x *= -1; // point front to the right

    registry.playerTanks.emplace(entity);
    registry.playerTanks.get(entity).id = 1;
    registry.ai_Info.emplace(entity);
    registry.physics.emplace(entity);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::NewTank, // TEXTURE_COUNT indicates that no txture is needed
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createPlayer2(RenderSystem* renderer, vec2 pos)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);
    vec2 scale = vec2(2*(renderer->window_width_px / 39.0f), 2*(renderer->window_height_px / 22.0f));
    
    Motion& motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = vec2(scale.x * 0.75f, scale.x * 0.5f);//mesh.original_size * 0.5f;
    motion.scale.x *= -1; // point front to the right

    registry.playerTanks.emplace(entity);
    registry.playerTanks.get(entity).id = 2;
    registry.ai_Info.emplace(entity);
    registry.physics.emplace(entity);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::NewTank2, // TEXTURE_COUNT indicates that no txture is needed
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createWall(RenderSystem* renderer, vec2 position)
{
    // Reserve en entity
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = position;
    vec2 scale = vec2(-(renderer->window_width_px / 39.0f), (renderer->window_height_px / 22.0f));
    motion.scale = scale;
    registry.walls.emplace(entity);
    registry.ai_Info.emplace(entity);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::WALL,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createBrokenWall(RenderSystem* renderer, vec2 position)
{
    // Reserve en entity
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);
    
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0, 0 };
    motion.position = position;
    vec2 scale = vec2(-(renderer->window_width_px / 39.0f), (renderer->window_height_px / 22.0f));
    motion.scale = scale;
    registry.walls.emplace(entity);
    registry.ai_Info.emplace(entity);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::TANK_BROKEN_WALL,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createHardWall(RenderSystem* renderer, vec2 position)
{
    // Reserve en entity
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { -(renderer->window_width_px / 39.0f), 0 };
    motion.position = position;
    vec2 scale = vec2(-(renderer->window_width_px / 39.0f), (renderer->window_height_px / 22.0f));
    motion.scale = scale;
    registry.hardWall.emplace(entity);
    registry.ai_Info.emplace(entity);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::TANK_WALL2,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createHome(RenderSystem* renderer, vec2 position)
{
    // Reserve en entity
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);
    
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    // motion.velocity = { -50, 0 };
    motion.position = position;
    vec2 scale = vec2(-3*(renderer->window_width_px / 39.0f), 3*(renderer->window_height_px / 22.0f));
    motion.scale = scale;
    registry.tankHome.emplace(entity);
    registry.ai_Info.emplace(entity);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::TANK_HOME,
            EFFECT_ASSET_ID::TEXTURED,
            GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createAITank(RenderSystem* renderer, vec2 position)
{
    auto entity = Entity();

    
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    
    vec2 scale = vec2(2*(renderer->window_width_px / 39.0f), 2*(renderer->window_height_px / 22.0f));
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.position = position;

    motion.scale = vec2(scale.x*-0.5f,scale.x*0.5f);

    registry.aiTanks.emplace(entity);
    registry.physics.emplace(entity);
    registry.ai_Info.emplace(entity);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::AITank,
         EFFECT_ASSET_ID::TEXTURED,
         GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createTanking(RenderSystem* renderer, vec2 position)
{
    auto entity = Entity();

    
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    
    vec2 scale = vec2(renderer->window_width_px/20.0f, renderer->window_height_px/31.0f);
    auto& motion = registry.motions.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.position = position;

    motion.scale = vec2(scale.x*-0.5f,scale.x*0.5f);

    registry.tankings.emplace(entity);
    registry.physics.emplace(entity);
    registry.colors.emplace(entity);
    registry.ai_Info.emplace(entity);
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::AITank,
         EFFECT_ASSET_ID::TEXTURED,
         GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createLine(vec2 position, vec2 scale)
{
    Entity entity = Entity();

    
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

    registry.debugComponents.emplace(entity);
    return entity;
}

Entity createPoints()
{
    auto entity = Entity();
    
    registry.tankGamePoints.emplace(entity);
    return entity;
}

Entity createAIBullet(vec2 pos, vec2 size)
{
    auto entity = Entity();

    
    Motion& motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = size;

    registry.aiBullets.emplace(entity);
    registry.physics.emplace(entity);
    
    float bullet_density = 20.5;
    registry.physics.get(entity).density = bullet_density;
    registry.physics.get(entity).mass = pow(size.x, 3) * M_PI * bullet_density;
    
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::TEXTURE_COUNT,
            EFFECT_ASSET_ID::PEBBLE,
            GEOMETRY_BUFFER_ID::PEBBLE });

    return entity;
}

Entity createPlayerBullet(vec2 pos, vec2 size)
{
    auto entity = Entity();

    
    Motion& motion = registry.motions.emplace(entity);
    motion.position = pos;
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = size;

    registry.playerBullets.emplace(entity);
    registry.physics.emplace(entity);
    registry.colors.emplace(entity);
    
    registry.colors.get(entity) = {0.5,0.5,0.5};

    float bullet_density = 10.5;
    registry.physics.get(entity).density = bullet_density;
    registry.physics.get(entity).mass = pow(size.x, 3) * M_PI * bullet_density;
    
    registry.renderRequests.insert(
        entity,
        { TEXTURE_ASSET_ID::TEXTURE_COUNT, // TEXTURE_COUNT indicates that no txture is needed
            EFFECT_ASSET_ID::PEBBLE,
            GEOMETRY_BUFFER_ID::PEBBLE });

    return entity;
}

Entity createTankGame(RenderSystem* renderer, vec2 position)
{
	// Reserve an entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.position = position;

	motion.scale = mesh.original_size * 80.f;

	registry.platforms.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PLATFORM,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}
