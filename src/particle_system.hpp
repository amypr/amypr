// #pragma once

// #include "common.hpp"
// #include "render_system.hpp"
// #include "weather.hpp"

// #include <random>

// // Particle system structure based on example done by youtuber The Cherno in https://www.youtube.com/watch?v=GK0jHlv3e3w

// class Random
// {
// public:
// 	static void Init()
// 	{
// 		s_RandomEngine.seed(std::random_device()());
// 	}

// 	static float Float()
// 	{
// 		return (float)s_Distribution(s_RandomEngine) / 100.f;
// 	}

// private:
// 	static std::mt19937 s_RandomEngine;
// 	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
// };

// class ParticleSystem
// {
// public:
// 	static ParticleSystem* GetInstance();

// 	void step(float elapsed_ms);

// 	void Emit(const ParticleProperties& particleProps);

// 	void grass_collision_listener(ECS_ENTT::Entity entity_i, ECS_ENTT::Entity entity_j, bool hit_wall);

// 	// void weather_listener(ECS_ENTT::Scene* scene);

// 	std::vector<Particle> GetParticles() const { return m_ParticlePool; }
// 	std::vector<Particle> GetActiveParticles() const;
// 	void clearParticles();

// 	ShadedMesh* GetParticleMesh() const { return m_ParticleMesh; }
// 	ShadedMesh* GetParticleMeshInstanced() const { return m_ParticleMeshInstanced; }

// private:
// 	ParticleSystem(uint32_t maxNumParticles);

// 	static ParticleSystem* instance;
// 	std::vector<Particle> m_ParticlePool;
// 	int32 m_PoolIndex;
// 	ShadedMesh* m_ParticleMesh;
// 	ShadedMesh* m_ParticleMeshInstanced;
// };