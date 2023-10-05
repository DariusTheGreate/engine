#pragma once

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <functional>
#include <optional>

#include <FlatMesh.h>
#include <InstancedRenderer.h>

struct Particle
{
    glm::vec3 pos = {0,0,0};
    float alpha = 1;
    float t = 0.0f;
    float tmax = 5.0f;
    float mass = 2.0f;
    glm::vec3 force = {0,0,0};
    glm::vec3 velocity = {0,0,0};
};

class ParticleSystem{
public:
	ParticleSystem() = default;
    ParticleSystem(int amount) : amount(amount) {};

	void setEmitter(glm::vec3 emitter_in);

    void addParticle(FlatMesh&& m, glm::vec3 particleSize = {0.1, 0.1, 0.1}) noexcept;

    void changeShader();

    void addPosition(glm::vec3 pos);

    void addPositions(size_t n);

    //https://www.youtube.com/watch?v=Y0Ko0kvwfgA&ab_channel=Acerola
    void renderParticles();
   
	void updateUniform3DDistribution(float timeValue);

	void updateSpawnFromEmitter(float timeValue);

    void update(float timeValue);

    void setBuffers();

    std::vector<glm::vec4> positions;
    std::vector<glm::mat4> instanceMats;
    std::vector<Particle> particles;

	unsigned int buffer = 0;
    bool buffCreated = false;

	int amount = 21;

    std::optional<Mesh> particle; 
    std::optional<Shader> shader;

    glm::vec3 particle_size = {1,1,1};
    glm::vec3 emitter = glm::vec3{0,0,0};

    float maxBound = 1;
    float minBound = -1;

    float lastTime = 0;

    bool pause = false;

    InstancedRenderer particlesRenderer;
};
