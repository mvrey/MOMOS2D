#include <MOMOS/input.h>
#include <MOMOS/momos.h>
#include <array>
#include <limits>

namespace MOMOS {

namespace {

struct KeyState {
	int previous = GLFW_RELEASE;
	int current = GLFW_RELEASE;
	unsigned long long frame = std::numeric_limits<unsigned long long>::max();
};

struct MouseState {
	int previous = GLFW_RELEASE;
	int current = GLFW_RELEASE;
	unsigned long long frame = std::numeric_limits<unsigned long long>::max();
};

constexpr int kMaxKeys = GLFW_KEY_LAST + 1;
constexpr int kMaxMouseButtons = GLFW_MOUSE_BUTTON_LAST + 1;

std::array<KeyState, kMaxKeys> g_key_states;
std::array<MouseState, kMaxMouseButtons> g_mouse_states;

void RefreshKeyState(int key) {
	if (key < 0 || key >= kMaxKeys)
		return;

	KeyState& state = g_key_states[key];
	if (state.frame == MOMOS::frame_counter)
		return;

	state.previous = state.current;
	state.current = glfwGetKey(MOMOS::win, key);
	state.frame = MOMOS::frame_counter;
}

void RefreshMouseState(int button) {
	if (button < 0 || button >= kMaxMouseButtons)
		return;

	MouseState& state = g_mouse_states[button];
	if (state.frame == MOMOS::frame_counter)
		return;

	state.previous = state.current;
	state.current = glfwGetMouseButton(MOMOS::win, button);
	state.frame = MOMOS::frame_counter;
}

bool IsPressedState(int state) {
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

int NormalizeMouseButton(int button_id) {
	return (button_id == 0) ? GLFW_MOUSE_BUTTON_RIGHT : GLFW_MOUSE_BUTTON_LEFT;
}

}

	// -- Keyboard ------------------------------------------

	bool IsKeyPressed(char key) {
		int key_code = static_cast<unsigned char>(key);
		RefreshKeyState(key_code);
		return IsPressedState(g_key_states[key_code].current);
	}


	bool IsKeyDown(char key) {
		int key_code = static_cast<unsigned char>(key);
		RefreshKeyState(key_code);
		const KeyState& state = g_key_states[key_code];
		return IsPressedState(state.current) && !IsPressedState(state.previous);
	}


	bool IsKeyUp(char key) {
		int key_code = static_cast<unsigned char>(key);
		RefreshKeyState(key_code);
		const KeyState& state = g_key_states[key_code];
		return !IsPressedState(state.current) && IsPressedState(state.previous);
	}


	bool IsSpecialKeyPressed(SpecialKey key) {
		int status = glfwGetKey(win, key);
		return (status == GLFW_PRESS);
	}


	bool IsSpecialKeyDown(SpecialKey key) {
		int key_code = static_cast<int>(key);
		RefreshKeyState(key_code);
		const KeyState& state = g_key_states[key_code];
		return IsPressedState(state.current) && !IsPressedState(state.previous);
	}


	bool IsSpecialKeyUp(SpecialKey key) {
		int key_code = static_cast<int>(key);
		RefreshKeyState(key_code);
		const KeyState& state = g_key_states[key_code];
		return !IsPressedState(state.current) && IsPressedState(state.previous);
	}


	char GetNextPressedKey() {
		char rt = last_key_pressed;
		last_key_pressed = -1;
		return rt;
	}


	void ResetBufferdKeyInput() {
		last_key_pressed = -1;
	}


	// -- Mouse ---------------------------------------------

	double MousePositionX() {
		double x, y;
		glfwGetCursorPos(win, &x, &y);
		return x;
	}


	double MousePositionY() {
		double x, y;
		glfwGetCursorPos(win, &x, &y);
		return y;
	}


	double MouseWheelX() {
		double delta = mouse_scroll_x;
		mouse_scroll_x = 0.0;
		return delta;
	}


	double MouseWheelY() {
		double delta = mouse_scroll_y;
		mouse_scroll_y = 0.0;
		return delta;
	}


	bool MouseButtonPressed(int button_id) {
		int button = NormalizeMouseButton(button_id);
		RefreshMouseState(button);
		return g_mouse_states[button].current == GLFW_PRESS;
	}


	bool MouseButtonDown(int button_id) {
		int button = NormalizeMouseButton(button_id);
		RefreshMouseState(button);
		const MouseState& state = g_mouse_states[button];
		return state.current == GLFW_PRESS && state.previous == GLFW_RELEASE;
	}


	bool MouseButtonUp(int button_id) {
		int button = NormalizeMouseButton(button_id);
		RefreshMouseState(button);
		const MouseState& state = g_mouse_states[button];
		return state.previous == GLFW_PRESS && state.current == GLFW_RELEASE;
	}

}