#include <ParticleSystem.h>
#include <Object.h>
#include <Renderer.h>

void ParticleSystem::setEmitter(glm::vec3 emitter_in)
{
	emitter = emitter_in;
}

void ParticleSystem::addParticle(FlatMesh&& m, Shader&& shader_in, LightingShaderRoutine&& shaderRoutine_in, glm::vec3 particleSize) noexcept
{
    shader = shader_in;
    shaderRoutine = shaderRoutine_in;
    particle.emplace(std::move(m));
    addPositions(amount);
    //particle.emplace(m, shader_in, shaderRoutine_in);
    glGenBuffers(1, &buffer);
    
    setBuffers();

    for(auto i : positions)
    {
        particles.push_back(Particle{i, 1, 5.0f}); 
    }
}

void ParticleSystem::changeShader()
{
    //change shader here
}

void ParticleSystem::addPosition(glm::vec3 pos)
{
    positions.push_back(glm::vec4{pos.x, pos.y, pos.z, 0});
}

void ParticleSystem::addPositions(size_t n)
{
    for(int i = 0; i < n; ++i)
        addPosition(glm::vec3{i,i,i});
}

//https://www.youtube.com/watch?v=Y0Ko0kvwfgA&ab_channel=Acerola
void ParticleSystem::renderParticles()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   
    Object tmpObj("tmp");

    auto comp = [](glm::vec4 a, glm::vec4 b){return a.z < b.z;};
    std::sort(positions.begin(), positions.end(), comp);

    {
        Shader sv = Renderer::shaderLibInstance->getCurrShader();
        glUseProgram(sv.getProgram());
        std::cout << particles[0].alpha << "\n";
        sv.setFloat("alpha", 1.0f - particles[0].alpha); 

        glm::vec3 pos = glm::vec3(0,0,0);//particlesTransfroms[0].getPosition();
        tmpObj.getTransform() = Transform(glm::vec3(pos.x, pos.y, pos.z), particle_size);
        shaderRoutine->operator()(&tmpObj);
        particle->Draw(*shader, positions.size());
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


            positions[i++] = glm::vec4{p.pos.x, p.pos.y, p.pos.z, p.alpha};
            positions[i - 1].z += i * 0.001f;
        }

        auto comp = [](glm::vec4 a, glm::vec4 b) {return a.z < b.z; };
        std::sort(positions.begin(), positions.end(), comp);
    }		
}

void ParticleSystem::update(float timeValue)
{
    if(pause)
        return;

    updateSpawnFromEmitter(timeValue);

    setBuffers();
}

void ParticleSystem::setBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * amount, &positions[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    particle->getVao().bind();

    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.

    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(1 * sizeof(glm::vec3)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(4, 1); // tell OpenGL this is an instanced vertex attribute.
}
