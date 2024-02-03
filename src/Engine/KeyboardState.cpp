#include "KeyboardState.h"
#include <GLFW/glfw3.h>

void KeyboardState::set_w(bool state) {
	m_keys[GLFW_KEY_W] = state;
}

void KeyboardState::set_a(bool state) {
	m_keys[GLFW_KEY_A] = state;
}

void KeyboardState::set_s(bool state) {
	m_keys[GLFW_KEY_S] = state;
}

void KeyboardState::set_d(bool state) {
	m_keys[GLFW_KEY_D] = state;
}

void KeyboardState::set_q(bool state) {
	m_keys[GLFW_KEY_Q] = state;
}

void KeyboardState::set_e(bool state) {
	m_keys[GLFW_KEY_E] = state;
}

void KeyboardState::set_c(bool state) {
	m_keys[GLFW_KEY_C] = state;
}

void KeyboardState::set_l(bool state) {
	m_keys[GLFW_KEY_L] = state;
}

void KeyboardState::set_lshift(bool state) {
	m_keys[GLFW_KEY_LEFT_SHIFT] = state;
}

void KeyboardState::set_mouse_right_button(bool state) {
	m_keys[GLFW_MOUSE_BUTTON_RIGHT] = state;
}

void KeyboardState::set_mouse_left_button(bool state) {
	m_keys[GLFW_MOUSE_BUTTON_LEFT] = state;
}

void KeyboardState::set_0(bool state) {
	m_keys[GLFW_KEY_0] = state;
}

void KeyboardState::set_1(bool state) {
	m_keys[GLFW_KEY_1] = state;
}

void KeyboardState::set_2(bool state) {
	m_keys[GLFW_KEY_2] = state;
}

void KeyboardState::set_3(bool state) {
	m_keys[GLFW_KEY_3] = state;
}

void KeyboardState::set_4(bool state) {
	m_keys[GLFW_KEY_4] = state;
}

void KeyboardState::set_9(bool state) {
	m_keys[GLFW_KEY_9] = state;
}

void KeyboardState::set_cntrl(bool state) {
	m_keys[GLFW_KEY_LEFT_CONTROL] = state;
}

bool KeyboardState::get_cntrl() {
	return m_keys[GLFW_KEY_LEFT_CONTROL];
}

bool KeyboardState::get_w() {
	return m_keys[GLFW_KEY_W];
}

bool KeyboardState::get_a() {
	return m_keys[GLFW_KEY_A];
}

bool KeyboardState::get_s() {
	return m_keys[GLFW_KEY_S];
}

bool KeyboardState::get_d() {
	return m_keys[GLFW_KEY_D];
}

bool KeyboardState::get_e() {
	return m_keys[GLFW_KEY_E];
}

bool KeyboardState::get_c()
{
	return m_keys[GLFW_KEY_C];
}

bool KeyboardState::get_l()
{
	return m_keys[GLFW_KEY_L];
}


bool KeyboardState::get_lshift()
{
	return m_keys[GLFW_KEY_LEFT_SHIFT];
}

bool KeyboardState::get_q() {
	return m_keys[GLFW_KEY_Q];
}

bool KeyboardState::get_0() {
	return m_keys[GLFW_KEY_0];
}

bool KeyboardState::get_1() {
	return m_keys[GLFW_KEY_1];
}

bool KeyboardState::get_2() {
	return m_keys[GLFW_KEY_2];
}

bool KeyboardState::get_3() {
	return m_keys[GLFW_KEY_3];
}

bool KeyboardState::get_4() {
	return m_keys[GLFW_KEY_4];
}

bool KeyboardState::get_9() {
	return m_keys[GLFW_KEY_9];
}

bool KeyboardState::get_mouse_right_button()
{
	return m_keys[GLFW_MOUSE_BUTTON_RIGHT];
}

bool KeyboardState::get_mouse_left_button()
{
	return m_keys[GLFW_MOUSE_BUTTON_LEFT];
}
