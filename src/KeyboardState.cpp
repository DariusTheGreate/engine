#include "KeyboardState.h"

void KeyboardState::set_w(bool state) {
	w_pressed.store(state);
}

void KeyboardState::set_a(bool state) {
	a_pressed.store(state);
}

void KeyboardState::set_s(bool state) {
	s_pressed.store(state);
}

void KeyboardState::set_d(bool state) {
	d_pressed.store(state);
}

void KeyboardState::set_q(bool state) {
	q_pressed.store(state);
}

void KeyboardState::set_e(bool state) {
	e_pressed.store(state);
}

void KeyboardState::set_mouse_right_button(bool state)
{
	mouse_right_button_pressed.store(state);
}

void KeyboardState::set_mouse_left_button(bool state)
{
	mouse_left_button_pressed.store(state);
}

void KeyboardState::set_0(bool state) {
	pressed_0.store(state);
}

void KeyboardState::set_1(bool state) {
	pressed_1.store(state);
}

void KeyboardState::set_2(bool state) {
	pressed_2.store(state);
}

void KeyboardState::set_3(bool state) {
	pressed_3.store(state);
}

void KeyboardState::set_4(bool state) {
	pressed_4.store(state);
}

void KeyboardState::set_9(bool state) {
	pressed_9.store(state);
}

bool KeyboardState::get_w() {
	return w_pressed.load();
}

bool KeyboardState::get_a() {
	return a_pressed.load();
}

bool KeyboardState::get_s() {
	return s_pressed.load();
}

bool KeyboardState::get_d() {
	return d_pressed.load();
}

bool KeyboardState::get_e() {
	return e_pressed.load();
}

bool KeyboardState::get_q() {
	return q_pressed.load();
}

bool KeyboardState::get_0() {
	return pressed_0.load();
}

bool KeyboardState::get_1() {
	return pressed_1.load();
}

bool KeyboardState::get_2() {
	return pressed_2.load();
}

bool KeyboardState::get_3() {
	return pressed_3.load();
}

bool KeyboardState::get_4() {
	return pressed_4.load();
}

bool KeyboardState::get_9() {
	return pressed_9.load();
}

bool KeyboardState::get_mouse_right_button()
{
	return mouse_right_button_pressed.load();
}

bool KeyboardState::get_mouse_left_button()
{
	return mouse_left_button_pressed.load();
}

