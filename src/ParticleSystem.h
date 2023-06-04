#pragma once

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <functional>
#include <optional>

#include <Scene.h>
#include <Object.h>


class ParticleSystem{
public:
	ParticleSystem() = default;

	void setEmitter(glm::vec3 emitter_in)
	{
		emitter = emitter_in;
	}

	void addParticle(const Model& m, glm::vec3 particleSize = {0.1, 0.1, 0.1})
	{
		//if(!currScene)
		//	return 0;
        //TODO(darius) make it so each particles is like reference to object, instread of being object on its own
		//Object* particle = currScene->createObject(std::forward<Args>(args)...);
		//particles.push_back(particle);
        particle = Model(m);
	}

    void addParticle(Mesh& m, Shader& shader_in, std::function<void(Transform)> shaderRoutine_in, glm::vec3 particleSize = {0.1, 0.1, 0.1})
    {
        //if(!currScene)
        //  return 0;
        //TODO(darius) make it so each particles is like reference to object, instread of being object on its own
        //Object* particle = currScene->createObject(std::forward<Args>(args)...);
        //particles.push_back(particle);
        particle = Model(m, shader_in, shaderRoutine_in);
    }

    void changeShader()
    {
        //change shader here
    }

    void addPosition(glm::vec3 pos)
    {
        positions.push_back(pos);
    }

    void renderParticles()
    {
        std::optional<PointLight> pl = std::nullopt;
        for(auto& p : positions)
        {
            particle.Draw(Transform(p, particle_size), pl);
        }
    }

	void updateUniform3DDistribution(float timeValue)
	{
        if (timeValue - lastTime > 1) {
            lastTime = timeValue;
            for (auto& position : positions)
            {
                int max = 1;
                int min = -1;
                float v1 = (rand() % (max - min)) + min;
                float v2 = (rand() % (max - min)) + min;
                float v3 = (rand() % (max - min)) + min;
                position = {v1,v2,v3};
            }
        }		
	}

	void updateSpawnFromEmitter(float timeValue)
	{
	    if (timeValue - lastTime > 1) {
            lastTime = timeValue;
            for (auto& p: positions)
            {
                int max = 1;
                int min = -1;
                int v1 = (rand() % (max - min)) + min;
                int v2 = (rand() % (max - 0));
                int v3 = (rand() % (max - min)) + min;

                //particle->get_pos_ref() = emitter;
                //particle->getRigidBody().reset();
                //particle->getRigidBody().add_force({v1/5,(v2 + 9.8)/5,v3/5});

                p = emitter;
            }
        }		
	}

private:
    std::vector<glm::vec3> positions;
    Model particle; 
    glm::vec3 particle_size = {0.1,0.1,0.1};
    glm::vec3 emitter = glm::vec3{0,0,0};
    float lastTime = 0;
};
