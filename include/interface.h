#pragma once
#include "global.h"
#include "settings.h"
#include "game.h"


class Interface {
public:
	Interface() {};

	void clear_all(uint16_t res);

	void set_hotkey(hotkey_ptr hotkey) { _hotkey = *hotkey.get(); }

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
	std::vector<component> _flush_list;
	std::atomic<bool> _running;
};

class StageMenu : public Interface {
public:
	StageMenu(uint16_t res) : Interface(), _menu(res) {};
	void main(Stage& s);
	void fresh() { _menu.set_fresh_new(); };
	void old() { _menu.set_damn_old(); };

private:
	MainMenu _menu;
};

class StagePause : public Interface {
public:
	StagePause(uint16_t res) : _pause(res) {}
	void main(Stage& s);

private:
	PauseWindow _pause;
};

class StageGameOver : public Interface {
public:
	StageGameOver(uint16_t res) : _game_over(res){}
	void main(Stage& s);

private:
	GameOverWindow _game_over;
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
	bool rookie() { return _rookie.load(); };
	void set_rookie(bool r) { _rookie.store(r); };

private:
	// game main workflow functions
	void game_show(Stage& s);
	void reset_all_assets();
	void load_basic_assets();
	bool check_died();
	void check_grown();
	void post_changes();

private:
	StageGameOver _game_over;
	StagePause _pause;

	Global _global;
	GameMapControl _map;
	SnakeControl _snake;
	std::atomic<bool> _rookie;

// ver.2
private:
	void judge_key(Stage& s);
};


class StageSettings : public Interface {
public:
	StageSettings(uint16_t res):
		Interface(),
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



