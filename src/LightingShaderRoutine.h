#pragma once
#include <optional>

#include <Shader.h>
#include <PointLight.h>
#include <Color.h>
#include <GameState.h>

class LightingShaderRoutine
{
public:
	LightingShaderRoutine()  = default;
	LightingShaderRoutine(Shader&& sv_in, DirectionalLight&& dl_in, PointLight&& pl_in, Material&& mat_in) : sv(sv_in), directionalLight(dl_in), pointLight(pl_in)
	{
	}

	LightingShaderRoutine(Shader&& sv_in) : sv(sv_in) { 
	}

	void operator() (Transform tr){
		sv.setVec3("viewPos", GameState::cam.getCameraPos());
        sv.setInt("lightsCount", PointLight::LightsCount);
        sv.setFloat("gammaFactor", 1); 

        if(directionalLight)
	        directionalLight->setShaderLight(sv);
	    if(pointLight)
	        pointLight->setShaderLight(sv);

        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 pos = tr.position;
        glm::mat4 q = tr.get_quatmat();
        glm::vec3 scale = tr.scale;

        model = glm::translate(model, pos);
        model *= q;
        model = glm::scale(model, scale);

        sv.setMat4("model", model);
	}

	std::optional<PointLight>& getPointLight()
	{
		return pointLight;
	}

private:
	Shader sv;
	std::optional<DirectionalLight> directionalLight;
	std::optional<PointLight> pointLight;
};
