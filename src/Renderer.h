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

class Renderer
{
public:
    Renderer() = default;
    Renderer(Scene* currScene_in, GameState* instance);

    void render(Window* wind, bool& debug_mode);

    void updateBuffers(Window* wind);

    size_t getShaderRoutine();

    LightingShaderRoutine& getCurrShaderRoutine();

    Shader getShader();

    DebugRenderer& getDebugRenderer();
    
    glm::vec3 backgroundColor = glm::vec3{0.5f, 0.5f, 0.5f};

private:
    DebugRenderer dbr;
    Shader sv;
    Shader sf;

    Scene* currScene;

    LightingShaderRoutine currShaderRoutine;
    EmptyScriptRoutine routine;

    //TODO(darius) make it lighting system
    PointLight pointLight;
    DirectionalLight directionalLight;
    SpotLight spotLight;

    float lastFrame = 0;
};

