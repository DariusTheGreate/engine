#include "GameState.h"

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

//std::string GameState::debug_msg;
//size_t GameState::debug_len = 100;

int GameState::editor_mode = 2;//0 - no UI
Camera* GameState::editorCamera = nullptr;
bool GameState::editorCameraMode = true;
GameState* GameState::instance = nullptr;

std::string GameState::engine_path = "E:/Dean/fedor/engine/";

float GameState::gammaBrightness = 3.0f;

float GameState::gammaFactor = 1.0f;

//Camera GameState::cam;
MouseState GameState::ms;

bool GameState::cullEnabled = true;
bool GameState::shadowEnabled = true;
//KeyboardState GameState::ks;
