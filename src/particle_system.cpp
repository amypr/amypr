// #include "particle_system.hpp"
// #include "render_system.hpp"
// #include "world_system.hpp"

// #include <glm/gtc/constants.hpp>
// #define GLM_ENABLE_EXPERIMENTAL
// #include <glm/gtx/compatibility.hpp>

// // Particle system structure based on example done by youtuber The Cherno in https://www.youtube.com/watch?v=GK0jHlv3e3w

// std::mt19937 Random::s_RandomEngine;
// std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution(1, 100);

// ParticleSystem* ParticleSystem::instance = nullptr;

// const float WIND_MAGNITUDE = 0.005;

// ParticleSystem::ParticleSystem(uint32_t maxNumParticles)
// 	: m_PoolIndex(maxNumParticles - 1), m_ParticleMesh(nullptr), m_ParticleMeshInstanced(nullptr))
// {
// 	Random::Init();
// 	m_ParticlePool.resize(maxNumParticles);
	
// 	std::string key = "particleSystemShadedMesh";
// 	ShadedMesh* particleMesh = &cache_resource(key);

// 	if (particleMesh->effect.program.resource == 0) {
// 		RenderSystem::createParticle(*particleMesh, "particle_shader");
// 	}

// 	std::string keyInstanced = "particleSystemShadedMeshInstanced";
// 	ShadedMesh* particleMeshInstanced = &cache_resource(keyInstanced);

// 	if (particleMeshInstanced->effect.program.resource == 0) {
// 		RenderSystem::createParticle(*particleMeshInstanced, "particle_shader_instanced");
// 	}

// 	// Store references to the potentially re-used mesh objects
// 	m_ParticleMesh = particleMesh;
// 	m_ParticleMeshInstanced = particleMeshInstanced;
// }

// ParticleSystem* ParticleSystem::GetInstance()
// {
// 	if (!instance)
// 		instance = new ParticleSystem(MAX_NUM_PARTICLES);
	
// 	return instance;
// }

// void ParticleSystem::step(float elapsed_ms) 
// {
// 	for (auto& particle : m_ParticlePool)
// 	{
// 		if (!particle.active)
// 			continue;

// 		if (particle.lifeRemaining <= 0.0f)
// 		{
// 			particle.active = false;
// 			particle.currentColour = glm::vec4(0.0f);
// 			continue;
// 		}

// 		particle.lifeRemaining -= elapsed_ms;
// 		particle.position += particle.velocity * elapsed_ms;
// 		particle.rotation += elapsed_ms/1000.0f;

// 		// interpolate particle colour and size between set parameters
// 		float lifeSpan = particle.lifeRemaining / particle.lifeTimeMs;
// 		particle.currentColour = glm::lerp(particle.colourEnd, particle.colourBegin, lifeSpan);
// 		particle.currentSize = glm::lerp(particle.sizeEnd, particle.sizeBegin, lifeSpan);

// 		if (particle.affectedByWind)
// 		{
// 			// larger particles get blown away slower, smaller particles get blown faster
// 			particle.velocity.x -= (1 / particle.currentSize) * WIND_MAGNITUDE;
// 		}
// 	}
// }

// void ParticleSystem::Emit(const ParticleProperties& particleProps)
// {
// 	Particle& particle = m_ParticlePool[m_PoolIndex];
// 	particle.active = true;
// 	particle.affectedByWind = particleProps.affectedByWind;
// 	particle.position = particleProps.position;
// 	particle.rotation = Random::Float() * 2.0f * PI;

// 	// Velocity
// 	particle.velocity = particleProps.velocity;
// 	particle.velocity.x += particleProps.velocityVariation.x * (Random::Float() - 0.5f);
// 	particle.velocity.y += particleProps.velocityVariation.y * (Random::Float() - 0.5f);
// 	particle.velocity.z += particleProps.velocityVariation.z * (Random::Float() - 0.5f);

// 	// Color
// 	particle.colourBegin = particleProps.colourBegin;
// 	particle.colourEnd = particleProps.colourEnd;
// 	particle.currentColour = particle.colourBegin;

// 	particle.lifeTimeMs = particleProps.lifeTimeMs;
// 	particle.lifeRemaining = particleProps.lifeTimeMs;
// 	particle.sizeBegin = particleProps.sizeBegin + particleProps.sizeVariation * (Random::Float() - 0.5f);
// 	particle.sizeEnd = particleProps.sizeEnd;
// 	particle.currentSize = particle.sizeBegin;

// 	m_PoolIndex--;
// 	if(m_PoolIndex < 0)
// 		m_PoolIndex = MAX_NUM_PARTICLES - 1;
// }

// // Collisions between between character and windy grass tiles - callback function, TODO should be listening to PhysicsSystem::characterLandsOnPlatform
// void ParticleSystem::grass_collision_listener(ECS_ENTT::Entity entity_i, ECS_ENTT::Entity entity_j, bool hit_wall)
// {
// 	if (!entity_i.IsValid() || !entity_j.IsValid()) {
// 		return;
// 	}

// 	if (registry.platforms.has(entity_j) && registry.sprites.has(entity_i)) {
// 		Motion& grassMotionComponent = registry.motions.get(entity_j);
// 		Motion& characterMotion = registry.motions.get(entity_i);

// 		if (glm::length(characterMotion.velocity) < 100.0f || grassMotionComponent.position.y >= characterMotion.position.y + 20.0f)
// 			return;

// 		// Emit grass particle going downward
// 		ParticleProperties particle;
// 		glm::vec3 particleOffset = glm::vec3(0.0f, 40.0f, 10.0f);
// 		particle.position = characterMotion.position + particleOffset;
// 		particle.velocity = glm::vec3(0.0f, 0.05f, 0.0f);
// 		particle.velocityVariation = glm::vec3(0.5f, 0.2f, 0.0f);
// 		particle.colourBegin = glm::vec4(0.0f, 1.0f, 0.2f, 0.8f);
// 		particle.colourEnd = glm::vec4(0.0f, 0.4f, 0.0f, 1.0f);
// 		particle.sizeBegin = 10.0f;
// 		particle.sizeEnd = 2.0f;
// 		particle.sizeVariation = 5.0f;
// 		particle.lifeTimeMs = 1000.0f;
// 		for (int i = 0; i < 1; i++)
// 			Emit(particle);

// 		// Emit grass particle with slight upward velocity
// 		particle.velocity = glm::vec3(0.0f, -0.025f, 0.0f);
// 		particle.velocityVariation = glm::vec3(0.1f, 0.05f, 0.0f);
// 		particle.colourBegin = glm::vec4(0.0f, 0.6f, 0.1f, 0.8f);
// 		particle.colourEnd = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
// 		particle.sizeBegin = 5.0f;
// 		particle.sizeEnd = 12.0f;
// 		particle.sizeVariation = 5.0f;
// 		particle.lifeTimeMs = 3000.0f;
// 		for (int i = 0; i < 2; i++)
// 			Emit(particle);

// 		// Emit dirt particles 
// 		particle.position -= glm::vec3(0.0f, 0.0f, 1.0f);
// 		particle.velocity = glm::vec3(0.0f, -0.1f, 0.025f);
// 		particle.velocityVariation = glm::vec3(0.5f, 0.1f, 0.0f);
// 		particle.colourBegin = glm::vec4(105.0f / 255.0f, 56.0f / 255.0f, 25.0f / 255.0f, 0.0f);
// 		particle.colourEnd = glm::vec4(105.0f / 255.0f, 56.0f / 255.0f, 25.0f / 255.0f, 1.0f);
// 		particle.sizeBegin = 4.0f;
// 		particle.sizeEnd = 1.0f;
// 		particle.sizeVariation = 1.0f;
// 		particle.lifeTimeMs = 200.0f;
// 		for (int i = 0; i < 1; i++)
// 			Emit(particle);
// 		particle.sizeBegin = 8.0f;
// 		particle.sizeEnd = 4.0f;
// 		for (int i = 0; i < 1; i++)
// 			Emit(particle);
// 	}

// }

// // void ParticleSystem::weather_listener(ECS_ENTT::Scene* scene)
// // {
// // 	ParticleProperties particle;

// // 	if (scene->m_Weather == WeatherTypes::Rain) {
// // 		// Emit rain particles
// // 		particle.position = {Random::Float() * (scene->m_Size.x + 100.f), .0, 0.0};
// // 		particle.velocity = glm::vec3(0.0f, 0.4f, 0.01f);
// // 		particle.velocityVariation = glm::vec3(0.0f, 0.2f, 0.0);
// // 		particle.colourBegin = glm::vec4(0.58f, 0.66f, 0.7f, 1.0f);
// // 		particle.colourEnd = glm::vec4(0.58f, 0.66f, 0.7f, 1.0f);
// // 		particle.sizeBegin = 7.0f;
// // 		particle.sizeEnd = 7.0f;
// // 		particle.sizeVariation = 1.0f;
// // 		particle.lifeTimeMs = scene->m_Size.y * 10.f;
// // 		particle.affectedByWind = true;

// // 		Emit(particle);
// // 	} else if (scene->m_Weather == WeatherTypes::Snow) {
// // 		// Emit snow particles
// // 		particle.position = {Random::Float() * (scene->m_Size.x + 100.f), .0, 0.0};
// // 		particle.velocity = glm::vec3(0.0f, 0.2f, 0.01f);
// // 		particle.velocityVariation = glm::vec3(0.0f, 0.2f, 0.0);
// // 		particle.colourBegin = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
// // 		particle.colourEnd = glm::vec4(0.9f, 0.9f, 0.92f, 0.8f);
// // 		particle.sizeBegin = 10.0f;
// // 		particle.sizeEnd = 10.0f;
// // 		particle.sizeVariation = 10.0f;
// // 		particle.lifeTimeMs = scene->m_Size.y * 10.f;
// // 		particle.affectedByWind = true;

// // 		Emit(particle);
// // 	}
// // }

// void ParticleSystem::clearParticles()
// {
// 	for (Particle& particle : m_ParticlePool)
// 	{
// 		particle.lifeRemaining = 0.0f;
// 		particle.active = false;
// 		particle.currentColour = glm::vec4(0.0f);
// 	}
// }

// std::vector<Particle> ParticleSystem::GetActiveParticles() const
// {
// 	std::vector<Particle> activeParticles;
// 	for (const Particle& particle : m_ParticlePool)
// 		if (particle.active)
// 			activeParticles.push_back(particle);
// 	return activeParticles;
// }
