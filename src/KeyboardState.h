#pragma once
#include <atomic>

class KeyboardState
{
public:
	void set_w(bool state) {
		w_pressed.store(state);
	}
	
	void set_a(bool state) {
		a_pressed.store(state);
	}
	
	void set_s(bool state) {
		s_pressed.store(state);
	}
	
	void set_d(bool state) {
		d_pressed.store(state);
	}

	bool get_w() {
		return w_pressed.load();
	}

	bool get_a() {
		return a_pressed.load();
	}

	bool get_s() {
		return s_pressed.load();
	}

	bool get_d() {
		return d_pressed.load();
	}

private:
	std::atomic_bool w_pressed;
	std::atomic_bool a_pressed;
	std::atomic_bool s_pressed;
	std::atomic_bool d_pressed;
};
