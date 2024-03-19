#include <windows.h>
#include "Renderer.h"
#include <Physics/Colider.h>
#include <Engine/ObjectSelector.h>
#include <Core/Timer.h>
#include <Engine/FrustumCulling.h>
#include <Engine/OcclusionCulling.h>


#include <optional>

DebugRenderer::DebugRenderer() : dsv(GameState::engine_path + "shaders/debugVertexShader.glsl", GL_VERTEX_SHADER), dsf(GameState::engine_path + "shaders/debugFragmentShader.glsl", GL_FRAGMENT_SHADER)
{
	dsv.compile();
	dsf.compile();
	dsv.link(dsf);

	vbo.init();
	ebo.init();
	vao.init();

	vao.bind();

	vbo.bind(48 * 6 * sizeof(float), vertices);
	vbo.setVAO(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	vbo.vboEnableVertexAttribArray(0);

	ebo.bind(6 * sizeof(unsigned int), indices);

	setupSceneGrid();
}

void DebugRenderer::setupSceneGrid()
{
	std::vector<std::string> faces
	{
	    GameState::engine_path + "textures/skybox/skybox/right.jpg",
	    GameState::engine_path + "textures/skybox/skybox/left.jpg",
	    GameState::engine_path + "textures/skybox/skybox/top.jpg",
	    GameState::engine_path + "textures/skybox/skybox/bottom.jpg",
	    GameState::engine_path + "textures/skybox/skybox/front.jpg",
	    GameState::engine_path + "textures/skybox/skybox/back.jpg"
	};

	//cubemap.loadCubemap(faces);
	//cubemap.setup();
	
    grid_mode = GameState::editor_mode;
    //std::cout << "grid setup " << grid_mode << "\n";
    indices_grid.clear();
    vertices_grid.clear();
    vertices_grid.shrink_to_fit();
    indices_grid.shrink_to_fit();

	for (int i = 0; i <= slices; ++i) {
		for (int j = 0; j <= slices; ++j) {
			float x = (float)j;
            float y = 0.0f;
            float z = (float)i;

		    if(grid_mode == 2){	
                x = (float)j;
                y = (float)i;
                z = 0;
            }

            if(grid_mode == 3){

            	//https://devforum.roblox.com/t/how-to-spawn-things-along-the-surface-of-a-sphere/1566555/3
            	//https://stackoverflow.com/questions/9604132/how-to-project-a-point-on-to-a-sphere
            	/*
            	Write the point in a coordinate system centered at the center of the sphere (x0,y0,z0):

				P = (x',y',z') = (x - x0, y - y0, z - z0)

				Compute the length of this vector:

				|P| = sqrt(x'^2 + y'^2 + z'^2)

				Scale the vector so that it has length equal to the radius of the sphere:

				Q = (radius/|P|)*P

				And change back to your original coordinate system to get the projection:

				R = Q + (x0,y0,z0)
            	*/

            	glm::vec3 pos = glm::vec3((x-slices/2) * grid_scale, (y) * grid_scale, (z-slices/2) * grid_scale);

            	//pos.y -= 0.1 * (i > j ? i : j);

                vertices_grid.push_back(pos);
            }

            else if(grid_mode == 2)
                vertices_grid.push_back(glm::vec3((x-slices/2) * grid_scale, (y-slices/2) * grid_scale, (z) * grid_scale));
		}
	}

	for (int i = 0; i < slices; ++i) {
		for (int j = 0; j < slices; ++j) {
			int row1 = j * (static_cast<int>(slices) + 1);
			int row2 = (i + 1) * (static_cast<int>(slices) + 1);

			indices_grid.push_back(glm::vec4(row1 + j, row1 + j + 1, row1 + j + 1, row2 + j + 1));
			indices_grid.push_back(glm::vec4(row2 + j + 1, row2 + j, row2 + j, row1 + j));
		}
	}

	vao_grid.init();
	VBO vbo_grid;
	vbo_grid.init();
	EBO ebo_grid;
	ebo_grid.init();
	
	vao_grid.bind();
	vbo_grid.bind(vertices_grid.size() * sizeof(glm::vec3), &(vertices_grid[0]));
	vbo_grid.setVAO(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	vbo_grid.vboEnableVertexAttribArray(0);
	ebo_grid.bind(indices_grid.size() * sizeof(glm::uvec4), &(indices_grid[0]));

	length = (GLuint)indices_grid.size() * 4;
	glLineWidth(2);
}
void DebugRenderer::renderDebugColider(Window* wind, std::optional<Colider>& collider, std::optional<RigidBody>& body)
{
	if(!collider)
		return;

	Renderer::shaderLibInstance->getCache().SwitchShader(dsv.getProgram());
	auto model = glm::mat4(1.0f);
	model = glm::translate(model, collider->get_transform().getPosition() + glm::vec3{collider->get_size().x / 2, collider->get_size().y / 2, collider->get_size().z / 2});

	//if(body)
	//	model *= body->get_quatmat();

	//model *= glm::toMat4(collider->get_transform().matrixQuaternion());
	auto vv = collider->get_render_shift();
	model = glm::translate(model, glm::vec3{vv.x, vv.y, vv.z});

	//TODO(darius) its not size, its scale
	model = glm::scale(model, collider->get_size());

    //dont! model = glm::scale(model, {0.1,0.1,0.1});//for space scaling and depth buffer

	//model = glm::scale(model, glm::vec3{size.x, size.y,size.z});
	//model[3] += glm::vec4{size.x/2 -size.x, size.y/2-size.x,size.z/2-size.x,0};
	dsv.setVec4("objectColor", collider->collider_debug_color);
	dsv.setMat4("model", model);
	vao.bind();
	glDrawArrays(GL_LINE_STRIP, 0, 36);
	glBindVertexArray(0);
}

void DebugRenderer::renderDebugCube(glm::vec3 pos, float x, float y, float z)
{
	Renderer::shaderLibInstance->getCache().SwitchShader(dsv.getProgram());
	auto model = glm::mat4(1.0f);
	//print(pos, x, y, z, "\n");
	model = glm::translate(model, pos + glm::vec3(-0.5 * x, -0.5 * y, 0.5 * z));
	model = glm::scale(model, {x,y,z});
	//model = glm::scale(model, glm::vec3{size.x, size.y,size.z});
	//model[3] += glm::vec4{size.x/2 -size.x, size.y/2-size.x,size.z/2-size.x,0};
	dsv.setVec4("objectColor", {0,0,1,0});
	dsv.setMat4("model", model);
	vao.bind();
	glDrawArrays(GL_LINE_STRIP, 0, 36);
	glBindVertexArray(0); 
}

//TODO(darius) make it instanced. Dont upload each point individually
void DebugRenderer::renderDebugPoint(glm::vec3 a, glm::vec4 color = glm::vec4(0,1,0,0))
{
    if(!debug_render)
        return;

	Renderer::shaderLibInstance->getCache().SwitchShader(dsv.getProgram());
	auto model = glm::mat4(1.0f);
	model = glm::translate(model, a);
	model = glm::scale(model, {0.05,0.05,0.05});
	//TODO(darius) its not size, its scale
	//model = glm::scale(model, {r,r,r});
	//model = glm::scale(model, glm::vec3{size.x, size.y,size.z});
	//model[3] += glm::vec4{size.x/2 -size.x, size.y/2-size.x,size.z/2-size.x,0};
	dsv.setVec4("objectColor", color);
	dsv.setMat4("model", model);
	flat.DrawRaw(dsv, a, {1,1,1});
	//vao.bind();
	//glDrawArrays(GL_LINE_STRIP, 0, 36);
	//glBindVertexArray(0);
}

void DebugRenderer::renderDebugLine(glm::vec3 a, glm::vec3 b, glm::vec4 color)
{
	Renderer::shaderLibInstance->getCache().SwitchShader(dsv.getProgram());
	auto model = glm::mat4(1.0f);
	
	float x = b.x - a.x;
	float y = b.y - a.y;
	float z = b.z - a.z;

	model = glm::translate(model, a);
	model = glm::scale(model, {x,y,z});

	model = glm::rotate(model, x, glm::vec3(1,0,0));	
	model = glm::rotate(model, y, glm::vec3(0,1,0));	
	model = glm::rotate(model, z, glm::vec3(0,0,1));	

	dsv.setVec4("objectColor", {0,0,1,0});
	dsv.setMat4("model", model);
	vao.bind();
	glDrawArrays(GL_LINE_STRIP, 0, 2);
	glBindVertexArray(0); 
}

void DebugRenderer::renderDebugGrid()
{
	//cubemap.draw();

    if(GameState::editor_mode != grid_mode)
    {
        setupSceneGrid(); 
    }

	Renderer::shaderLibInstance->getCache().SwitchShader(dsv.getProgram());
	auto model = glm::mat4(1.0f);
	dsv.setVec4("objectColor", {0.4,0.4,0.4,0});
	dsv.setMat4("model", model);
	vao_grid.bind();
	glDrawElements(GL_LINES, static_cast<int>(length), GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

void DebugRenderer::renderDebugLightSource(std::optional<PointLight>& p)
{
	if(!p)
		return;
	Renderer::shaderLibInstance->getCache().SwitchShader(dsv.getProgram());
	auto model = glm::mat4(1.0f);
	model = glm::translate(model, p->position);
	model = glm::scale(model, glm::vec3{0.02,0.02,0.02});

	dsv.setVec4("objectColor", {p->color.x, p->color.y, p->color.z, 0});
	dsv.setMat4("model", model);
	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void DebugRenderer::updateCamera(glm::mat4 projection, glm::mat4 view)
{
	Renderer::shaderLibInstance->getCache().SwitchShader(dsv.getProgram());
	dsv.setMat4("projection", projection);
	dsv.setMat4("view", view);
}

void DebugRenderer::renderPoints()
{
	for(auto& i : pointsToRender)
		renderDebugPoint(i.point, i.color);
}

void DebugRenderer::renderAABB()
{
	for(auto& i : aabbToRender)
		renderDebugCube({i.center.x + i.size.x/2, i.center.y + i.size.y/2, i.center.z - i.size.z/2}, i.size.x, i.size.y, i.size.z);
}

void DebugRenderer::clearPoints()
{
	pointsToRender.clear();
	pointsToRender.shrink_to_fit();
}

Renderer::Renderer(Scene* currScene_in, GameState* instance, Window* wind_in) : currScene(currScene_in), wind(wind_in) {
	//NOTE(darius) this leak doesnt matter
	shaderLibInstance = new ShaderLibrary();

	FlatMesh flat;
	m.addMesh(flat);
    
	//OpenglWrapper::EnableDepthTest();
	state.EnableDepthTesting();

	framebuffer.AttachMultisampledTexture(wind->getWidth(), wind->getHeight());
	framebuffer.Unbind();

	deferredLightingBuffer.AttachTexture(wind->getWidth(), wind->getHeight(), 3);
	bloomBuffer.AttachTexture(wind->getWidth(), wind->getHeight(), 2);
	//NOTE(darius) make it two buffers, not two color attachments
	pingPongBlurBufferA.AttachTexture(wind->getWidth(), wind->getHeight(), 1);
	pingPongBlurBufferB.AttachTexture(wind->getWidth(), wind->getHeight(), 1);
	bokeBuffer.AttachTexture(wind->getWidth(), wind->getHeight(), 1);
	bufferCombination.AttachTexture(wind->getWidth(), wind->getHeight(), 1);
	depthFramebuffer.AttachTexture(wind->getWidth(), wind->getHeight());
	depthTexture.AttachTexture(wind->getWidth(), wind->getHeight());
	//u draw inside of this one
	intermidiateFramebuffer.AttachTexture(wind->getWidth(), wind->getHeight());
	intermidiateFramebuffer.setTaget(GL_DRAW_FRAMEBUFFER);

	OpenglWrapper::UnbindFrameBuffer(GL_FRAMEBUFFER);

	OpenglWrapper::SetWindowSize(wind, 1600, 800);
    resizeFrameBuffers(1600, 800);
}

void Renderer::render(Window* wind)
{
	FrustumCuller::updateFrustum();
	OcclusionCuller::updateOrigin();
	//NOTE(darius) in progress uncomment to test
	//OcclusionCuller::cull(currScene->get_objects());
	OcclusionCuller::rasterizeOccluders(currScene->getObjects());

	Renderer::drawCallsCount = 0;
	Renderer::drawCallsInstancedCount = 0;

    //Timer t;
    //t.setBoolPrint(true);

	int display_w, display_h;
	OpenglWrapper::GetWindowSize(wind, &display_w, &display_h);
	OpenglWrapper::SetWindow(display_w, display_h);
	FrustumCuller::setFrustumSize(static_cast<float>(display_w), static_cast<float>(display_h));

	if(display_w != wind->getWidth() || display_h != wind->getHeight())
	{
		resizeFrameBuffers(display_w, display_h);	
		wind->setWidth(display_w);
		wind->setHeight(display_h);
	}

	shaderLibInstance->checkForShaderReload();

	if(GameState::shadowEnabled)
		depthStage();

	// NOTE(darius) to draw ONLY result of last stage
	// draw it into quad and return from renderer 
	//quad.DrawQuad(shaderLibInstance->depthMap);
	//return;

	//bloomStage();

	if(GameState::editor_mode != 0)
		EditorIDsStage();

	//return;
    
	albedoStage();

	//deferredStage();
	//intermidiateFramebuffer.Blit(framebuffer, intermidiateFramebuffer);

	//bokeStage();

	quad.DrawQuad(intermidiateFramebuffer);
}

void Renderer::deferredStage()
{
	shaderLibInstance->stage = ShaderLibrary::STAGE::DEFERRED;

	deferredLightingBuffer.setTaget(GL_FRAMEBUFFER);
	deferredLightingBuffer.Bind();

	OpenglWrapper::ClearScreen(backgroundColor);
	OpenglWrapper::ClearBuffer();
	OpenglWrapper::ClearDepthBuffer();
	OpenglWrapper::EnableDepthTest();

	renderAll(wind);

	deferredLightingBuffer.Unbind();

	intermidiateFramebuffer.Blit(deferredLightingBuffer, intermidiateFramebuffer);
}

void Renderer::EditorIDsStage() 
{
	//OpenglWrapper::BindFrameBuffer(ObjectSelector::pick_fbo);

	shaderLibInstance->stage = ShaderLibrary::STAGE::EDITOR_ID;

	//TODO(darius) move creation of this buffer to Renderer::Renderer()
	FrameBuffer& workerBuff = ObjectSelector::buff;

	workerBuff.setTaget(GL_FRAMEBUFFER);
	workerBuff.Bind();
	
	//NOTE(darius) this order is important!!
	OpenglWrapper::ClearScreen(backgroundColor);
	OpenglWrapper::ClearBuffer();
	OpenglWrapper::ClearDepthBuffer();
	OpenglWrapper::EnableDepthTest();

	//TODO(darius) wrap it
	//shaderLibInstance->loadCurrentShader();
	renderScene();

	workerBuff.Unbind();

	//NOTE(darius) for debug 
	//intermidiateFramebuffer.Blit(workerBuff, intermidiateFramebuffer);
	//quad.DrawQuad(intermidiateFramebuffer);
}

void Renderer::depthStage()
{
	//Timer t(true);
	shaderLibInstance->stage = ShaderLibrary::STAGE::DEPTH;
	framebuffer.setTaget(GL_FRAMEBUFFER);
	depthFramebuffer.Bind();

	OpenglWrapper::ClearScreen({0.9,0.2,0.9});
	OpenglWrapper::ClearBuffer();
	OpenglWrapper::ClearDepthBuffer();
	OpenglWrapper::EnableDepthTest();

	//shaderLibInstance->loadCurrentShader();

	//glCullFace(GL_FRONT);

	renderScene();

	//glCullFace(GL_BACK);

	depthFramebuffer.Unbind();

	//depthTexture.setTaget(GL_DRAW_FRAMEBUFFER);
	//depthTexture.Bind();
	//depthTexture.Blit();
	shaderLibInstance->depthMap = depthFramebuffer.getTexture().get_texture();
	//shaderLibInstance->depthMap = depthTexture.getTexture().get_texture();
}

void Renderer::albedoStage()
{
	//Timer t(true);
	shaderLibInstance->stage = ShaderLibrary::STAGE::ALBEDO;
	framebuffer.setTaget(GL_FRAMEBUFFER);
	framebuffer.Bind();

	OpenglWrapper::EnableMultisample();
	OpenglWrapper::ClearScreen(backgroundColor);
	OpenglWrapper::ClearBuffer();
	OpenglWrapper::ClearDepthBuffer();
	OpenglWrapper::EnableDepthTest();

	//NOTE(darius) work on that
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   

	renderAll(wind);

    //glDisable(GL_BLEND);

	intermidiateFramebuffer.Blit(framebuffer, intermidiateFramebuffer);
}

void Renderer::bokeStage()
{
	shaderLibInstance->stage = ShaderLibrary::STAGE::BOKE;
	//NOTE(darius) dont work yet
	float far_plane = 5;
	float close_plane = 0;

	//NOTE(darius) 1) Draw objects that are in focus in separate buffer and then blit on blured?
	//			   2) Blur each objcet using its depth buffer value?

	//bokeBuffer.setTaget(GL_FRAMEBUFFER);
	//bokeBuffer.Bind();

	{
		BinderPointer bptr(pingPongBlurBufferA);
		quad.DrawQuad(framebuffer.getTextureAt(0).get_texture());
	}

	Shader boke = shaderLibInstance->getBokeShader();
	OpenglWrapper::UseProgram(boke.getProgram());

	pingPongBlurBufferA.Bind();

	quad.bindQuadVAO();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(
		GL_TEXTURE_2D, intermidiateFramebuffer.getTextureAt(0).get_texture()
	);

	quad.drawArrays();

	pingPongBlurBufferA.Unbind();

	bokeBuffer.setTaget(GL_FRAMEBUFFER);
	bokeBuffer.Bind();

	OpenglWrapper::ClearDepthBuffer();
	OpenglWrapper::EnableDepthTest();

	//bokeBuffer.Unbind();

	bokeBuffer.Blit(pingPongBlurBufferA, bokeBuffer);

	quad.DrawQuad(bokeBuffer);
}

void Renderer::bloomStage()
{
	shaderLibInstance->stage = ShaderLibrary::STAGE::BLOOM;
	bloomBuffer.Blit(intermidiateFramebuffer, bloomBuffer);
	
	//NOTE(daris) currently works
	//quad.DrawQuad(bloomBuffer.getTextureAt(1).get_texture());
	//return;

	bool horizontal = true, first_iteration = true;
	int amount = 10;

	//NOTE(darius) set HDR main
	{
		BinderPointer bptr(pingPongBlurBufferA);
		quad.DrawQuad(bloomBuffer.getTextureAt(0).get_texture());
	}

	//NOTE(darius) set HDR bright
	{
		BinderPointer bptr(&pingPongBlurBufferB);
		quad.DrawQuad(bloomBuffer.getTextureAt(1).get_texture());
	}

	Shader shaderBlur = shaderLibInstance->getBlurShader();
	OpenglWrapper::UseProgram(shaderBlur.getProgram());

	//TODO(darius) pingpong it here
	pingPongBlurBufferB.Bind();
	for (int i = 0; i < amount; ++i) {
		quad.bindQuadVAO();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(
			GL_TEXTURE_2D, pingPongBlurBufferB.getTextureAt(0).get_texture()
		);

		shaderBlur.setInt("horizontal", i % 2);

		quad.drawArrays();
	}

	pingPongBlurBufferB.Unbind();

	framebuffer.setTaget(GL_FRAMEBUFFER);
	framebuffer.Bind();

	OpenglWrapper::ClearDepthBuffer();
	OpenglWrapper::EnableDepthTest();

	Shader shad = shaderLibInstance->getTextureCombinerShader();
	OpenglWrapper::UseProgram(shad.getProgram());

	quad.bindQuadVAO();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(
		GL_TEXTURE_2D, pingPongBlurBufferA.getTextureAt(0).get_texture()
	);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(
		GL_TEXTURE_2D, pingPongBlurBufferB.getTextureAt(0).get_texture()
	);

	quad.drawArrays();
}

void Renderer::resizeFrameBuffers(int W, int H)
{
	framebuffer.Resize(W, H);
	deferredLightingBuffer.Resize(W, H);
	bloomBuffer.Resize(W, H);
	pingPongBlurBufferA.Resize(W, H);
	pingPongBlurBufferB.Resize(W, H);
	depthFramebuffer.Resize(W, H);
	intermidiateFramebuffer.Resize(W, H);
	ObjectSelector::buff.Resize(W, H);
}

void Renderer::resizeWindow(int W, int H)
{
	//NOTE(darius) https://github.com/ocornut/imgui/issues/2442
	OpenglWrapper::SetWindowSize(wind, W, H);
    resizeFrameBuffers(W, H);
    wind->setWidth(W);
	wind->setHeight(H);
}

void Renderer::renderAll(Window* wind)
{
	//Timer t(true);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	if (true || GameState::instance->cam.cursor_hidden) {
		projection = GameState::instance->cam.getPerspective(wind->getWidth(), wind->getHeight());
		view = GameState::instance->cam.getBasicLook();
	}

	if(dbr.debug_render)
		dbr.renderDebugGrid();

	//TODO(darius) optimize here
	//shaderLibInstance->loadCurrentShader();
	renderScene();

	if (!dbr.debug_render)
		return;

	if (true || GameState::instance->cam.cursor_hidden) {
		dbr.updateCamera(projection, view);
	}

	for (int i = 0; i < currScene->getObjects().size(); ++i) {
		if (currScene->getObjects()[i]) {
			dbr.renderDebugColider(wind, currScene->get_object_at(i)->getColider(),
				currScene->get_object_at(i)->getRigidBody());
			dbr.renderDebugLightSource(currScene->getObjects()[i]->getPointLight());
			currScene->getObjects()[i]->traverseObjects([&dbr = dbr](Object* obj) {
				dbr.renderDebugLightSource(obj->getPointLight());
				});
		}
	}

	//dbr.renderDebugLine({0,0,0}, {1,1,1});

	dbr.renderDebugPoint(DirectionalLight::lightPos, {1,0,0,0});

	//dbr.renderPoints();
	dbr.renderAABB();
}

void Renderer::renderScene() 
{
	float currentFrame = static_cast<float>(glfwGetTime());
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	//NOTE(darius) 50%+ perfomance boost, but may work weird
	//NOTE(darius) didnt saw any perfomance boost on heavy scene. Also dont work with cubeMeshes(prblbly wrong indices)
	//OpenglWrapper::CullFaces();

	currScene->renderScene();
	Renderer::shaderLibInstance->stage = ShaderLibrary::STAGE::PARTICLES;
	currScene->renderParticles();

	currScene->updateAnimators(deltaTime);
	currScene->updateSpriteAnimations(static_cast<float>(glfwGetTime()));

	//currScene->updateScene();
}

void Renderer::updateBuffers(Window* wind)
{
    glfwSwapBuffers(wind->getWindow());
	glfwPollEvents();
}

size_t Renderer::getShaderRoutine()
{
    return getShader().getProgram();
}

Shader Renderer::getShader()
{
    return Renderer::shaderLibInstance->getCurrShader();
}

Scene* Renderer::getScene()
{
	return currScene;
}

DebugRenderer& Renderer::getDebugRenderer()
{
    return dbr;
}


ShaderLibrary* Renderer::shaderLibInstance = nullptr;
Renderer* Renderer::currentRendererPtr = nullptr;

int Renderer::drawCallsCount = 0;
int Renderer::drawCallsInstancedCount = 0;

