#pragma once
#include "global.h"
#include "base.h"
#include "settings.h"


static const char HEAD = '@';
static const char BODY = '#';
static const char TAIL = '.';
static const char FOOD = '*';
static const char ARROW = '@';


class MenuControl : public BaseControl {
public:
	MenuControl(uint16_t res);
	component& arrow();
	void load_menu(snake_vec menu);
	void load_menu(snake_vec menu, size_t idx);

	std::string arrow_at();
	size_t arrow_idx() { return _option_idx; };

	void update_casted(component cpnt);
	void update_casted(snake_vec v);
	void update_cleared(component cpnt);
	void update_cleared(snake_vec v);

	snake_vec casted();
	snake_vec cleared();

public:
	component _arrow;
	snake_vec _need_cleared;
	snake_vec _need_casted;
	
protected:
	snake_vec _menu;
	size_t _option_idx;

protected:
	void arrow_to_front();
	void arrow_to_end();

private:
	void cast_arrow(component& cpnt);
};



class SnakeControl: public BaseControl {
public:
	SnakeControl(int res)
		: 
		BaseControl(res),
		_status(SnakeStatus::HEADING_RIGHT),
		_growing(false)
	{
		init();
	};

	void init() override;

	component& head() { return _components[0]; };
	component& tail() { return _components[_components.size() - 1]; };
	snake_vec body() { return snake_vec(_components.begin() + 1, _components.end() - 1); };
	
	void set_status(SnakeStatus s) { _status = s; };
	SnakeStatus status() { return _status.load(); };

	// erase tail before step
	int step();

	void grow() { _growing.store(true); };
	bool touches(component& c);
	bool touches(snake_vec c);

private:
	void dir_reverse();
	bool heading_backhead();

private:
	std::atomic<SnakeStatus> _status;
	std::atomic<bool> _growing;
};

class GameMapControl:public BaseControl {
public:
	GameMapControl(uint16_t res)
		: 
		BaseControl(res)
	{
		init();
	};

	void init() override;

	snake_vec foods() { return snake_vec(_components.begin() + 1, _components.end()); };
	void erase_food(size_t position) { _components.erase(_components.begin() + 1 + position); };

	component& fence() { return _components[0]; };

	// find a position that not duplicate with snake, fence and existed foods
	void gen_food(SnakeControl& snake, Global& global);
};


class MainMenu : public MenuControl {
public:
	MainMenu(uint16_t res)
		:
		MenuControl(res),
		_menu_start_height(res * 3 / 8),
		_menu_left_align(res / 2 - 2),
		_restart(false)
	{
		init();
	};

	void init() override;

	// if it's first time enter menu or died and return
	bool rookie() { return _restart.load(); };

	// first time load menu, or died and return to menu
	void set_fresh_new() { _restart.store(true); };

	void arrow_up() { arrow_to_front(); };
	void arrow_down() { arrow_to_end(); };

	// back from pause window
	void set_damn_old() { _restart.store(false); };
	snake_vec options() { return snake_vec(_components.begin() + 1, _components.end()); };
	Stage to_stage();

private:
	std::atomic<bool> _restart;

private:
	uint16_t _menu_start_height;
	uint16_t _menu_left_align;
};

class Window : public BaseControl {
public:
	Window(uint16_t res)
		: BaseControl(res)
	{};

	virtual component& fence() = 0;
	virtual component& message() = 0;
	virtual component& arrow() = 0;
	virtual snake_vec options() = 0;
	virtual void arrow_mv(uint16_t distance, Event direction) = 0;
	virtual bool is_legal() = 0;
};

class PauseWindow : public MenuControl {
public:
	PauseWindow(uint16_t res)
		: MenuControl(res)
	{
		init();
	}
	
	void init() override;

	Stage on_event(Event e);
	
	component& fence()  { return _components[0]; };
	component& message()   { return _components[1]; };
	snake_vec options()  { return snake_vec(_components.begin() + 2, _components.end()); };
	
	component opt_resume() { return options()[0]; };
	component opt_menu() { return options()[1]; };
	component opt_settings() { return options()[2]; };
	component opt_exit() { return options()[3]; };
private:
	Stage on_option();

};

class GameOverWindow : public MenuControl {
public:
	GameOverWindow(uint16_t res)
		: MenuControl(res)
	{
		init();
	}
	void init() override;

	Stage on_event(Event e);

	component& fence() { return _components[0]; };
	component& message() { return _components[1]; };
	snake_vec options() { return snake_vec(_components.begin() + 2, _components.end()); };

private:
	Stage on_option();

};

struct SettingOption {
	std::string name;
	component title;
	component value;
	std::vector<std::string> candidates;
	size_t candidate_index;
};


typedef std::vector<SettingOption> option_list;
typedef std::function<void(Event)> hotkey_modifier;

class SettingControl : public MenuControl {
public:
	SettingControl(uint16_t res) :
		MenuControl(res), 
		_arrow_at(SettingPage::PAGE_MENU),
		_page_showing(SettingPage::PAGE_HOTKEY),
		_hotkey(res),
		_global(res),
		_changing_hotkey(false)
	{ init(); };

	void init() override;

	void reset_page();

	Global& global() { return _global; }
	inline hotkey_ptr get_hotkey_ptr() { return std::make_shared<HotKeys>(_hotkey); }
	bool changing_hotkey() { return _changing_hotkey.load(); }
	snake_vec current_page();
	void on_event(Event e);
	void change_hotkey(Event e, key k);

private:
	inline snake_vec trunk_menu() { return snake_vec(_components.begin() + 1, _components.begin() + 3); }
	inline snake_vec settings_menu() { return snake_vec(_components.begin() + 1, _components.end()); };
	void give_up_change_hotkey();
	component arrow_point_value();

private:
	void on_menu(Event e);
	void on_global(Event e);
	void on_hotkey(Event e);
	void global_check_marker_cleared();
	void global_check_marker_casted(Event e);

private:
	HotKeys _hotkey;
	Global _global;

	SettingPage _arrow_at;
	SettingPage _page_showing;
	std::atomic<bool> _changing_hotkey;
};

