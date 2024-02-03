#pragma once
#include <atomic>
#include<array>
class KeyboardState
{
public:
	void set_w(bool state);

	void set_a(bool state);

	void set_s(bool state);

	void set_d(bool state);

	void set_q(bool state);

	void set_e(bool state);

	void set_c(bool state);

	void set_l(bool state);

	void set_lshift(bool state);

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

	bool get_lshift();

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

	std::array<bool, 349> m_keys;
};