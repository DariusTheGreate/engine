#include "GameState.h"

#include <cstdlib>

static std::string resolveEnginePath()
{
	if (const char* env = std::getenv("DEAN_ENGINE_ROOT"))
		if (env[0] != '\0')
			return std::string(env);
#ifdef DEAN_ENGINE_ROOT
	return std::string(DEAN_ENGINE_ROOT);
#else
	return std::string("E:/own/programming/engine/");
#endif
}

void GameState::msg(std::string&& str)
{
	//if (debug_msg.size() >= debug_len)
	//	clear_msg();
	debug_msg.append(std::move(str));
}

void GameState::clear_msg()
{
	debug_msg.clear();
}

void GameState::set_instance(GameState* ptr)
{
	GameState::instance = ptr;
}

GameState* GameState::get_instance()
{
	return instance;
}

void GameState::setEditorCameraMode()
{
	if(!editorCamera)
		return;

	GameState::instance->cam = *editorCamera;
}

void GameState::saveActiveCameraStateIntoEditorCamera()
{
	if(!editorCamera)
		return;
	*editorCamera = GameState::instance->cam;
}

void GameState::setCameraMotionPoints(const CameraPoints& points)
{
	GameState::cameraMotionPoints = points;
}

//std::string GameState::debug_msg;
//size_t GameState::debug_len = 100;

int GameState::editor_mode = 2;//0 - no UI
Camera* GameState::editorCamera = nullptr;
bool GameState::editorCameraMode = true;
CameraPoints GameState::cameraMotionPoints;
GameState* GameState::instance = nullptr;
Timer GameState::currTime;

std::string GameState::engine_path = resolveEnginePath();

float GameState::gammaBrightness = 3.0f;

float GameState::gammaFactor = 1.0f;

//Camera GameState::cam;
MouseState GameState::ms;

bool GameState::cullEnabled = true;
bool GameState::occlusionCullEnabled = true;
bool GameState::shadowEnabled = true;
//KeyboardState GameState::ks;
