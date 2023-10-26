#include <ShaderLibrary.h>
#include <Timer.h>
#include <Object.h>

ShaderLibrary::ShaderLibrary() : lightingVertex(GameState::engine_path + "shaders/vertexShader.glsl", GL_VERTEX_SHADER),
        lightingFragment(GameState::engine_path + "shaders/lightSumFragmentShader.glsl", GL_FRAGMENT_SHADER),
        depthVertex(GameState::engine_path + "shaders/shadowMappingDepthVertex.glsl", GL_VERTEX_SHADER),
        depthFragment(GameState::engine_path + "shaders/shadowMappingDepthFragment.glsl", GL_FRAGMENT_SHADER),
        shadowVertex(GameState::engine_path + "shaders/shadowMappingVertex.glsl", GL_VERTEX_SHADER),
        shadowFragment(GameState::engine_path + "shaders/shadowMappingFragment.glsl", GL_FRAGMENT_SHADER),
        blurVertex(GameState::engine_path + "shaders/blurShaderVertex.glsl", GL_VERTEX_SHADER),
        blurFragment(GameState::engine_path + "shaders/blurShaderFragment.glsl", GL_FRAGMENT_SHADER),
        textureCombinerVertex(GameState::engine_path + "shaders/textureCombinerVertex.glsl", GL_VERTEX_SHADER),
        textureCombinerFragment(GameState::engine_path + "shaders/textureCombinerFragment.glsl", GL_FRAGMENT_SHADER),
        editorIdVertex(GameState::engine_path + "shaders/vertexShader.glsl", GL_VERTEX_SHADER),
        editorIdFragment(GameState::engine_path + "shaders/editorIDFragment.glsl", GL_FRAGMENT_SHADER),
        bokeVertex(GameState::engine_path + "shaders/bokeVertex.glsl", GL_VERTEX_SHADER),
        bokeFragment(GameState::engine_path + "shaders/bokeFragment.glsl", GL_FRAGMENT_SHADER),
        gBufferVertex(GameState::engine_path + "shaders/GBufferVertex.glsl", GL_VERTEX_SHADER),
        gBufferFragment(GameState::engine_path + "shaders/GBufferFragment.glsl", GL_FRAGMENT_SHADER),
        deferredLightPasVertex(GameState::engine_path + "shaders/deferredLightPasVertex.glsl", GL_VERTEX_SHADER),
        deferredLightPasFragment(GameState::engine_path + "shaders/deferredLightPasFragment.glsl", GL_FRAGMENT_SHADER),
        particlesVertex(GameState::engine_path + "shaders/particleVertexShader.glsl", GL_VERTEX_SHADER),
        particlesFragment(GameState::engine_path + "shaders/particleFragmentShader.glsl", GL_FRAGMENT_SHADER),
        terrainVertex(GameState::engine_path + "shaders/terrainVertex.glsl", GL_VERTEX_SHADER),
        terrainFragment(GameState::engine_path + "shaders/terrainFragment.glsl", GL_FRAGMENT_SHADER),
        skeletalAnimationVertex(GameState::engine_path + "shaders/skeletalAnimationVertexShader.glsl", GL_VERTEX_SHADER),
        skeletalAnimationFragment(GameState::engine_path + "shaders/skeletalAnimationFragmentShader.glsl", GL_FRAGMENT_SHADER),
        cubeMapVertex(GameState::engine_path + "shaders/cubeMapVertex.glsl", GL_VERTEX_SHADER),
        cubeMapFragment(GameState::engine_path + "shaders/cubeMapFragment.glsl", GL_FRAGMENT_SHADER)
    
    {
        lightingVertex.compile();
        lightingFragment.compile();
        lightingVertex.link(lightingFragment);

        depthVertex.compile();
        depthFragment.compile();
        depthVertex.link(depthFragment);

        shadowVertex.compile();
        shadowFragment.compile();
        shadowVertex.link(shadowFragment);

        blurVertex.compile();
        blurFragment.compile();
        blurVertex.link(blurFragment);

        textureCombinerVertex.compile();
        textureCombinerFragment.compile();
        textureCombinerVertex.link(textureCombinerFragment);

        editorIdVertex.compile();
        editorIdFragment.compile();
        editorIdVertex.link(editorIdFragment);

        bokeVertex.compile();
        bokeFragment.compile();
        bokeVertex.link(bokeFragment);

        gBufferVertex.compile();
        gBufferFragment.compile();
        gBufferVertex.link(gBufferFragment);

		deferredLightPasVertex.compile();
        deferredLightPasFragment.compile();
        deferredLightPasVertex.link(deferredLightPasFragment);

        particlesVertex.compile();
        particlesFragment.compile();
        particlesVertex.link(particlesFragment);

        terrainVertex.compile();
        terrainFragment.compile();
        terrainVertex.link(terrainFragment);

        skeletalAnimationVertex.compile();
        skeletalAnimationFragment.compile();
        skeletalAnimationVertex.link(skeletalAnimationFragment);

        cubeMapVertex.compile();
        cubeMapFragment.compile();
        cubeMapVertex.link(cubeMapFragment);
        
        stage = STAGE::DEPTH;
    }

Shader& ShaderLibrary::getCurrShader()
{
    if (stage == STAGE::ALBEDO)
        return lightingVertex;
    else if (stage == STAGE::DEPTH)
        return depthVertex;
    else if (stage == STAGE::SHADOWS)
        return shadowVertex;
    else if (stage == STAGE::BLOOM)
        return blurVertex;
    else if (stage == STAGE::DEFERRED)
        return gBufferVertex;
    else if (stage == STAGE::DEFERRED_LIGHT)
        return deferredLightPasVertex;
    else if (stage == STAGE::EDITOR_ID)
        return editorIdVertex;
    else if (stage == STAGE::PARTICLES)
        return particlesVertex;
    else if (stage == STAGE::TERRAIN)
        return terrainVertex;
    else if (stage == STAGE::SKELETAL)
        return skeletalAnimationVertex;
    else if (stage == STAGE::CUBEMAP)
        return cubeMapVertex;
    else
        return lightingVertex;
}

Shader& ShaderLibrary::getDepthShader()
{
    return depthVertex;
}

Shader& ShaderLibrary::getAlbedoShader()
{
    return lightingVertex;
}

Shader& ShaderLibrary::getShadowsShader()
{
    return shadowVertex;
}

Shader& ShaderLibrary::getBlurShader()
{
    return blurVertex;
}

Shader& ShaderLibrary::getTextureCombinerShader()
{
    return textureCombinerVertex;
}

Shader& ShaderLibrary::getEditorIdShader()
{
    return editorIdVertex;
}

Shader& ShaderLibrary::getBokeShader()
{
    return bokeVertex;
}

Shader& ShaderLibrary::getGBufferShader()
{
    return gBufferVertex;
}

Shader& ShaderLibrary::getParticlesShader()
{
    return particlesVertex;
}

Shader& ShaderLibrary::getTerrainShader()
{
    return terrainVertex;
}

Shader& ShaderLibrary::getSkeletalShader()
{
    return skeletalAnimationVertex;
}

Shader& ShaderLibrary::getCubeMapShader()
{
    return cubeMapVertex; 
}

void ShaderLibrary::checkForShaderReload() 
{
    //TODO(darius) refactor copypast
    if(lightingFragment.checkForSourceChanges() || lightingVertex.checkForSourceChanges())
    {
        lightingVertex.reload();
        lightingFragment.reload();

        lightingVertex.compile();
        lightingFragment.compile();
        lightingVertex.link(lightingFragment);
        
		std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANED_RELOADING_HAPPENED IN ALBEDO SHADER: " << std::endl;
    }

    if(textureCombinerFragment.checkForSourceChanges() || textureCombinerVertex.checkForSourceChanges())
    {
        textureCombinerVertex.reload();
        textureCombinerFragment.reload();

        textureCombinerVertex.compile();
        textureCombinerFragment.compile();
        textureCombinerVertex.link(textureCombinerFragment);
        
		std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANED_RELOADING_HAPPENED IN TEXTURE COMBINER SHADER: " << std::endl;
    }

    if(depthFragment.checkForSourceChanges() || depthVertex.checkForSourceChanges())
    {
        depthFragment.reload();
        depthVertex.reload();

        depthVertex.compile();
        depthFragment.compile();
        depthVertex.link(depthFragment);

		std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANED_RELOADING_HAPPENED IN DEPTH SHADER: " << std::endl;
    }

    if (shadowFragment.checkForSourceChanges() || shadowVertex.checkForSourceChanges()) 
    {
        shadowVertex.reload();
        shadowFragment.reload();

        shadowVertex.compile();
        shadowFragment.compile();
        shadowVertex.link(shadowFragment);

		std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANED_RELOADING_HAPPENED IN SHADOW SHADER: " << std::endl;
    }

    if(blurFragment.checkForSourceChanges() || blurVertex.checkForSourceChanges())
    {
        blurVertex.reload();
        blurFragment.reload();

        shadowVertex.compile();
        shadowFragment.compile();
        shadowVertex.link(shadowFragment);

		std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANED_RELOADING_HAPPENED IN BLUR SHADER: " << std::endl;
    }

    if(editorIdFragment.checkForSourceChanges() || editorIdVertex.checkForSourceChanges())
    {
		editorIdVertex.reload();
        editorIdFragment.reload();

        editorIdVertex.compile();
        editorIdFragment.compile();
        editorIdVertex.link(editorIdFragment);

		std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANED_RELOADING_HAPPENED IN EDITOR_ID SHADER: " << std::endl;
    }

    if(bokeFragment.checkForSourceChanges() || bokeVertex.checkForSourceChanges())
    {
		bokeVertex.reload();
        bokeFragment.reload();

        bokeVertex.compile();
        bokeFragment.compile();
        bokeVertex.link(bokeFragment);

		std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANED_RELOADING_HAPPENED IN BOKE SHADER: " << std::endl;
    }

    if (gBufferFragment.checkForSourceChanges() || gBufferVertex.checkForSourceChanges())
    {
        gBufferVertex.reload();
        gBufferFragment.reload();

        gBufferVertex.compile();
        gBufferFragment.compile();
        gBufferVertex.link(gBufferFragment);

        std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANED_RELOADING_HAPPENED IN GBUFFER SHADER: " << std::endl;
    }

    if (particlesFragment.checkForSourceChanges() || particlesVertex.checkForSourceChanges())
    {
        particlesVertex.reload();
        particlesFragment.reload();

        particlesVertex.compile();
        particlesFragment.compile();
        particlesVertex.link(particlesFragment);

        std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANED_RELOADING_HAPPENED IN PARTICLES SHADER: " << std::endl;
    }

    if (terrainFragment.checkForSourceChanges() || terrainVertex.checkForSourceChanges())
    {
        terrainVertex.reload();
        terrainFragment.reload();

        terrainVertex.compile();
        terrainFragment.compile();
        terrainVertex.link(terrainFragment);

        std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANED_RELOADING_HAPPENED IN TERRAIN SHADER: " << std::endl;
    }

    if (skeletalAnimationFragment.checkForSourceChanges() || skeletalAnimationVertex.checkForSourceChanges())
    {
        skeletalAnimationVertex.reload();
        skeletalAnimationFragment.reload();

        skeletalAnimationVertex.compile();
        skeletalAnimationFragment.compile();
        skeletalAnimationVertex.link(skeletalAnimationFragment);

        std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANED_RELOADING_HAPPENED IN SKELETAL ANIMATION SHADER: " << std::endl;
    }
}

void ShaderLibrary::loadCurrentShader()
{
    //Timer t;
    Shader& sv = getCurrShader();
    //cache.SwitchShader(sv.getProgram());
    glUseProgram(sv.getProgram());
}

void ShaderLibrary::shaderRoutine(Object* obj)
{
    //TODO(darius) if{} else{} is better than uniform set

    Shader& sv = getCurrShader();

    //println((int)stage, obj->get_name());

    loadCurrentShader();

    if(obj->getAnimator()){
        //stage = STAGE::SKELETAL;
        auto transforms = obj->getAnimator()->GetFinalBoneMatrices();

        //auto tr = glm::mat4(1.0f);
        /*glm::mat4 ViewRotateX = glm::rotate(
            tr,
            3.14f,
            glm::vec3(-1.0f, 0.0f, 0.0f)
        );
        for(int i = 0; i < 99; ++i)
            transforms[i] = ViewRotateX;
            */
        //sv = getCurrShader();
        //loadCurrentShader();

        for (int i = 0; i < transforms.size(); ++i)
            sv.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    } 

    sv.setVec3("viewPos", GameState::cam.getCameraPos());
    sv.setInt("lightsCount", PointLight::LightsCount);
    sv.setFloat("gammaFactor", GameState::gammaFactor); 
    sv.setFloat("gammaBrightness", GameState::gammaBrightness); 

    //std::cout << "ID: " << obj->getID() << "\n";
    /*int id = obj->getID() * 25500;
    double r = ((id) & 0x000000FF) >> 0;
    double b = ((id) & 0x0000FF00) >> 8;
    double g = (id & 0x00FF0000) >> 16;

    sv.setVec4("PickingColor", glm::vec4{float(r/255),float(g/255),float(b/255),0});
    */

    double id = obj->getID();

    sv.setVec4("PickingColor", glm::vec4{id/255,id/255,id/255,0});

    /*glm::vec3 lightPos = glm::vec3(-2.0f, 0.0f, -1.0f);
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 7.5f;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    */


    OpenglWrapper::SetShaderInt(sv.getShader(), "depthMap", 16);//NOTE(darius) now depthMap is at binmding 16(works on glsl version 420)
    OpenglWrapper::ActivateTexture(GL_TEXTURE0 + 16);
    OpenglWrapper::BindTexture(depthMap);

    sv.setMat4("lightSpaceMatrix", DirectionalLight::getLightMat());

    sv.setBool("shadowCaster", obj->shadowCasterRef());

    glm::mat4 projection = GameState::cam.getPerspective(1920, 1080);
    glm::mat4 view = (GameState::cam.getBasicLook());

    sv.setVec3("lightPos", DirectionalLight::lightPos);
    sv.setMat4("projection", projection);
    sv.setMat4("view", view);

    //if(directionalLight)
    //    directionalLight->setShaderLight(sv);
    //if(pointLight)
    //    pointLight->setShaderLight(sv);

    glm::mat4 model = glm::mat4(1.0f);
    auto& transfromRef = obj->getTransform();
    glm::vec3 pos = transfromRef.getPosition();
    glm::mat4 q = transfromRef.matrix;
    //glm::vec3 scale = obj->getTransform().geS;

    //TODO(darius) add distanced rendering here. NOTE(darius) Not that hard
    float dist = glm::length(GameState::cam.getCameraPos() - pos);
    if(dist > 99) 
    {
        //scale *= 0.5;//2 * GameState::cam.getFov()/dist;
        pos = GameState::cam.getCameraPos() + ((GameState::cam.getCameraPos() - pos));
    }

    //model = glm::translate(model, pos);
    //model = glm::scale(model, scale);
    //model *= q;

    model = glm::mat4(1.0f);
    model *= obj->getTransform().matrix;
    glm::vec3 posm = obj->getTransform().getPosition();

    model = glm::scale(model, {0.1,0.1,0.1});//for space scaling and depth buffer
    //glm::mat4 mvp = glm::translate(model, posm);

    //model[3] = glm::vec4(posm.x, posm.y, posm.z, 0);
    //model = glm::translate(model, obj->getTransform().getPosition());
    //model = glm::scale(model, obj->getTransform().getScale());
    //glm::quat qu = obj->getTransform().matrix;
    //glm::mat4 RotationMatrix = glm::toMat4(qu);
    //NOTE(darius) brokes if uncomment
    //model *= RotationMatrix;

    sv.setMat4("model", model);
}

void ShaderLibrary::skeletalAnimationShaderRoutine(Transform tr){
    /*if(animator){
        auto transforms = animator->GetFinalBoneMatrices();

        auto tr = glm::mat4(1.0f);
        /*glm::mat4 ViewRotateX = glm::rotate(
            tr,
            3.14f,
            glm::vec3(-1.0f, 0.0f, 0.0f)
        );
        for(int i = 0; i < 99; ++i)
            transforms[i] = ViewRotateX;
            */
        /*for (int i = 0; i < transforms.size(); ++i)
            sv.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    }

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 pos = tr.getPosition();
    glm::mat4 q = tr.matrix;
    glm::vec3 scale = { tr.getScale().x / 100, tr.getScale().y / 100, tr.getScale().z / 100};

    model = glm::translate(model, pos);
    model *= q;
    model = glm::scale(model, scale);

    sv.setMat4("model", model);
    */
}
