#pragma once
#include <atomic>

class KeyboardState
{
public:
	//TODO(darius) make macro for this shit
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
	
	void set_q(bool state) {
		q_pressed.store(state);
	}
	
	void set_e(bool state) {
		e_pressed.store(state);
	}

	void set_mouse_right_button(bool state)
	{
		mouse_right_button_pressed.store(state);
	}

	void set_mouse_left_button(bool state)
	{
		mouse_left_button_pressed.store(state);
	}
	
	void set_0(bool state) {
		pressed_0.store(state);
	}

	void set_1(bool state) {
		pressed_1.store(state);
	}

	void set_2(bool state) {
		pressed_2.store(state);
	}

	void set_3(bool state) {
		pressed_3.store(state);
	}

	void set_4(bool state) {
		pressed_4.store(state);
	}

	void set_9(bool state) {
		pressed_9.store(state);
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

	bool get_e() {
		return e_pressed.load();
	}

	bool get_q() {
		return q_pressed.load();
	}

	bool get_0() {
		return pressed_0.load();
	}

	bool get_1() {
		return pressed_1.load();
	}

	bool get_2() {
		return pressed_2.load();
	}

	bool get_3() {
		return pressed_3.load();
	}

	bool get_4() {
		return pressed_4.load();
	}

	bool get_9() {
		return pressed_9.load();
	}

	bool get_mouse_right_button()
	{
		return mouse_right_button_pressed.load();
	}

	bool get_mouse_left_button()
	{
		return mouse_left_button_pressed.load();
	}

private:
	std::atomic_bool w_pressed;
	std::atomic_bool a_pressed;
	std::atomic_bool s_pressed;
	std::atomic_bool d_pressed;
	std::atomic_bool q_pressed;
	std::atomic_bool e_pressed;

	std::atomic_bool pressed_0;
	std::atomic_bool pressed_1;
	std::atomic_bool pressed_2;
	std::atomic_bool pressed_3;
	std::atomic_bool pressed_4;
	std::atomic_bool pressed_9;
	std::atomic_bool mouse_right_button_pressed;
	std::atomic_bool mouse_left_button_pressed;
};
