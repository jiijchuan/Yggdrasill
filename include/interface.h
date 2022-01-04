#pragma once
#include "global.h"
#include "settings.h"
#include "game.h"



class Interface {
public:
	Interface(HotKeys hotkey) : 
		_flush_list({}),
		_running(true),
		_keyboard(hotkey)
	{};

	Interface() :
		_flush_list({}),
		_running(true),
		_keyboard()
	{};

	void clear_all(uint16_t res);

protected:
	// set everything in flush list to console and then empty flush list
	void flush(); 
	void cover_ch(char ch, uint16_t x, uint16_t y);

	// cast waiting changes to flush list
	void cast_change(component c);
	void cast_change(std::vector<component> c);
	void clear_area(component& c);
	void clear_area(snake_vec vec);

protected:
	HotKeys _hotkey;
	Keyboard _keyboard;
	std::vector<component> _flush_list;
	std::atomic<bool> _running;
};

class StageMenu : public Interface {
public:
	StageMenu(uint16_t res) : Interface(HotKeys(res)), _menu(res) {};
	void main(Stage& s);
	void fresh() { _menu.set_fresh_new(); };
	void old() { _menu.set_damn_old(); };

private:
	MainMenu _menu;
};


class StageGame: public Interface {
public:
	StageGame(uint16_t res, uint8_t max_food = 3) :
		Interface(),
		_global(res),
		_map(res),
		_snake(res),
		_pause(res),
		_game_over(res),
		_rookie(true)
	{};

	void main(Stage& s);
	void set_global(Global& global) { _global = global; }
	void set_hotkey(hotkey_ptr hkp) { _keyboard.set_hotkey(hkp); }
	bool rookie() { return _rookie.load(); };
	void set_rookie(bool r) { _rookie.store(r); };

private:
	// game main workflow functions
	void game_show(Stage& s);
	void reset_all_assets();
	void load_basic_assets();
	void judge_keys(Stage& s);
	bool check_died();
	void check_grown();
	void post_changes();

private:
	// windows
	void show_pause_window(Stage& s);
	void show_go_window(Stage& s);

private:
	void game_keyboard();
	void end_stage();

private:
	Global _global;
	GameOverWindow _game_over;
	PauseWindow _pause;
	GameMapControl _map;
	SnakeControl _snake;
	std::atomic<bool> _rookie;
};


class StageSettings : public Interface {
public:
	StageSettings(uint16_t res): 
		Interface(HotKeys()), 
		_setting_control(res)
	{};

	void main(Stage& s);
	hotkey_ptr hotkey() { return _setting_control.get_hotkey_ptr(); }
	Global& global() { return _setting_control.global(); }

private:
	void load_basic_assets();

private:
	SettingControl _setting_control;
};



