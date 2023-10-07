#include <ParticleSystem.h>
#include <Object.h>
#include <Renderer.h>

void ParticleSystem::setEmitter(glm::vec3 emitter_in)
{
	emitter = emitter_in;
}

void ParticleSystem::addParticle(FlatMesh&& m, glm::vec3 particleSize) noexcept
{
    particle.emplace(std::move(m));
    particle->setDrawMode(DrawMode::DRAW_AS_INSTANCE);

    //addPositions(amount);

    for(int i = 0; i < amount; ++i)
        addPosition(glm::vec3{i,i,i});

    for(auto i : positions)
    {
        particles.emplace_back(Particle{i, 1, 5.0f}); 

        glm::mat4 modl = glm::mat4(1.0f);
        modl = glm::translate(modl, glm::vec3{ i.x, i.y, i.z });
        particlesRenderer.pushMat(modl);
    }

    particlesRenderer.setupMesh(&particle.value());
    particlesRenderer.init();
}

void ParticleSystem::addPosition(glm::vec3 pos)
{
    positions.push_back(glm::vec4{pos.x, pos.y, pos.z, 0});
}

void ParticleSystem::addPositions(size_t n)
{
    for(int i = 0; i < n; ++i)
        addPosition(glm::vec3{i,i,i});
    amount += n;

    for(auto i : positions)
    {
        particles.emplace_back(Particle{i, 1, 5.0f}); 
        particlesRenderer.pushMat(glm::mat4(1.0f));
    }
}

//https://www.youtube.com/watch?v=Y0Ko0kvwfgA&ab_channel=Acerola
void ParticleSystem::renderParticles()
{
    if (!particle)
        return;

    //TODO(darius) add blending stage
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   

    auto comp = [](glm::vec4 a, glm::vec4 b){return a.z < b.z;};
    std::sort(positions.begin(), positions.end(), comp);

    {
        Object tmpObj("tmp");
        Shader sv = Renderer::shaderLibInstance->getCurrShader();
        glUseProgram(sv.getProgram());

        Renderer::shaderLibInstance->shaderRoutine(&tmpObj);
        particle->Draw(sv, positions.size());
    }

    glDisable(GL_BLEND);
}

void ParticleSystem::updateUniform3DDistribution(float timeValue)
{
    if(pause)
        return;

    if (timeValue - lastTime > 1) {
        lastTime = timeValue;

        for (auto& position : positions)
        {
            int max = static_cast<int>(maxBound) * 100;
            int min = static_cast<int>(minBound) * 100;

            if (max == min)
                continue;

            float v1 = static_cast<float>((rand() % (max - min)) + min);
            float v2 = static_cast<float>((rand() % (max - min)) + min);
            float v3 = static_cast<float>((rand() % (max - min)) + min);

            position = {v1/100,v2/100,v3/100, 1};
        }
    }		
}

void ParticleSystem::updateSpawnFromEmitter(float timeValue)
{
    if(pause)
        return;

    if (timeValue - lastTime > 0.1) {
        lastTime = timeValue;

        int i = 0;
        for(auto& p : particles)
        {
            p.t++; 
            p.alpha = (p.t/p.tmax);
            if(p.t > p.tmax){
                p.alpha = 0.9f;

                float ptmax = static_cast<float>((rand() % (42 - 33)) + 33);
                p.tmax = ptmax;
                p.t = 0.0f;

                p.pos = emitter;

                int max = static_cast<int>(maxBound) * 100;
                int min = static_cast<int>(minBound) * 100; 

                float v1 = static_cast<float>((rand() % (max - min)) + min);
                float v2 = static_cast<float>((rand() % (max - min)) + min);
                float v3 = static_cast<float>((rand() % (max - min)) + min);

                p.force = {v1/1000, std::abs(1), 0};
            }

            /*glm::vec3 acceleration = p.force;
            acceleration *= p.mass;
            p.velocity = p.velocity + (timeValue - lastTime) * acceleration;
            p.velocity *= 0.98;

            p.pos = ((timeValue - lastTime) * p.velocity);
            */

            p.pos += p.force;
            p.force /= p.mass;
            p.force -= glm::vec3{0,0.0098,0};//glm::vec3{0, 980 * (timeValue - lastTime), 0};
            //p.force *= 0.98 * p.mass;


            if(i < positions.size()){
                positions[i++] = glm::vec4{p.pos.x, p.pos.y, p.pos.z, p.alpha};
                positions[i - 1].z += i * 0.001f;
            }
        }

        auto comp = [](glm::vec4 a, glm::vec4 b) {return a.z < b.z; };
        std::sort(positions.begin(), positions.end(), comp);

        for (int i = 0; i < positions.size(); ++i) 
        {
            particlesRenderer.getMats()[i][3][0] = positions[i].x;
            particlesRenderer.getMats()[i][3][1] = positions[i].y;
            particlesRenderer.getMats()[i][3][2] = positions[i].z;
        }
    }		
}

void ParticleSystem::update(float timeValue)
{
    if(pause)
        return;

    updateSpawnFromEmitter(timeValue);

    particlesRenderer.setupBuffer();
}
