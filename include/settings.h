#pragma once
#include "global.h"
#include "base.h"

typedef std::pair<KeyType, int> key;

struct hotkey {
	std::string name;
	Event key_event;
	KeyType type;
	int value;

	key get_key() { return key(type, value); };
	void set_key(key k) { type = k.first; value = k.second; };
};

typedef std::vector<hotkey> hotkey_set;

class HotKeys : public BaseControl {
public:
	HotKeys(uint16_t res);
	HotKeys();

	void init() override;
	snake_vec options();
	snake_vec& menu();
	component& up();
	component& dn();
	component& lt();
	component& rt();
	component& pause();
	hotkey_set& set() {return _set; };

	component& value_component(size_t index) { return _components[_components.size() / 2 + index]; };

	key get_key(size_t index) { return _set[index].get_key(); };
	key get_key(Event e);

	void set_key(size_t index, key k) { _set[index].set_key(k); };
	void set_key(Event e, key k);

	std::string k2str(key k);
	Event k2evt(key k);	
	void set_default();

private:
	hotkey_set _set;

	uint16_t _opt_hotkey_x;
	uint16_t _hotkey_left_align;
	uint16_t _hotkey_value_left_align;
	uint16_t _hotkey_start_y;
};

typedef std::shared_ptr<HotKeys> hotkey_ptr;

struct global_option {
	std::string name;
	std::vector<std::string> options;
	size_t current_option_index;

	std::string option() { return options[current_option_index]; }
	std::string to_previous() { 
		if (current_option_index != 0) current_option_index--;
		return option();
	}
	std::string to_next() {
		if (current_option_index != options.size() - 1) current_option_index++;
		return option();
	}
};

typedef std::vector<global_option> global_list;
typedef std::vector<hotkey> hotkey_list;

class Global : public BaseControl {
public:
	Global(uint16_t res);

	void init() override;

	snake_vec options();
	snake_vec menu();
	global_list& settings();

	uint16_t resolution_val();
	float speed_val();
	uint8_t max_food_val();

	component resolution_cpnt();
	component speed_cpnt();
	component max_food_cpnt();
	component value_component(size_t index);

	void option_slide_left(size_t option_idx);
	void option_slide_right(size_t option_idx);

	snake_vec markers() { return _markers; };
	component get_marker(size_t idx, bool left);

private:
	void add_marker(uint16_t x, uint16_t y);


private:
	snake_vec _markers;
	global_list _set;

	uint16_t _global_left_align;
	uint16_t _global_value_left_align;
	uint16_t _global_start_y;
};

typedef std::function<void(Event)> keyboard_callback;

class Keyboard {
public:
	Keyboard() :
		_keyboard_running(true)
	{}

	void main();

	static std::condition_variable kbcv;
	static std::mutex kbmtx;
	static std::atomic<bool> on_hit;
	static std::atomic<key> current_key;

private:
	key get_key(int k);

private:
	std::atomic<bool> _keyboard_running;
};