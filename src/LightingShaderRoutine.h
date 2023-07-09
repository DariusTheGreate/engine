#pragma once
#include <optional>

#include <Shader.h>
#include <PointLight.h>
#include <Color.h>
#include <GameState.h>
#include <Transform.h>

class LightingShaderRoutine
{
public:
	LightingShaderRoutine()  = default;
	LightingShaderRoutine(Shader&& sv_in, DirectionalLight&& dl_in, Material&& mat_in) : sv(sv_in), directionalLight(dl_in) {}

	LightingShaderRoutine(Shader&& sv_in) : sv(sv_in) {}

	void operator() (Transform tr);
private:
	Shader sv;
	std::optional<DirectionalLight> directionalLight;
};
