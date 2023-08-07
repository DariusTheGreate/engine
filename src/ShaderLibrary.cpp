#include <ShaderLibrary.h>

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
        deferredLightPasFragment(GameState::engine_path + "shaders/deferredLightPasFragment.glsl", GL_FRAGMENT_SHADER)
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
        else
            return deferredLightPasVertex;
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

    LightingShaderRoutine& ShaderLibrary::getShaderRoutine() 
    {
        return routine;
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
    }

