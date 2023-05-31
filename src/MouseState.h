#pragma once
#include <atomic>


class MouseState
{
public:

	void init(int x_pos = 430, int y_pos = 300, int x_off = 0, int y_off = 0, bool left_press = 0, bool right_press = 0) {
		set_x(x_pos);
		set_y(y_pos);

		set_offset_x(x_off);
		set_offset_y(y_off);

		set_left(left_press);
		set_right(right_press);
	}
	
	//TODO(darius) make macro for this shit
	void set_x(int x) {

		curr_x.store(x);
	}

	void set_y(int y) {
		curr_y.store(y);
	}
	
	void set_offset_x(int x) {
		offset_x.store(x);
	}

	void set_offset_y(int y) {
		offset_y.store(y);
	}

	void set_left(bool state) {
		left_pressed.store(state);
	}

	void set_right(bool state) {
		right_pressed.store(state);
	}

	int get_x() {
		return curr_x.load();
	}

	int get_y() {
		return curr_y.load();
	}

	bool get_left() {
		return left_pressed.load();
	}

	bool get_right() {
		return right_pressed.load();
	}

	int get_offset_x() {
		return offset_x.load();
	}

	int get_offset_y() {
		return offset_y.load();
	}


	int prev_x;
	int prev_y;

private:
	//don need atomics at all?..
	std::atomic_int curr_x;
	std::atomic_int curr_y;
	std::atomic_int offset_x;
	std::atomic_int offset_y;
	std::atomic_bool left_pressed;
	std::atomic_bool right_pressed;
};

