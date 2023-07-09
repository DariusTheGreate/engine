#pragma once

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <functional>
#include <optional>

#include <Scene.h>
#include <Object.h>
#include <LightingShaderRoutine.h>
#include <FlatMesh.h>


class ParticleSystem{
public:
	ParticleSystem() = default;

	void setEmitter(glm::vec3 emitter_in)
	{
		emitter = emitter_in;
	}

    void addParticle(FlatMesh&& m, Shader&& shader_in, LightingShaderRoutine&& shaderRoutine_in, Material&& mat, glm::vec3 particleSize = {0.1, 0.1, 0.1})
    {
        shader = shader_in;
        shaderRoutine = shaderRoutine_in;
        particleMaterial = mat;
        particle = m;

        //particle.emplace(m, shader_in, shaderRoutine_in);
    }

    void changeShader()
    {
        //change shader here
    }

    void addPosition(glm::vec3 pos)
    {
        positions.push_back(pos);
    }

    void addPositions(size_t n)
    {
        for(int i = 0; i < n; ++i)
            addPosition(glm::vec3{i,i,i});
    }

    //https://www.youtube.com/watch?v=Y0Ko0kvwfgA&ab_channel=Acerola
    void renderParticles()
    {
        if(positions.size() < 3)
            return;

       /* unsigned int instanceVBO;
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), &positions[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0); 


        glEnableVertexAttribArray(7);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);   
        glVertexAttribDivisor(2, 1);  
        */

        //if(particleMaterial)
        //    particleMaterial->setShaderMaterial(*shader);

        for (auto& p : positions)
        {
            shaderRoutine->operator()(Transform(p, particle_size));
            //shader->setVec3("aOffset", glm::vec3{0,0,0});
            particle->Draw(*shader, positions.size());
        }

        /*std::optional<PointLight> pl = std::nullopt;//PointLight(glm::vec3{-0.2f, -1.0f, -0.3f}, glm::vec3(1,1,1));
        for(auto& p : positions)
        {
            particle->DrawRaw(*shader, 0);
        }
        */
    }

	void updateUniform3DDistribution(float timeValue)
	{
        if(pause)
            return;

        if (timeValue - lastTime > 1) {
            lastTime = timeValue;
            for (auto& position : positions)
            {
                int max = maxBound * 100;
                int min = minBound * 100;
                float v1 = (rand() % (max - min)) + min;
                float v2 = (rand() % (max - min)) + min;
                float v3 = (rand() % (max - min)) + min;
                position = {v1/100,v2/100,v3/100};
            }
        }		
	}

	void updateSpawnFromEmitter(float timeValue)
	{
        if(pause)
            return;

	    if (timeValue - lastTime > 1) {
            lastTime = timeValue;
            for (auto& p: positions)
            {
                int max = maxBound;
                int min = minBound;
                int v1 = (rand() % (max - min)) + min;
                int v2 = (rand() % (max - 0));
                int v3 = (rand() % (max - min)) + min;

                p = emitter;
            }
        }		
	}

    void update(float timeValue)
    {
        if(pause)
            return;
        updateUniform3DDistribution(timeValue);
    }

    std::vector<glm::vec3> positions;

    std::optional<FlatMesh> particle; 
    std::optional<Material> particleMaterial;
    std::optional<Shader> shader;
    std::optional<LightingShaderRoutine> shaderRoutine;


    glm::vec3 particle_size = {0.1,0.1,0.1};
    glm::vec3 emitter = glm::vec3{0,0,0};

    float maxBound = 1;
    float minBound = -1;

    float lastTime = 0;

    bool pause = false;
};

