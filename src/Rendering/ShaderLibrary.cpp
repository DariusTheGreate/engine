#define _WIN32_WINNT 0x0601
#include <Rendering/ShaderLibrary.h>
#include <Core/Timer.h>
#include <Engine/Editor.h>
#include <Engine/Object.h>

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
        cubeMapFragment(GameState::engine_path + "shaders/cubeMapFragment.glsl", GL_FRAGMENT_SHADER),
        quadVertex(GameState::engine_path + "shaders/quadShader.glsl", GL_VERTEX_SHADER),
        quadFragment(GameState::engine_path + "shaders/quadShaderFragment.glsl", GL_FRAGMENT_SHADER)
    
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

        quadVertex.compile();
        quadFragment.compile();
        quadVertex.link(quadFragment);
        
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

Shader& ShaderLibrary::getQuadShader()
{
    return quadVertex;
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
        
		std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANGED_RELOADING_HAPPENED IN ALBEDO SHADER: " << std::endl;
    }

    if(textureCombinerFragment.checkForSourceChanges() || textureCombinerVertex.checkForSourceChanges())
    {
        textureCombinerVertex.reload();
        textureCombinerFragment.reload();

        textureCombinerVertex.compile();
        textureCombinerFragment.compile();
        textureCombinerVertex.link(textureCombinerFragment);
        
		std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANGED_RELOADING_HAPPENED IN TEXTURE COMBINER SHADER: " << std::endl;
    }

    if(depthFragment.checkForSourceChanges() || depthVertex.checkForSourceChanges())
    {
        depthFragment.reload();
        depthVertex.reload();

        depthVertex.compile();
        depthFragment.compile();
        depthVertex.link(depthFragment);

		std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANGED_RELOADING_HAPPENED IN DEPTH SHADER: " << std::endl;
    }

    if (shadowFragment.checkForSourceChanges() || shadowVertex.checkForSourceChanges()) 
    {
        shadowVertex.reload();
        shadowFragment.reload();

        shadowVertex.compile();
        shadowFragment.compile();
        shadowVertex.link(shadowFragment);

		std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANGED_RELOADING_HAPPENED IN SHADOW SHADER: " << std::endl;
    }

    if(blurFragment.checkForSourceChanges() || blurVertex.checkForSourceChanges())
    {
        blurVertex.reload();
        blurFragment.reload();

        shadowVertex.compile();
        shadowFragment.compile();
        shadowVertex.link(shadowFragment);

		std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANGED_RELOADING_HAPPENED IN BLUR SHADER: " << std::endl;
    }

    if(editorIdFragment.checkForSourceChanges() || editorIdVertex.checkForSourceChanges())
    {
		editorIdVertex.reload();
        editorIdFragment.reload();

        editorIdVertex.compile();
        editorIdFragment.compile();
        editorIdVertex.link(editorIdFragment);

		std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANGED_RELOADING_HAPPENED IN EDITOR_ID SHADER: " << std::endl;
    }

    if(bokeFragment.checkForSourceChanges() || bokeVertex.checkForSourceChanges())
    {
		bokeVertex.reload();
        bokeFragment.reload();

        bokeVertex.compile();
        bokeFragment.compile();
        bokeVertex.link(bokeFragment);

		std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANGED_RELOADING_HAPPENED IN BOKE SHADER: " << std::endl;
    }

    if (gBufferFragment.checkForSourceChanges() || gBufferVertex.checkForSourceChanges())
    {
        gBufferVertex.reload();
        gBufferFragment.reload();

        gBufferVertex.compile();
        gBufferFragment.compile();
        gBufferVertex.link(gBufferFragment);

        std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANGED_RELOADING_HAPPENED IN GBUFFER SHADER: " << std::endl;
    }

    if (particlesFragment.checkForSourceChanges() || particlesVertex.checkForSourceChanges())
    {
        particlesVertex.reload();
        particlesFragment.reload();

        particlesVertex.compile();
        particlesFragment.compile();
        particlesVertex.link(particlesFragment);

        std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANGED_RELOADING_HAPPENED IN PARTICLES SHADER: " << std::endl;
    }

    if (terrainFragment.checkForSourceChanges() || terrainVertex.checkForSourceChanges())
    {
        terrainVertex.reload();
        terrainFragment.reload();

        terrainVertex.compile();
        terrainFragment.compile();
        terrainVertex.link(terrainFragment);

        std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANGED_RELOADING_HAPPENED IN TERRAIN SHADER: " << std::endl;
    }

    if (skeletalAnimationFragment.checkForSourceChanges() || skeletalAnimationVertex.checkForSourceChanges())
    {
        skeletalAnimationVertex.reload();
        skeletalAnimationFragment.reload();

        skeletalAnimationVertex.compile();
        skeletalAnimationFragment.compile();
        skeletalAnimationVertex.link(skeletalAnimationFragment);

        std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANGED_RELOADING_HAPPENED IN SKELETAL ANIMATION SHADER: " << std::endl;
    }

    if (quadFragment.checkForSourceChanges() || quadVertex.checkForSourceChanges())
    {
        quadVertex.reload();
        quadFragment.reload();

        quadVertex.compile();
        quadFragment.compile();
        quadVertex.link(quadFragment);

        std::cout << "NOTIFICATION::SHADER_LIBRARY::FILE_WAS_CHANGED_RELOADING_HAPPENED IN QUAD SHADER: " << std::endl;
    }
}

void ShaderLibrary::loadCurrentShader()
{
    //Timer t(1);
    Shader& sv = getCurrShader();
    cache.SwitchShader(sv.getProgram());
    //glUseProgram(sv.getProgram());
}

void getColorFromRgbId(double id)
{
    //double r = ((id) & 0x000000FF) >> 0;
    //double b = ((id) & 0x0000FF00) >> 8;
    //double g = (id & 0x00FF0000) >> 16; 
}

void distanceRendering(Object* obj)
{
    auto& transfromRef = obj->getTransform();
    glm::vec3 pos = transfromRef.getPosition();
    glm::mat4 q = transfromRef.matrix;
    //glm::vec3 scale = obj->getTransform().geS;

    //TODO(darius) add distanced rendering here. NOTE(darius) Not that hard
    float dist = glm::length(GameState::instance->cam.getCameraPos() - pos);
    if(dist > 99) 
    {
        //scale *= 0.5;//2 * GameState::cam.getFov()/dist;
        pos = GameState::instance->cam.getCameraPos() + ((GameState::instance->cam.getCameraPos() - pos));
    }
}

//TODO(darius) try to pack all uniform data into one batch
void ShaderLibrary::shaderRoutine(Object* obj)
{
    //Timer t(true);//~0.0003

    Shader& sv = getCurrShader();

    //println((int)stage, obj->get_name());

    loadCurrentShader();

    if(obj->getSkeletalAnimation()){
        //auto transforms = obj->getAnimator()->getCurrentAnimation()->getFinalBoneMatrices();
        auto transforms = obj->getSkeletalAnimation()->getFinalBoneMatrices();
        //auto variantCaller = [&transforms](auto& obj){ transforms = obj.getFinalBoneMatrices(); };

        //obj->getAnimator()->access(variantCaller);

        for (int i = 0; i < transforms.size(); ++i)
            sv.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    } 

    if(stage == STAGE::ALBEDO)
    {
        sv.setVec3("viewPos", GameState::instance->cam.getCameraPos());
        sv.setInt("lightsCount", PointLight::LightsCount);
        sv.setFloat("gammaFactor", GameState::gammaFactor); 
        sv.setFloat("gammaBrightness", GameState::gammaBrightness); 
        sv.setBool("shadowCaster", obj->shadowCasterRef());
    }

    if(stage == STAGE::EDITOR_ID){
        double id = obj->getID();
        sv.setVec4("PickingColor", glm::vec4{id/255,id/255,id/255,0});
    }

    if(GameState::shadowEnabled){
        OpenglWrapper::SetShaderInt(sv.getShader(), "depthMap", 4);//NOTE(darius) now depthMap is at binmding 16(works on glsl version 420)
        OpenglWrapper::ActivateTexture(GL_TEXTURE0 + 4);
        OpenglWrapper::BindTexture(depthMap);
    }

    sv.setMat4("lightSpaceMatrix", DirectionalLight::getLightMat());
    sv.setVec3("lightPos", DirectionalLight::lightPos);

    glm::mat4 projection = GameState::instance->cam.getPerspective(Editor::window->getWidth(), Editor::window->getHeight());
    glm::mat4 view = (GameState::instance->cam.getBasicLook());

    sv.setMat4("projection", projection);
    sv.setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    model *= obj->getTransform().matrix;
    model = glm::scale(model, {0.1,0.1,0.1});//for space scaling and depth buffer

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
