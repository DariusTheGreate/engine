#pragma once

#include <Shader.h>
#include <Transform.h>

class Animator;

class SkeletalAnimationShaderRoutine
{
public:
	SkeletalAnimationShaderRoutine()  = default;
	SkeletalAnimationShaderRoutine(Shader&& sv_in, Animator* anim) : sv(sv_in), animator(anim) {}
	SkeletalAnimationShaderRoutine(Shader&& sv_in) : sv(sv_in), animator(nullptr) { }

	void operator() (Transform tr);
private:
	Shader sv;
	Animator* animator = nullptr;
};

