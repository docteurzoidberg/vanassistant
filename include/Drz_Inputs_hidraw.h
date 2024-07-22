#pragma once

#include "IDrz_Inputs.h"
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <sys/ioctl.h>
#include <errno.h>
#include <map>

namespace drz {

class Drz_Inputs_hidraw : public IDrzInputs {
public:
	Drz_Inputs_hidraw() {
		CreateKeyMap();
	}

	~Drz_Inputs_hidraw() override {
		Close();
	}

	bool Setup() override {
		fd = open("/dev/input/event0", O_RDONLY | O_NONBLOCK);
		if (fd < 0) {
			std::cerr << "Failed to open input device: " << strerror(errno) << std::endl;
			return false;
		}
		return true;
	}

	void Update() override {
		struct input_event ev;
		while (read(fd, &ev, sizeof(ev)) > 0) {
			if (ev.type == EV_KEY && ev.code < 256) {
				auto keyIter = mapKeys.find(ev.code);
				if (keyIter != mapKeys.end()) {
					Key key = keyIter->second;
					UpdateKeyState(static_cast<int>(key), ev.value);
				}
			}
		}
		ScanHardware(pKeyboardState, pKeyOldState, pKeyNewState, 256);
	}

	hwbutton GetKey(Key k) override {
		return pKeyboardState[static_cast<int>(k)];
	}

	bool Close() override {
		if (fd >= 0) {
			close(fd);
			fd = -1;
		}
		return true;
	}

private:
	int fd = -1;

	bool pKeyNewState[256] = {0};
	bool pKeyOldState[256] = {0};
	hwbutton pKeyboardState[256] = {0};

	std::map<int, Key> mapKeys;

	void CreateKeyMap() {
		mapKeys[0x00] = Key::NONE;
		mapKeys[KEY_A] = Key::A; mapKeys[KEY_B] = Key::B; mapKeys[KEY_C] = Key::C; mapKeys[KEY_D] = Key::D; mapKeys[KEY_E] = Key::E;
		mapKeys[KEY_F] = Key::F; mapKeys[KEY_G] = Key::G; mapKeys[KEY_H] = Key::H; mapKeys[KEY_I] = Key::I; mapKeys[KEY_J] = Key::J;
		mapKeys[KEY_K] = Key::K; mapKeys[KEY_L] = Key::L; mapKeys[KEY_M] = Key::M; mapKeys[KEY_N] = Key::N; mapKeys[KEY_O] = Key::O;
		mapKeys[KEY_P] = Key::P; mapKeys[KEY_Q] = Key::Q; mapKeys[KEY_R] = Key::R; mapKeys[KEY_S] = Key::S; mapKeys[KEY_T] = Key::T;
		mapKeys[KEY_U] = Key::U; mapKeys[KEY_V] = Key::V; mapKeys[KEY_W] = Key::W; mapKeys[KEY_X] = Key::X; mapKeys[KEY_Y] = Key::Y;
		mapKeys[KEY_Z] = Key::Z;

		mapKeys[KEY_F1] = Key::F1; mapKeys[KEY_F2] = Key::F2; mapKeys[KEY_F3] = Key::F3; mapKeys[KEY_F4] = Key::F4;
		mapKeys[KEY_F5] = Key::F5; mapKeys[KEY_F6] = Key::F6; mapKeys[KEY_F7] = Key::F7; mapKeys[KEY_F8] = Key::F8;
		mapKeys[KEY_F9] = Key::F9; mapKeys[KEY_F10] = Key::F10; mapKeys[KEY_F11] = Key::F11; mapKeys[KEY_F12] = Key::F12;

		mapKeys[KEY_DOWN] = Key::DOWN; mapKeys[KEY_LEFT] = Key::LEFT; mapKeys[KEY_RIGHT] = Key::RIGHT; mapKeys[KEY_UP] = Key::UP;

		mapKeys[KEY_BACKSPACE] = Key::BACK; mapKeys[KEY_ESC] = Key::ESCAPE; mapKeys[KEY_ENTER] = Key::ENTER; mapKeys[KEY_PAUSE] = Key::PAUSE;
		mapKeys[KEY_SCROLLLOCK] = Key::SCROLL; mapKeys[KEY_TAB] = Key::TAB; mapKeys[KEY_DELETE] = Key::DEL; mapKeys[KEY_HOME] = Key::HOME;
		mapKeys[KEY_END] = Key::END; mapKeys[KEY_PAGEUP] = Key::PGUP; mapKeys[KEY_PAGEDOWN] = Key::PGDN; mapKeys[KEY_INSERT] = Key::INS;
		mapKeys[KEY_LEFTSHIFT] = Key::SHIFT; mapKeys[KEY_LEFTCTRL] = Key::CTRL; mapKeys[KEY_RIGHTSHIFT] = Key::SHIFT; mapKeys[KEY_RIGHTCTRL] = Key::CTRL;

		mapKeys[KEY_SPACE] = Key::SPACE;

		mapKeys[KEY_0] = Key::K0; mapKeys[KEY_1] = Key::K1; mapKeys[KEY_2] = Key::K2; mapKeys[KEY_3] = Key::K3; mapKeys[KEY_4] = Key::K4;
		mapKeys[KEY_5] = Key::K5; mapKeys[KEY_6] = Key::K6; mapKeys[KEY_7] = Key::K7; mapKeys[KEY_8] = Key::K8; mapKeys[KEY_9] = Key::K9;

		mapKeys[KEY_KP0] = Key::NP0; mapKeys[KEY_KP1] = Key::NP1; mapKeys[KEY_KP2] = Key::NP2; mapKeys[KEY_KP3] = Key::NP3; mapKeys[KEY_KP4] = Key::NP4;
		mapKeys[KEY_KP5] = Key::NP5; mapKeys[KEY_KP6] = Key::NP6; mapKeys[KEY_KP7] = Key::NP7; mapKeys[KEY_KP8] = Key::NP8; mapKeys[KEY_KP9] = Key::NP9;
		mapKeys[KEY_KPASTERISK] = Key::NP_MUL; mapKeys[KEY_KPPLUS] = Key::NP_ADD; mapKeys[KEY_KPSLASH] = Key::NP_DIV; mapKeys[KEY_KPMINUS] = Key::NP_SUB; mapKeys[KEY_KPDOT] = Key::NP_DECIMAL;
	}

	void ScanHardware(hwbutton* pKeys, bool* pStateOld, bool* pStateNew, uint32_t nKeyCount) {
		for (uint32_t i = 0; i < nKeyCount; i++) {
			pKeys[i].bPressed = false;
			pKeys[i].bReleased = false;
			if (pStateNew[i] != pStateOld[i]) {
				if (pStateNew[i]) {
					pKeys[i].bPressed = !pKeys[i].bHeld;
					pKeys[i].bHeld = true;
				} else {
					pKeys[i].bReleased = true;
					pKeys[i].bHeld = false;
				}
			}
			pStateOld[i] = pStateNew[i];
		}
	}

	void UpdateKeyState(int key, int value) {
		if (value == 1) { // Key Pressed
			pKeyNewState[key] = true;
			pKeyboardState[key].bPressed = true;
			pKeyboardState[key].bHeld = true;
		} else if (value == 0) { // Key Released
			pKeyNewState[key] = false;
			pKeyboardState[key].bReleased = true;
			pKeyboardState[key].bHeld = false;
		}

		// Handle modifier keys
		if (key == KEY_LEFTSHIFT || key == KEY_RIGHTSHIFT) {
			UpdateModifierState(Key::SHIFT, value);
		} else if (key == KEY_LEFTCTRL || key == KEY_RIGHTCTRL) {
			UpdateModifierState(Key::CTRL, value);
		} else if (key == KEY_LEFTALT || key == KEY_RIGHTALT) {
			UpdateModifierState(Key::ALT, value);
		}
	}

	void UpdateModifierState(Key modifierKey, bool state) {
		pKeyNewState[static_cast<int>(modifierKey)] = state;
	}
};

} // namespace drz
