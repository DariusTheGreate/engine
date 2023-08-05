#include <ObjectSelector.h>
#include <FrameBuffer.h>

ObjectSelector::ObjectSelector(int W, int H)
{
    /*OpenglWrapper::GenerateFrameBuffers(&pick_fbo);
    OpenglWrapper::BindFrameBuffer(static_cast<int>(pick_fbo));
    OpenglWrapper::GenerateTextures(&pick_texture);
    OpenglWrapper::BindTexture(static_cast<int>(pick_texture));
    OpenglWrapper::ImageTexture(GL_DEPTH_COMPONENT, W, H, nullptr, GL_TEXTURE_2D, GL_FLOAT);
    OpenglWrapper::ImageFrameBuffer(static_cast<int>(pick_texture));
    OpenglWrapper::UnbindTexture();
    OpenglWrapper::UnbindFrameBuffer();
    */
    buff.Generate();
    buff.AttachTexture(W,H);
}

int ObjectSelector::ReadPixel(int x, int y)
{
    //std::cout << x << "|" << y << "\n";
    //OpenglWrapper::BindFrameBuffer(static_cast<int>(pick_fbo), GL_READ_BUFFER);
    //buff.setTaget(GL_READ_FRAMEBUFFER);

    buff.setTaget(GL_READ_FRAMEBUFFER);
    buff.Bind();

    OpenglWrapper::ReadBuffer();
    float pixel[4] = { 0, 0, 0, 0 };
    OpenglWrapper::ReadPixels(x, y, &pixel);

    buff.Unbind();

    pixel[0] *= 255;
    pixel[1] *= 255;
    pixel[2] *= 255;
    pixel[3] *= 255;

    /*int pickedID =
        pixel[0] +
        pixel[1] * 256 +
        pixel[2] * 256 * 256;
    
    pickedID /= 25500;
    */
    
    float pickedID = pixel[0];

    //std::cout << pixel[0] << " " << pixel[1] << " " << pixel[2] << " " << pixel[3] << "\n";
    std::cout << pickedID << "\n";
    OpenglWrapper::ReadZeroBuffer();

    //buff.setTaget(GL_FRAMEBUFFER);
    //GameState::msg("Pixel data: " + std::to_string(pixel[0]) + "|" + std::to_string(pixel[1]) + "|" + std::to_string(pixel[2]) + "|" + std::to_string(pixel[3]) + "\n");

    return pickedID;
}

glm::vec3 ObjectSelector::GetRayFromMouse(float mouseX, float mouseY, Window* w)
{
    mouseX = (2.0f * mouseX) / static_cast<float>(w->getWidth()) - 1.0f;
    mouseY = (2.0f * mouseY) / static_cast<float>(w->getHeight()) - 1.0f;

    glm::vec2 ray_nds = glm::vec2(mouseX, mouseY);
    glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
    glm::mat4 invProjMat = glm::inverse(GameState::cam.getPerspective(w->getWidth(), w->getHeight()));
    glm::vec4 eyeCoords = invProjMat * ray_clip;
    eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
    glm::mat4 invViewMat = glm::inverse(GameState::cam.getBasicLook());
    glm::vec4 rayWorld = invViewMat * eyeCoords;
    glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

    //std::cout << "Ray: " << rayDirection.x << "|" << rayDirection.y << "|" << rayDirection.z << "\n";

    return rayDirection;
}

bool ObjectSelector::RaySphere(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 position, float r, Renderer* renderer)
{

    glm::vec3 v = glm::vec3(position.x, position.y, position.z) - ray_origin;

    renderer->getDebugRenderer().pointsToRender.push_back({ v, glm::vec4{0,0,1,0} });

    float dist = 100;
    return glm::intersectRaySphere(ray_origin, ray_direction, position, r, dist);

    float a = glm::dot(ray_direction, ray_direction);
    float b = 2 * glm::dot(v, ray_direction);
    float c = glm::dot(v, v) - r * r;

    float b_squared_minus_4ac = b * b - 4 * a * c;

    if (b_squared_minus_4ac == 0)
        return true;
    if (b_squared_minus_4ac > 0)
    {
        float x1 = (-b - sqrt(b_squared_minus_4ac)) / 2.0f;
        float x2 = (-b + sqrt(b_squared_minus_4ac)) / 2.0f;
        if (x1 >= 0.0f && x2 >= 0.0f)
            return true;
        if (x1 < 0.0f && x2 >= 0.0f)
            return true;
    }

    return false;
}

void ObjectSelector::ProbeSceneObjects(Scene* scene, float mouseX, float mouseY, Window* w, Renderer* renderer)
{
    //std::cout << "Coords: " << mouseX << "|" << mouseY << "\n";
    //GameState::msg("Coords: " + std::to_string(mouseX) + "|" + std::to_string(mouseY) + "\n");
    auto& objects = scene->get_objects();

    auto ray = GetRayFromMouse(mouseX, mouseY, w);
    //render->getDebugRenderer().renderDebugPoint(ray);
    //render->getDebugRenderer().renderDebugPoint({0,0,0});
    renderer->getDebugRenderer().pointsToRender.push_back(ray);
    renderer->getDebugRenderer().pointsToRender.push_back(glm::vec3{ 0,0,0 });

    for (size_t i = 0; i < objects.size(); ++i)
    {
        if (RaySphere(GameState::cam.getCameraPos(), ray, objects[i]->getTransform().position, 3, renderer)) {
            std::cout << "clicked " << objects[i]->get_name() << "\n";
            //msg("clicked: " +objects[i]->get_name());
        }
    }
}

size_t ObjectSelector::pick_fbo = 0;
unsigned int ObjectSelector::pick_texture= 0;

FrameBuffer ObjectSelector::buff = true;
