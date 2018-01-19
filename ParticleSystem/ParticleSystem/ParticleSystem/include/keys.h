#pragma once

enum PCDEV_KEY
{
	PCDEV_KEY_SPACE = 32,
	PCDEV_KEY_APOSTROPHE = 39,
	PCDEV_KEY_PLUS = 43,
	PCDEV_KEY_COMMA = 44,
	PCDEV_KEY_MINUS = 45,
	PCDEV_KEY_PERIOD = 46,
	PCDEV_KEY_SLASH = 47,
	PCDEV_KEY_0 = 48,
	PCDEV_KEY_1 = 49,
	PCDEV_KEY_2 = 50,
	PCDEV_KEY_3 = 51,
	PCDEV_KEY_4 = 52,
	PCDEV_KEY_5 = 53,
	PCDEV_KEY_6 = 54,
	PCDEV_KEY_7 = 55,
	PCDEV_KEY_8 = 56,
	PCDEV_KEY_9 = 57,
	PCDEV_KEY_SEMICOLON = 58,
	PCDEV_KEY_EQUAL = 61,
	PCDEV_KEY_A = 65,
	PCDEV_KEY_B = 66,
	PCDEV_KEY_C = 67,
	PCDEV_KEY_D = 68,
	PCDEV_KEY_E = 69,
	PCDEV_KEY_F = 70,
	PCDEV_KEY_G = 71,
	PCDEV_KEY_H = 72,
	PCDEV_KEY_I = 73,
	PCDEV_KEY_J = 74,
	PCDEV_KEY_K = 75,
	PCDEV_KEY_L = 76,
	PCDEV_KEY_M = 77,
	PCDEV_KEY_N = 78,
	PCDEV_KEY_O = 79,
	PCDEV_KEY_P = 80,
	PCDEV_KEY_Q = 81,
	PCDEV_KEY_R = 82,
	PCDEV_KEY_S = 83,
	PCDEV_KEY_T = 84,
	PCDEV_KEY_U = 85,
	PCDEV_KEY_V = 86,
	PCDEV_KEY_W = 87,
	PCDEV_KEY_X = 88,
	PCDEV_KEY_Y = 89,
	PCDEV_KEY_Z = 90,
	PCDEV_KEY_LEFT_BRACKET = 91,
	PCDEV_KEY_BACKSLASH = 92,
	PCDEV_KEY_RIGHT_BRACKET = 93,
	PCDEV_KEY_a = 97,
	PCDEV_KEY_b = 98,
	PCDEV_KEY_c = 99,
	PCDEV_KEY_d = 100,
	PCDEV_KEY_e = 101,
	PCDEV_KEY_f = 102,
	PCDEV_KEY_g = 103,
	PCDEV_KEY_h = 104,
	PCDEV_KEY_i = 105,
	PCDEV_KEY_j = 106,
	PCDEV_KEY_k = 107,
	PCDEV_KEY_l = 108,
	PCDEV_KEY_m = 109,
	PCDEV_KEY_n = 110,
	PCDEV_KEY_o = 111,
	PCDEV_KEY_p = 112,
	PCDEV_KEY_q = 113,
	PCDEV_KEY_r = 114,
	PCDEV_KEY_s = 115,
	PCDEV_KEY_t = 116,
	PCDEV_KEY_u = 117,
	PCDEV_KEY_v = 118,
	PCDEV_KEY_w = 119,
	PCDEV_KEY_x = 120,
	PCDEV_KEY_y = 121,
	PCDEV_KEY_z = 122,
	PCDEV_KEY_ESCAPE,
	PCDEV_KEY_ENTER,
	PCDEV_KEY_TAB,
	PCDEV_KEY_BACKSPACE,
	PCDEV_KEY_INSERT,
	PCDEV_KEY_DELETE,
	PCDEV_KEY_RIGHT,
	PCDEV_KEY_LEFT,
	PCDEV_KEY_DOWN,
	PCDEV_KEY_UP,
	PCDEV_KEY_PAGE_UP,
	PCDEV_KEY_PAGE_DOWN,
	PCDEV_KEY_HOME,
	PCDEV_KEY_END,
	PCDEV_KEY_F1,
	PCDEV_KEY_F2,
	PCDEV_KEY_F3,
	PCDEV_KEY_F4,
	PCDEV_KEY_F5,
	PCDEV_KEY_F6,
	PCDEV_KEY_F7,
	PCDEV_KEY_F8,
	PCDEV_KEY_F9,
	PCDEV_KEY_F10,
	PCDEV_KEY_F11,
	PCDEV_KEY_F12,
	PCDEV_KEY_UNDEFINED = 999,
};

enum PCDEV_MOUSE
{
	PCDEV_MOUSE_BUTTON_LEFT,
	PCDEV_MOUSE_BUTTON_MIDDLE,
	PCDEV_MOUSE_BUTTON_RIGHT,
	PCDEV_MOUSE_UNDEFINED = 999
};

enum PCDEV_KEY_STATE
{
	PCDEV_KEY_STATE_PRESS,
	PCDEV_KEY_STATE_RELEASE
};