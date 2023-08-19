#pragma once
#include <atomic>

class KeyboardState
{
public:
	//TODO(darius) make macro for this shit
	void set_w(bool state);
	
	void set_a(bool state);
	
	void set_s(bool state);
	
	void set_d(bool state);
	
	void set_q(bool state);
	
	void set_e(bool state);

	void set_c(bool state);

	void set_l(bool state);

	void set_cntrl(bool state);

	void set_mouse_right_button(bool state);

	void set_mouse_left_button(bool state);

	void set_0(bool state);

	void set_1(bool state);

	void set_2(bool state);

	void set_3(bool state);

	void set_4(bool state);

	void set_9(bool state);

	bool get_w();

	bool get_a();

	bool get_s();

	bool get_d();

	bool get_e();

	bool get_c();

	bool get_l();

	bool get_q();

	bool get_0();

	bool get_1();

	bool get_2();

	bool get_3();

	bool get_4();

	bool get_9();

	bool get_cntrl();

	bool get_mouse_right_button();

	bool get_mouse_left_button();

private:
	std::atomic_bool w_pressed;
	std::atomic_bool a_pressed;
	std::atomic_bool s_pressed;
	std::atomic_bool d_pressed;
	std::atomic_bool q_pressed;
	std::atomic_bool e_pressed;
	std::atomic_bool c_pressed;
	std::atomic_bool l_pressed;

	std::atomic_bool pressed_0;
	std::atomic_bool pressed_1;
	std::atomic_bool pressed_2;
	std::atomic_bool pressed_3;
	std::atomic_bool pressed_4;
	std::atomic_bool pressed_9;
	std::atomic_bool mouse_right_button_pressed;
	std::atomic_bool mouse_left_button_pressed;

	std::atomic_bool pressed_cntrl;
};
