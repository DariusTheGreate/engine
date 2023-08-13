#include <SkeletalAnimationShaderRoutine.h>

#include <Animator.h>
#include <glm/glm.hpp>

void SkeletalAnimationShaderRoutine::operator() (Transform tr){
    if(animator){
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
        for (int i = 0; i < transforms.size(); ++i)
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
}

