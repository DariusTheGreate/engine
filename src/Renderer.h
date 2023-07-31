#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <optional>

#include "Window.h"
#include "GameState.h"
#include "Shader.h"
#include "InputManager.h"
#include "VBO.h"
#include "VAO.h"
#include "Texture.h"
#include "EBO.h"
#include "Model.h"
#include <Animator.h>
#include <CubeMesh.h>
#include <FlatMesh.h>
#include <Scene.h>
#include <Transform.h>
#include <ParticleSystem.h>
#include <Script.h>
#include <Color.h>
#include <PointLight.h>
#include <LightingShaderRoutine.h>
#include <OpenglWrapper.h>
#include <FrameBuffer.h>

/*
TODO(all):
	code:
        - move and drag - subentities
        - multithreaded objects loading
        - debug thread
        - benchmark
        - camera as object
        - mouse clickable objects
        - entities destruction
        - scene serialization(JSON(try simdjson eventually?) and binary)

    renderer:
        - better renderer
        - matrials
        - mesh optimization - https://github.com/zeux/meshoptimizer
	
    Physics:
        - Basic Collission detection and Physics         v
        - Inertia Matrix physics                         v
        - Particles system                               v
        - Soft Bodyies
        - Liquid Simulation 
        - Cloth Simulation
        - Hair Simulation

    imgui:
        - Panel system
        - general menu
        - show file system traversal
        - better properties traverse

    refs:
        - unity coliders - https://www.youtube.com/watch?v=bh9ArKrPY8w&ab_channel=Unity
        - titanfall SIMD collisions - https://www.youtube.com/watch?v=6BIfqfC1i7U&t=81s&ab_channel=GDC
        - GJK explained - https://www.youtube.com/watch?v=ajv46BSqcK4&ab_channel=Reducible
        - Minkowski diff by Casey - https://www.youtube.com/watch?v=_g8DLrNyVsQ&t=2933s&ab_channel=MollyRocket
        - Gregory - http://ce.eng.usc.ac.ir/files/1511334027376.pd
        - RigidBody Dynamics - https://ocw.mit.edu/courses/16-07-dynamics-fall-2009/dd277ec654440f4c2b5b07d6c286c3fd_MIT16_07F09_Lec26.pdf
        - Quaternions - https://www.3dgep.com/understanding-quaternions/
        - Hair - https://github.com/hankeyyh/FurSim/tree/master
        - Grass - https://www.youtube.com/watch?v=Ibe1JBF5i5Y&t=0s&ab_channel=GDC
        - Vertex optimisation - https://tomforsyth1000.github.io/papers/fast_vert_cache_opt.html
        - Mikkelsen - https://gamedev.stackexchange.com/questions/146855/how-do-you-compute-the-tangent-space-vectors-with-normals-given-in-the-mesh
        - GPU RigidBody - https://developer.nvidia.com/gpugems/gpugems3/part-v-physics-simulation/chapter-29-real-time-rigid-body-simulation-gpus
        - Physics - https://theswissbay.ch/pdf/Gentoomen%20Library/Game%20Development/Programming/Game%20Physics%20Engine%20Development.pdf
        - particles - https://www.youtube.com/watch?v=G6OGKP3MaUI&list=PLnuhp3Xd9PYTt6svyQPyRO_AAuMWGxPzU&index=168&ab_channel=MollyRocket
        - Geometry - https://www.geometrictools.com/Source/Mathematics.html
        - https://en.wikipedia.org/wiki/Gauss%E2%80%93Seidel_method
        - https://raphaelpriatama.medium.com/sequential-impulses-explained-from-the-perspective-of-a-game-physics-beginner-72a37f6fea05
        - "Predictive contacts"
        - "Inastanced rendering"
        - https://hitokageproduction.com/article/11
        - https://gafferongames.com/post/collision_response_and_coulomb_friction/
        - https://www.youtube.com/watch?v=DGVZYdlw_uo&ab_channel=kuju
        - AOE networking - https://www.gamedeveloper.com/programming/1500-archers-on-a-28-8-network-programming-in-age-of-empires-and-beyond
        - Porting - https://gamedev.stackexchange.com/questions/103941/porting-sdl-opengl-game-to-android-and-ios
        - Net - https://web.archive.org/web/20210419133753/https://gameserverarchitecture.com/
        - Android port - https://arm-software.github.io/opengl-es-sdk-for-android/tutorials.html
                         https://keasigmadelta.com/store/wp-content/uploads/2017/03/GLES3-and-SDL2-Tutorials.pdf
        - mesh optimizer - https://github.com/zeux/meshoptimizer
        - https://www.youtube.com/watch?v=k2h9FORbLa4&ab_channel=Gamefromscratch
        - https://www.boost.org/doc/libs/1_82_0/doc/html/boost_dll/tutorial.html
        - https://www.youtube.com/watch?v=nmxZmIOAosY&ab_channel=mohamedshaalan
        - https://github.com/GarrettGunnell/CS2-Smoke-Grenades 
        - https://github.com/frtru/GemParticles
        - profiler - https://github.com/FlaxEngine/FlaxEngine/blob/master/Source/Engine/Profiler/ProfilerCPU.cpp
        - reddit thread om gpu memory - https://www.reddit.com/r/opengl/comments/oxl2fi/what_are_some_good_practices_to_manage_gpu_memory/
        - lecture - https://www.youtube.com/watch?v=-bCeNzgiJ8I&list=PLckFgM6dUP2hc4iy-IdKFtqR9TeZWMPjm&ab_channel=SteamworksDevelopment
        - opengl PBO - http://www.songho.ca/opengl/gl_pbo.html
        - fast texture example - https://stackoverflow.com/questions/13358455/opengl-combine-textures
        - J.Blow skeletal animations stuff - https://www.youtube.com/watch?v=4MBXWFfGYpo&ab_channel=UNOFFICIALJonathanBlowstreamarchive
        - Texture framebuffer multisample  https://stackoverflow.com/questions/42878216/opengl-how-to-draw-to-a-multisample-framebuffer-and-then-use-the-result-as-a-n
        - RDR2 - https://imgeself.github.io/posts/2020-06-19-graphics-study-rdr2/
        - DOOM - https://www.adriancourreges.com/blog/2016/09/09/doom-2016-graphics-study/
        - Godot - https://godotengine.org/article/godot-3-renderer-design-explained/
        - Frustrum culling https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/frustum-culling-r4613/
        - GTA5 - https://www.adriancourreges.com/blog/2015/11/02/gta-v-graphics-study/
        - Far Objects - https://www.reddit.com/r/opengl/comments/8z5egn/rendering_large_and_distant_object/?utm_source=share&utm_medium=web2x&context=3
        - GTA6 - https://vk.com/video-120800128_456251871?ysclid=lkfo8jp8lg311066769
*/


class DebugRenderer 
{
public:
    DebugRenderer();

    void setupSceneGrid();

    void renderDebugColider(Window* wind, std::optional<Colider>& collider, std::optional<RigidBody>& body);

    void renderDebugCube(glm::vec3 pos, int r);

    void renderDebugPoint(glm::vec3 a, glm::vec4 color);

    void renderDebugGrid();

    void renderDebugLightSource(std::optional<PointLight>& p);

    void updateCamera(glm::mat4 projection, glm::mat4 view);

    void renderPoints();

    void clearPoints();

    struct PointToRender{
        glm::vec3 point;
        glm::vec4 color = {0,1,0,0};

        PointToRender(glm::vec3 pos, glm::vec4 col = glm::vec4(0,1,0,0)) : point(pos), color(col)
        {
    
        }
    };

    std::vector<PointToRender> pointsToRender;
    bool debug_render_points = true;
private:
    size_t length = 0;
    size_t slices = 200;
	std::vector<glm::vec3> vertices_grid;
	std::vector<glm::uvec4> indices_grid;
    int grid_scale = 1;
    int grid_mode = 3;

    //TODO(darius) use cubeMesh
    float vertices[48 * 6] = {
		-0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f, -0.5f, 
		 0.5f,  0.5f, -0.5f, 
		 0.5f,  0.5f, -0.5f, 
		-0.5f,  0.5f, -0.5f, 
		-0.5f, -0.5f, -0.5f, 

		-0.5f, -0.5f,  0.5f, 
		 0.5f, -0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f, 
		-0.5f,  0.5f,  0.5f, 
		-0.5f, -0.5f,  0.5f, 

		-0.5f,  0.5f,  0.5f, 
		-0.5f,  0.5f, -0.5f, 
		-0.5f, -0.5f, -0.5f, 
		-0.5f, -0.5f, -0.5f, 
		-0.5f, -0.5f,  0.5f, 
		-0.5f,  0.5f,  0.5f, 

		 0.5f,  0.5f,  0.5f, 
		 0.5f,  0.5f, -0.5f, 
		 0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f, 

		-0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f,  0.5f, 
		 0.5f, -0.5f,  0.5f, 
		-0.5f, -0.5f,  0.5f, 
		-0.5f, -0.5f, -0.5f, 

		-0.5f,  0.5f, -0.5f, 
		 0.5f,  0.5f, -0.5f, 
		 0.5f,  0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f, 
		-0.5f,  0.5f,  0.5f, 
		-0.5f,  0.5f, -0.5f, 
    };

    unsigned int indices[6] = {
        0, 1, 3,
        1, 2, 3
    };

    VBO vbo;

    VBO lightVbo;

    VAO vao;

    VAO vao_grid;

    EBO ebo;

    Shader dsv;
    Shader dsf;

    FlatMesh flat;
};

class RenderBuffer 
{
public:
    RenderBuffer() = default;
    
    RenderBuffer(unsigned int W, unsigned int H) 
    {
        glGenRenderbuffers(1, &ID);
        glBindRenderbuffer(GL_RENDERBUFFER, ID);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, W, H);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ID);
    }

private:
    unsigned int ID = 0;
};

class RendererQuad 
{
public:
    RendererQuad() : qv(GameState::engine_path + "shaders/quadShader.glsl", GL_VERTEX_SHADER), 
        qf(GameState::engine_path + "shaders/quadShaderFragment.glsl", GL_FRAGMENT_SHADER)
    {
		qv.compile();
		qf.compile();
		qv.link(qf);

        /*vao.init();
        vbo.init();
        vao.bind();
        vbo.bind(vertices.size() * sizeof(float), &vertices[0]);
        vbo.vboEnableVertexAttribArray(0);
        vbo.setVAO(0,2,GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

        vbo.vboEnableVertexAttribArray(1);
        vbo.setVAO(1,2,GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 + sizeof(float)));*/

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    void DrawQuad(unsigned int screenTexture)
    {
        /*glUseProgram(qv.getProgram());
        vao.bind();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        */

        glUseProgram(qv.getProgram());

        glBindVertexArray(quadVAO);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void DrawQuadToBindedTexture(Shader s) 
    {
		glUseProgram(s.getProgram());

        glBindVertexArray(quadVAO);

        //glActiveTexture(GL_TEXTURE0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void bindQuadVAO() 
    {
        glBindVertexArray(quadVAO);
    }

    void drawArrays() 
    {
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

private:
    VAO vao;
    VBO vbo;
    unsigned int quadVAO;
    unsigned int quadVBO;

    Shader qv;
    Shader qf;

    std::vector<float> vertices = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
	};
};

class ShaderLibrary
{
public:
    enum class STAGE
    {
        DEPTH,
        EDITOR_ID,
        ALBEDO,
        SHADOWS,
        BLUR,
    };

public:

    ShaderLibrary() : lightingVertex(GameState::engine_path + "shaders/vertexShader.glsl", GL_VERTEX_SHADER),
        lightingFragment(GameState::engine_path + "shaders/lightSumFragmentShader.glsl", GL_FRAGMENT_SHADER),
        depthVertex(GameState::engine_path + "shaders/shadowMappingDepthVertex.glsl", GL_VERTEX_SHADER),
        depthFragment(GameState::engine_path + "shaders/shadowMappingDepthFragment.glsl", GL_FRAGMENT_SHADER),
        shadowVertex(GameState::engine_path + "shaders/shadowMappingVertex.glsl", GL_VERTEX_SHADER),
        shadowFragment(GameState::engine_path + "shaders/shadowMappingFragment.glsl", GL_FRAGMENT_SHADER),
        blurVertex(GameState::engine_path + "shaders/blurShaderVertex.glsl", GL_VERTEX_SHADER),
        blurFragment(GameState::engine_path + "shaders/blurShaderFragment.glsl", GL_FRAGMENT_SHADER)
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

        stage = STAGE::DEPTH;
    }

    Shader& getCurrShader()
    {
        if (stage == STAGE::ALBEDO)
            return lightingVertex;
        else if (stage == STAGE::DEPTH)
            return depthVertex;
        else if (stage == STAGE::SHADOWS)
            return shadowVertex;
        else
            return blurVertex;
    }

    Shader& getDepthShader()
    {
        return depthVertex;
    }

    Shader& getAlbedoShader()
    {
        return lightingVertex;
    }

    Shader& getShadowsShader()
    {
        return shadowVertex;
    }

    Shader& getBlurShader() 
    {
        return blurVertex;
    }

    LightingShaderRoutine& getShaderRoutine() 
    {
        return routine;
    }

    STAGE stage;

    unsigned int depthMap = 0;
private:
    LightingShaderRoutine routine;

    Shader lightingVertex;
    Shader lightingFragment;

    Shader depthVertex;
    Shader depthFragment;

    Shader shadowVertex;
    Shader shadowFragment;

    Shader blurVertex;
    Shader blurFragment;
};

class Renderer
{
public:
    Renderer() = default;
    Renderer(Scene* currScene_in, GameState* instance, Window* wind);

    void render(Window* wind);
	void updateBuffers(Window* wind);

    size_t getShaderRoutine();

    LightingShaderRoutine& getCurrShaderRoutine();

    Shader getShader();

    Scene* getScene();

    DebugRenderer& getDebugRenderer();

    void blurStage();
    
    glm::vec3 backgroundColor = glm::vec3{0.1f, 0.0f, 0.1f};

    FrameBuffer framebuffer;
    FrameBuffer depthTexture;
    FrameBuffer depthFramebuffer;
    FrameBuffer intermidiateFramebuffer;
    FrameBuffer bloomBuffer;
    FrameBuffer pingPongBlurBufferA;
    FrameBuffer pingPongBlurBufferB;
    RenderBuffer renderBuffer;

    static ShaderLibrary* shaderLibInstance;
private:
	void renderDebug(Window* wind);
    void renderScene(Window* wind);

private:
    DebugRenderer dbr;
    RendererQuad quad;

    Scene* currScene;

    //TODO(darius) make it lighting system
    float lastFrame = 0;

    Model m;
};

