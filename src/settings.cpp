#include "settings.h"

HotKeys::HotKeys(uint16_t res) : BaseControl(res),
_set({
	{ "Esc", Event::KEY_ESC, KeyType::ASCII, 27 },
	{ "Enter", Event::KEY_ENTER, KeyType::ASCII, 13 },
	{ "Up", Event::KEY_UP, KeyType::NONE_ASCII, 72 },
	{ "Down", Event::KEY_DOWN, KeyType::NONE_ASCII, 80 },
	{ "Left", Event::KEY_LEFT, KeyType::NONE_ASCII, 75 },
	{ "Right", Event::KEY_RIGHT, KeyType::NONE_ASCII, 77 },
	{ "Pause", Event::KEY_PAUSE, KeyType::ASCII, 27 }
	})
{
	init();
}

HotKeys::HotKeys() : BaseControl(0),
_set({
	{ "Esc", Event::KEY_ESC, KeyType::ASCII, 27 },
	{ "Enter", Event::KEY_ENTER, KeyType::ASCII, 13 },
	{ "Up", Event::KEY_UP, KeyType::NONE_ASCII, 72 },
	{ "Down", Event::KEY_DOWN, KeyType::NONE_ASCII, 80 },
	{ "Left", Event::KEY_LEFT, KeyType::NONE_ASCII, 75 },
	{ "Right", Event::KEY_RIGHT, KeyType::NONE_ASCII, 77 },
	{ "Pause", Event::KEY_PAUSE, KeyType::ASCII, 27 }
	}),
	_opt_hotkey_x(0),
	_hotkey_left_align(0),
	_hotkey_value_left_align(0),
	_hotkey_start_y(0)
{}

snake_vec HotKeys::options() { assert(_resolution); return snake_vec(_components.begin(), _components.begin() + _components.size() / 2); }
snake_vec& HotKeys::menu() { assert(_resolution);  return _components; }
component& HotKeys::up() { assert(_resolution); return _components[5]; }
component& HotKeys::dn() { assert(_resolution); return _components[6]; }
component& HotKeys::lt() { assert(_resolution); return _components[7]; }
component& HotKeys::rt() { assert(_resolution); return _components[8]; }
component& HotKeys::pause() { assert(_resolution); return _components[9]; }

void HotKeys::init() {
	uint16_t menu_height = 3;
	_hotkey_left_align = _width / 2 - 10;
	_hotkey_value_left_align = _hotkey_left_align + 14;
	_hotkey_start_y = menu_height + 2;

	_components.clear();
	for (size_t i = 2; i < _set.size(); i++) {
		add_component(_set[i].name, _hotkey_left_align, _hotkey_start_y + (uint16_t)i);
	}

	for (size_t i = 2; i < _set.size(); i++) {
		auto value = k2str(key(_set[i].type, _set[i].value));
		add_component(value, _hotkey_value_left_align, _hotkey_start_y + (uint16_t)i);
	}
}

Event HotKeys::k2evt(key k) {
	for (auto& i : _set) {
		if (i.type == k.first && i.value == k.second) return i.key_event;
	}
	return Event::NONE_EVENT;
}

key HotKeys::get_key(Event e) {
	for (auto& i : _set) {
		if (e == i.key_event) return key(i.type, i.value);
	}
	return key(KeyType::ASCII, 0);
}


void HotKeys::set_key(Event e, key k) {
	for (size_t i = 0; i < _set.size(); i++) {
		if (e == _set[i].key_event) {
			_set[i].type = k.first; _set[i].value = k.second;
			auto temp = k2str(k);
			// set corresponding component
			_components[_components.size() / 2 + i].mat() = { {std::vector<char>(temp.begin(), temp.end())} };
		}
	}
}

void HotKeys::set_default() {
	_set = {
		{ "Esc", Event::KEY_ESC, KeyType::ASCII, 27 },
		{ "Enter", Event::KEY_ENTER, KeyType::ASCII, 13 },
		{ "Up", Event::KEY_UP, KeyType::NONE_ASCII, 72 },
		{ "Down", Event::KEY_DOWN, KeyType::NONE_ASCII, 80 },
		{ "Left", Event::KEY_LEFT, KeyType::NONE_ASCII, 75 },
		{ "Right", Event::KEY_RIGHT, KeyType::NONE_ASCII, 77 },
		{ "Pause", Event::KEY_PAUSE, KeyType::ASCII, 27 }
	};
}

std::string HotKeys::k2str(key k) {
	if (k.first == KeyType::ASCII && k.second != 13) {
		if (k.second == 27) return "esc";
		if (k.second == 32) return "space";
		std::string temp = "";
		temp.push_back((char)k.second);
		return temp;
	}
	else {
		switch (k.second) {
		case 72:
			return "up";
		case 80:
			return "down";
		case 75:
			return "left";
		case 77:
			return "right";
		default:
			return "";
		}
	}
}

Global::Global(uint16_t res) : 
	BaseControl(res),
	_set({
		{ "Resolution", {"35", "50", "100", "150"}, 1 },
		{ "Speed", {"0.25", "0.5", "0.75", "1", "2", "4", "8"}, 3 },
		{ "Max food", {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"}, 2 }
	})
{
	uint16_t menu_height = 3;
	_global_left_align = _width / 2 - 10;
	_global_value_left_align = _global_left_align + 18;
	_global_start_y = menu_height + 2;

	init();
}

void Global::init() {
	_components.clear();
	for (size_t i = 0; i < _set.size(); i++) {
		add_component(_set[i].name, _global_left_align, _global_start_y + (uint16_t)i);
	}
	for (size_t i = 0; i < _set.size(); i++) {
		add_component(_set[i].option(), _global_value_left_align, _global_start_y + (uint16_t)i);
		add_marker(_global_value_left_align, _global_start_y + (uint16_t)i);
	}
}

snake_vec Global::options(){ assert(_resolution); return snake_vec(_components.begin(), _components.begin() + _components.size() / 2); }

snake_vec Global::menu() { 
	auto temp = _components;
	temp.insert(temp.end(), _markers.begin(), _markers.end());
	return temp;
};

global_list& Global::settings() { return _set; }

uint16_t Global::resolution_val() { return static_cast<uint16_t>(stoi(_components[3].to_string())); };

float Global::speed_val() { return atof(_components[4].to_string().c_str()); };

uint8_t Global::max_food_val() { return static_cast<uint8_t>(stoi(_components[5].to_string())); };

component Global::resolution_cpnt() { return _components[3]; };

component Global::speed_cpnt() { return _components[4]; };

component Global::max_food_cpnt() { return _components[5]; };

component Global::value_component(size_t index) { return _components[_components.size() / 2 + index]; };

void Global::option_slide_left(size_t option_idx) {
	auto temp = _set[option_idx].to_previous();
	_components[_components.size() / 2 + option_idx].mat() = { {std::vector<char>(temp.begin(), temp.end())} };
}

void Global::option_slide_right(size_t option_idx) {
	auto temp = _set[option_idx].to_next();
	_components[_components.size() / 2 + option_idx].mat() = { {std::vector<char>(temp.begin(), temp.end())} };
}

component Global::get_marker(size_t idx, bool left) {
	return left ? _markers[idx * 2] : _markers[idx * 2 + 1];
}

void Global::add_marker(uint16_t x, uint16_t y) {
	_markers.push_back(str2cpnt("<", x - 2, y));
	_markers.push_back(str2cpnt(">", x + 5, y));
}


void Keyboard::main(keyboard_callback callback) {
	std::unique_lock<std::mutex> key_lock(_keyboard_mtx);
	while (_keyboard_running) {
		while (_current_event == Event::KEY_ESC) _keyboard_cv.wait(key_lock);
		if (_kbhit()) {
			auto key = _getch();
			if (!_keyboard_running) break;
			if (_current_event == Event::KEY_ESC) continue;
			else _current_event.store(_hotkey.k2evt(get_key(key)));
			callback(_current_event.load());
		}
	}
}

void Keyboard::set_hotkey(hotkey_ptr hotkey) {
	_hotkey.set() = hotkey->set();
}

void Keyboard::main() {
	while (_keyboard_running) {
		if (_kbhit()) {
			auto key = _getch();
			_current_key.store(get_key(key));
			Keyboard::kbcv.notify_all();
		}
	}
}

inline key Keyboard::get_key(int k) {
	// case each button
	if (isascii(k)) return key(KeyType::ASCII, k);
	else {
		auto ch = _getch();
		return key(KeyType::NONE_ASCII, ch);
	}
}

