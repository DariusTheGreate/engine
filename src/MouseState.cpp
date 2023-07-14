#include "MouseState.h"

void MouseState::init(int x_pos, int y_pos, int x_off, int y_off, bool left_press, bool right_press) {
	set_x(x_pos);
	set_y(y_pos);

	set_offset_x(x_off);
	set_offset_y(y_off);

	set_left(left_press);
	set_right(right_press);
}

//TODO(darius) make macro for this shit
void MouseState::set_x(int x) {

	curr_x.store(x);
}

void MouseState::set_y(int y) {
	curr_y.store(y);
}

void MouseState::set_offset_x(int x) {
	offset_x.store(x);
}

void MouseState::set_offset_y(int y) {
	offset_y.store(y);
}

void MouseState::set_left(bool state) {
	left_pressed.store(state);
}

void MouseState::set_right(bool state) {
	right_pressed.store(state);
}

int MouseState::get_x() {
	return curr_x.load();
}

int MouseState::get_y() {
	return curr_y.load();
}

bool MouseState::get_left() {
	return left_pressed.load();
}

bool MouseState::get_right() {
	return right_pressed.load();
}

int MouseState::get_offset_x() {
	return offset_x.load();
}

int MouseState::get_offset_y() {
	return offset_y.load();
}

