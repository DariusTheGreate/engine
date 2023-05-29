#pragma once

#include <vector>
#include <iostream>
#include <glm/glm.hpp>

#include <Scene.h>
#include <Object.h>


class ParticleSystem{
public:
	ParticleSystem() = default;
	ParticleSystem(Scene* currScene_in, int n) : currScene(currScene_in){
        particles.reserve(n);
	}

	void setEmitter(glm::vec3 emitter_in)
	{
		emitter = emitter_in;
	}

	template<typename... Args>
	Object* addParticle(Args&&... args)
	{
		if(!currScene)
			return 0;
		Object* particle = currScene->createObject(std::forward<Args>(args)...);
		particles.push_back(particle);
		return particle;
	}

	void updateUniform3DDistribution(float timeValue)
	{
        if (timeValue - lastTime > 1) {
            lastTime = timeValue;
            for (auto& particle : particles)
            {
                int max = 3;
                int min = -3;
                int v1 = (rand() % (max - min)) + min;
                int v2 = (rand() % (max - min)) + min;
                int v3 = (rand() % (max - min)) + min;
                particle->get_pos_ref() = glm::vec3{ v1,v2,v3 };
            }
        }		
	}

	void updateSpawnFromEmitter(float timeValue)
	{
	    if (timeValue - lastTime > 1) {
            lastTime = timeValue;
            for (auto& particle : particles)
            {
                int max = 1;
                int min = -1;
                int v1 = (rand() % (max - min)) + min;
                int v2 = (rand() % (max - 0));
                int v3 = (rand() % (max - min)) + min;

                particle->get_pos_ref() = emitter;
                particle->getRigidBody().reset();
                particle->getRigidBody().add_force({v1/5,(v2 + 9.8)/5,v3/5});
            }
        }		
	}

private:
    std::vector<Object*> particles;
    Scene* currScene = nullptr;
    glm::vec3 emitter = glm::vec3{0,0,0};
    float lastTime = 0;
};
