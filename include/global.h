#pragma once

#ifdef _WIN64
#include <Windows.h>
#include <conio.h>
#endif

#ifdef __linux__
#include <unistd.h>
#endif

#include <vector>
#include <iostream>
#include <assert.h>
#include <atomic>
#include <thread>
#include <functional>
#include <deque>
#include <algorithm>
#include <string>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

enum class Stage {
	SNAKE_END,
	SNAKE_MENU,
	SNAKE_GAME,
	SNAKE_SETTINGS,	
	SNAKE_RESTART,
	GAME_CONTINUE,
	NONE_STAGE
};

enum class KeyType {
	ASCII,
	NONE_ASCII
};

enum class Event {
	KEY_ESC,
	KEY_ENTER,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_PAUSE,
	NONE_EVENT
};

enum class SnakeStatus {
	HEADING_UP,
	HEADING_DOWN,
	HEADING_LEFT,
	HEADING_RIGHT,
	DIED
};

enum class SettingPage {
	PAGE_MENU,
	PAGE_HOTKEY,
	PAGE_GLOBAL
};

