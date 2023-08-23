#pragma once

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <functional>
#include <optional>

#include <LightingShaderRoutine.h>
#include <FlatMesh.h>


class ParticleSystem{
public:
	ParticleSystem() = default;

	void setEmitter(glm::vec3 emitter_in);

    void addParticle(FlatMesh&& m, Shader&& shader_in, LightingShaderRoutine&& shaderRoutine_in, glm::vec3 particleSize = {0.1, 0.1, 0.1}) noexcept;

    void changeShader();

    void addPosition(glm::vec3 pos);

    void addPositions(size_t n);

    //https://www.youtube.com/watch?v=Y0Ko0kvwfgA&ab_channel=Acerola
    void renderParticles();
   
	void updateUniform3DDistribution(float timeValue);

	void updateSpawnFromEmitter(float timeValue);

    void update(float timeValue);

    std::vector<glm::vec3> positions;

    //NOTE(darius) DANGER(darius) for some reason when its uncomented u have explosion at deserealization in EngineLogic.dll
    std::vector<Transform> particlesTransfroms;
    std::vector<glm::mat4> modelMatrices;

	unsigned int buffer = 0;
	int amount = 100;

    std::optional<Mesh> particle; 
    std::optional<Shader> shader;
    std::optional<LightingShaderRoutine> shaderRoutine;

    glm::vec3 particle_size = {1,1,1};
    glm::vec3 emitter = glm::vec3{0,0,0};

    float maxBound = 1;
    float minBound = -1;

    float lastTime = 0;

    bool pause = false;
};

