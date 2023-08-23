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
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * amount, &positions[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    particle->getVao().bind();

    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.
}

void ParticleSystem::changeShader()
{
    //change shader here
}

void ParticleSystem::addPosition(glm::vec3 pos)
{
    positions.push_back(pos);
}

void ParticleSystem::addPositions(size_t n)
{
    for(int i = 0; i < n; ++i)
        addPosition(glm::vec3{i,i,i});
}

//https://www.youtube.com/watch?v=Y0Ko0kvwfgA&ab_channel=Acerola
void ParticleSystem::renderParticles()
{
    //if(positions.size() < 3)
     //   return;

    //if(particleMaterial)
    //    particleMaterial->setShaderMaterial(*shader);

    /*for (auto& p : positions)
    {
        shaderRoutine->operator()(Transform(p, particle_size));
        //shader->setVec3("aOffset", glm::vec3{0,0,0});
        //particle->Draw(*shader, positions.size());
    }
    */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   
    Object tmpObj("tmp");

    auto comp = [](glm::vec3 a, glm::vec3 b){return a.z < b.z;};
    std::sort(positions.begin(), positions.end(), comp);

    /*for(int i = 0; i < particlesTransfroms.size(); ++i)
    {
        if(positions.size() > i){
            particlesTransfroms[i].setPosition(positions[i]);
        }
    }

    /*int c = 0;
    for(unsigned int i = 0; i < 100; i++)
    {
        Renderer::shaderLibInstance->getCurrShader().setVec3(("offsets[" + std::to_string(i) + "]"), particlesTransfroms[i].getPosition());
    }  
    */

    //for (auto& m : particlesTransfroms)
    {
        //if(c++ > positions.size()-1)
        //    break;
        //tmpObj.getTransform().setMatrix(m);
        glm::vec3 pos = glm::vec3(0,0,0);//particlesTransfroms[0].getPosition();
        tmpObj.getTransform() = Transform(glm::vec3(pos.x, pos.y, pos.z), particle_size);
        shaderRoutine->operator()(&tmpObj);
        particle->Draw(*shader, 100);
    }

    /*tmpObj.getTransform() = Transform(glm::vec3{0,0,0}, particle_size);
    shaderRoutine->operator()(&tmpObj);
	particle->Draw(*shader);

    tmpObj.getTransform() = Transform(glm::vec3{1,1,1}, particle_size);
    shaderRoutine->operator()(&tmpObj);
	particle->Draw(*shader);
    */

    /*tmpObj.getTransform() = particlesTransfroms[0];
    shaderRoutine->operator()(&tmpObj);
    particle->Draw(*shader);
    */

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
            position = {v1/100,v2/100,v3/100};
        }
    }		
}

void ParticleSystem::updateSpawnFromEmitter(float timeValue)
{
    if(pause)
        return;

    if (timeValue - lastTime > 1) {
        lastTime = timeValue;
        for (auto& p: positions)
        {
            int max = static_cast<int>(maxBound);
            int min = static_cast<int>(minBound);
            int v1 = (rand() % (max - min)) + min;
            int v2 = (rand() % (max - 0));
            int v3 = (rand() % (max - min)) + min;

            p = emitter;
        }
    }		
}

void ParticleSystem::update(float timeValue)
{
    if(pause)
        return;
    updateUniform3DDistribution(timeValue);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * amount, &positions[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    particle->getVao().bind();

    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // this attribute comes from a different vertex buffer
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(3, 1); // tell OpenGL this is an instanced vertex attribute.
}
