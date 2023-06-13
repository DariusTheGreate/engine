#include <SkeletalAnimationShaderRoutine.h>

#include <Animator.h>
#include <glm/glm.hpp>

void SkeletalAnimationShaderRoutine::operator() (Transform tr){
    if(animator){
	    auto transforms = animator->GetFinalBoneMatrices();

        auto tr = glm::mat4(1.0f);
        glm::mat4 ViewRotateX = glm::rotate(
            tr,
            3.14f,
            glm::vec3(-1.0f, 0.0f, 0.0f)
        );
        for (int i = 0; i < transforms.size(); ++i)
            sv.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", ViewRotateX);
    }

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 pos = tr.position;
    glm::mat4 q = tr.get_quatmat();
    glm::vec3 scale = tr.scale;

    model = glm::translate(model, pos);
    model *= q;
    model = glm::scale(model, scale);

    sv.setMat4("model", model);
}

