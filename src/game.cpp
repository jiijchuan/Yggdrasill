#include "game.h"

MenuControl::MenuControl(uint16_t res) :
	BaseControl(res),
	_option_idx(0),
	_arrow(component("@", 0, 0))
{}

component& MenuControl::arrow() { return _arrow; };

void MenuControl::arrow_to_front() {
	assert(_menu.size() > 0);
	if (_option_idx > 0) {
		_option_idx -= 1;
		cast_arrow(_menu[_option_idx]);
	}
};

void MenuControl::arrow_to_end() {
	assert(_menu.size() > 0);
	if (_option_idx < _menu.size() - 1) { 
		_option_idx += 1; 
		cast_arrow(_menu[_option_idx]);
	}
};

void MenuControl::load_menu(snake_vec menu) {
	_menu = menu; 
	_option_idx = 0; 
	cast_arrow(menu[_option_idx]);
};

void MenuControl::load_menu(snake_vec menu, size_t idx) {
	_menu = menu;
	_option_idx = idx;
	cast_arrow(menu[_option_idx]);
}

std::string MenuControl::arrow_at() {
	assert(_menu.size() > 0);
	return _menu[_option_idx].to_string();
}

void MenuControl::update_casted(component cpnt) {
	_need_casted.push_back(cpnt); 
}

void MenuControl::update_casted(snake_vec v) {
	for (auto& i : v) _need_casted.push_back(i);
}

void MenuControl::update_cleared(component cpnt) {
	_need_cleared.push_back(cpnt); 
}

void MenuControl::update_cleared(snake_vec v) {
	for (auto& i : v) _need_cleared.push_back(i);
}

snake_vec MenuControl::casted() {
	auto temp = _need_casted; _need_casted.clear(); return temp;
}

snake_vec MenuControl::cleared() {
	auto temp = _need_cleared; _need_cleared.clear(); return temp;
}

void MenuControl::cast_arrow(component& cpnt) {
	_arrow.x() = cpnt.x() - 2;
	_arrow.y() = cpnt.y();
}

void SnakeControl::init() {
	_components.push_back(component(snake_mat({{ HEAD }}), (uint16_t)(_resolution / 2), (uint16_t)(_resolution / 4)));
	_components.push_back(component(snake_mat({ { BODY } }), (uint16_t)(_resolution / 2 - 1), (uint16_t)(_resolution / 4)));
	_components.push_back(component(snake_mat({ { TAIL } }), (uint16_t)(_resolution / 2 - 2), (uint16_t)(_resolution / 4)));
}

int SnakeControl::step() {
	// 1. judge direction
	if (heading_backhead()) dir_reverse();

	uint16_t new_x = 0, new_y = 0;
	
	switch (status_) {
	case SnakeStatus::HEADING_DOWN:
		new_x = head().x();
		new_y = head().y() + 1;
		break;
	case SnakeStatus::HEADING_UP:
		new_x = head().x();
		new_y = head().y() - 1;
		break;
	case SnakeStatus::HEADING_LEFT:
		new_x = head().x() - 1;
		new_y = head().y();
		break;
	case SnakeStatus::HEADING_RIGHT:
		new_x = head().x() + 1;
		new_y = head().y();
		break;
	default:
		break;
	}
	// 2. generate a new body at head position
	_components.insert(_components.begin() + 1, component(snake_mat({ {BODY} }), head().x(), head().y()));

	// 3. modify head position
	head().x() = new_x;
	head().y() = new_y;

	//if not growing, move tail and pop last part of body
	if (!growing_) {
		tail().x() = _components[_components.size() - 2].x();
		tail().y() = _components[_components.size() - 2].y();
		_components.erase(_components.end() - 2);
	}

	// whatever growing or not, set growing to false
	growing_.store(false);

	return 1;
}

bool SnakeControl::heading_backhead() {
	auto head_ = head();
	auto backhead = _components[1];
	if (backhead.x() == head_.x() && (backhead.y() == head_.y() + 1)) return status_ == SnakeStatus::HEADING_DOWN;
	else if (backhead.x() == head_.x() && (backhead.y() == head_.y() - 1)) return status_ == SnakeStatus::HEADING_UP;
	else if ((backhead.x() == head_.x() + 1) && backhead.y() == head_.y()) return status_ == SnakeStatus::HEADING_RIGHT;
	else if ((backhead.x() == head_.x() - 1) && backhead.y() == head_.y()) return status_ == SnakeStatus::HEADING_LEFT;
#ifdef _WIN64
	else throw std::exception("incorrect head-body direction!");
#else
	else throw std::exception();
#endif
}

void SnakeControl::dir_reverse() {
	switch (status_) {
	case SnakeStatus::HEADING_DOWN:
		status_ = SnakeStatus::HEADING_UP;
		break;
	case SnakeStatus::HEADING_UP:
		status_ = SnakeStatus::HEADING_DOWN;
		break;
	case SnakeStatus::HEADING_LEFT:
		status_ = SnakeStatus::HEADING_RIGHT;
		break;
	case SnakeStatus::HEADING_RIGHT:
		status_ = SnakeStatus::HEADING_LEFT;
		break;
	default:
		break;
	}
}


bool SnakeControl::touches(component& c) {
	auto& head = _components[0];
	int c_w = c.mat()[0].size();
	int c_h = c.mat().size();

	auto y_short = head.y() - c.y();
	auto x_short = head.x() - c.x();

	switch (status_) {
	case SnakeStatus::HEADING_UP:
		if (y_short < 1 || x_short < 0 || y_short - 1 >= c_h || x_short >= c_w) return false;
		return c.mat()[(size_t)y_short - (size_t)1][x_short] != SPACE;
	case SnakeStatus::HEADING_DOWN:
		if (y_short < -1 || x_short < 0 || y_short + 1 >= c_h || x_short >= c_w) return false;
		return c.mat()[(size_t)y_short + (size_t)1][x_short] != SPACE;
	case SnakeStatus::HEADING_LEFT:
		if (y_short < 0 || x_short < 1 || y_short >= c_h || x_short - 1 >= c_w) return false;
		return c.mat()[y_short][(size_t)x_short - (size_t)1] != SPACE;
	case SnakeStatus::HEADING_RIGHT:
		if (y_short < 0 || x_short < -1 || y_short >= c_h || x_short + 1 >= c_w) return false;
		return c.mat()[y_short][(size_t)x_short + (size_t)1] != SPACE;
	default:
		return true;
	}
}

bool SnakeControl::touches(snake_vec c) {
	bool touch = false;
	for (auto& i : c) {
		touch = touches(i) ? true : false;
		if (touch) return touch;
	}
	return touch;
}

void GameMapControl::init() {
	_components.push_back(init_fence(_height, _width));
}

void GameMapControl::gen_food(SnakeControl& snake, Global& global) {
	bool dup = false;
	uint16_t food_x = 0, food_y = 0;
	if (_components.size() - 1 < global.max_food_val()) {
		do {
			dup = false;
			food_x = rand() % (_width - 2) + 1;
			food_y = rand() % (_height - 2) + 1;
			for (auto& i : _components) { if (food_x == i.x() && food_y == i.y()) dup = true; }
			for (auto& i : snake.components()) { if (food_x == i.x() && food_y == i.y()) dup = true; }
		} while (dup);
		_components.push_back(component(snake_mat({ {FOOD} }), food_x, food_y));
	}
}

void MainMenu::init() {
	_components.clear();
	_components.push_back(init_fence(_height, _width));

	if (!rookie()) { 
		add_component("Continue", 0, 0);
		add_component("Restart", 0, 0);
	}
	else {
		add_component("Start", 0, 0);
	}
	add_component("Settings", 0, 0);
	add_component("Exit", 0, 0);

	for (size_t i = 1; i < _components.size(); i++) {
		_components[i].x() = _menu_left_align;
		_components[i].y() = _menu_start_height + i - 1;
	}
	load_menu(snake_vec(_components.begin() + 1, _components.end()));
}

Stage MainMenu::to_stage() {
	auto option = arrow_at();
	if (option == "Start") return Stage::SNAKE_GAME;
	else if (option == "Continue") return Stage::GAME_CONTINUE;
	else if (option == "Restart") {	return Stage::SNAKE_RESTART; }
	else if (option == "Settings") return Stage::SNAKE_SETTINGS;
	else if (option == "Exit") return Stage::SNAKE_END;
#ifdef _WIN64
	else throw std::exception("wrong option name!");
#else
	else return Stage::NONE_STAGE;
#endif
}


void PauseWindow::init() {
	uint16_t menu_start_height = _height / 4 + 4;
	uint16_t menu_left_align = _width / 2 - 3;

	// fence
	add_component(init_fence(_height / 2, _width / 3 + 2, _width / 3, _height / 4));
	
	// message
	add_component("Game Paused", menu_left_align - 2, menu_start_height - 2);

	// options
	add_component("Resume", menu_left_align, menu_start_height);
	add_component("Menu", menu_left_align, menu_start_height + 1);
	add_component("Settings", menu_left_align, menu_start_height + 2);
	add_component("Exit", menu_left_align, menu_start_height + 3);

	load_menu(options());
}

Stage PauseWindow::on_event(Event e) {
	switch (e) {
	case Event::KEY_ESC:
	case Event::KEY_PAUSE:
		return Stage::SNAKE_GAME;
	case Event::KEY_UP:
		update_cleared(_arrow);
		arrow_to_front();
		update_casted(_arrow);
		return Stage::NONE_STAGE;
	case Event::KEY_DOWN:
		update_cleared(_arrow);
		arrow_to_end();
		update_casted(_arrow);
		return Stage::NONE_STAGE;
	case Event::KEY_ENTER:
		return on_option();
	default:
		return Stage::NONE_STAGE;
	}
}

Stage PauseWindow::on_option() {
	switch (arrow_idx()) {
	case 0:
		return Stage::SNAKE_GAME;
	case 1:
		return Stage::SNAKE_MENU;
	case 2:
		return Stage::SNAKE_SETTINGS;
	case 3:
		return Stage::SNAKE_END;
	default:
		return Stage::NONE_STAGE;
	}
}

void GameOverWindow::init() {
	add_component(init_fence(_height / 2, _width / 3 + 2, _width / 3, _height / 4));
	add_component("Game Over", _width / 2 - 5, _height / 4 + 2);
	add_component("Restart", _width / 2 - 3, _height / 4 + 4);
	add_component("Menu", _width / 2 - 3, _height / 4 + 5);
	add_component("Exit", _width / 2 - 3, _height / 4 + 6);
	load_menu(options());
}

Stage GameOverWindow::on_event(Event e) {
	switch (e) {
	case Event::KEY_UP:
		update_cleared(_arrow);
		arrow_to_front();
		update_casted(_arrow);
		return Stage::NONE_STAGE;
	case Event::KEY_DOWN:
		update_cleared(_arrow);
		arrow_to_end();
		update_casted(_arrow);
		return Stage::NONE_STAGE;
	case Event::KEY_ENTER:
		return on_option();
	default:
		return Stage::NONE_STAGE;
	}
}

Stage GameOverWindow::on_option() {
	switch (arrow_idx()) {
	case 0:
		return Stage::SNAKE_RESTART;
	case 1:
		return Stage::SNAKE_MENU;
	case 2:
		return Stage::SNAKE_END;
	default:
		return Stage::NONE_STAGE;
	}
}


void SettingControl::init() {
	uint16_t menu_height = 3;
	uint16_t opt_hotkey_x = _width / 2 + 3;
	uint16_t opt_global_x = _width / 2 - 10;
	// fence
	add_component(init_fence(_height, _width, 0, 0));

	// menu
	add_component("Hotkey", opt_global_x, menu_height);
	add_component("Global", opt_hotkey_x, menu_height);

	load_menu(settings_menu());
}

snake_vec SettingControl::current_page() {
	snake_vec temp;
	switch (_page_showing) {
	case SettingPage::PAGE_HOTKEY:
		return  _hotkey.menu();
	case SettingPage::PAGE_GLOBAL:
		return _global.menu();
	default:
#ifdef _WIN64
		throw std::exception("wrong page value!");
#else
		throw std::exception();
#endif
	}
}

void SettingControl::on_event(Event e) {
	auto page = _arrow_at;
	switch (page) {
	case SettingPage::PAGE_MENU:
		on_menu(e);
		break;
	case SettingPage::PAGE_GLOBAL:
		on_global(e);
		break;
	case SettingPage::PAGE_HOTKEY:
		on_hotkey(e);
		break;
	default:
		break;
	}
}

void SettingControl::on_menu(Event e) {
	switch (e) {
	case Event::KEY_DOWN:
		if (_page_showing == SettingPage::PAGE_HOTKEY) {
			_arrow_at = SettingPage::PAGE_HOTKEY;
			update_cleared(_arrow);
			load_menu(_hotkey.options());
			update_casted(_arrow);
		}
		else if (_page_showing == SettingPage::PAGE_GLOBAL) {
			_arrow_at = SettingPage::PAGE_GLOBAL;
			update_cleared(_arrow);
			load_menu(_global.options());
			update_casted(_arrow);
		}
#ifdef _WIN64
		else throw std::exception("wrong arrow at value!");
#endif
#ifdef __linux__
		else throw std::exception();
#endif
		break;
	case Event::KEY_LEFT:
		if (_page_showing == SettingPage::PAGE_GLOBAL) { 
			_page_showing = SettingPage::PAGE_HOTKEY; 
			update_cleared(_global.menu());
			update_casted(_hotkey.menu());
		}
		update_cleared(_arrow);
		arrow_to_front();
		update_casted(_arrow);
		break;
	case Event::KEY_RIGHT:
		if (_page_showing == SettingPage::PAGE_HOTKEY) { 
			_page_showing = SettingPage::PAGE_GLOBAL; 
			update_cleared(_hotkey.menu());
			update_casted(_global.menu());
		}
		update_cleared(_arrow);
		arrow_to_end();
		update_casted(_arrow);
		break;
	default:
		break;
	}
}

void SettingControl::on_global(Event e) {
	assert(_menu.size() == _global.components().size() / 2);
	component old_val, new_val;
	switch (e) {
	case Event::KEY_UP:
		update_cleared(_arrow);
		arrow_to_front();
		if (_option_idx == 0) { 
			_arrow_at = SettingPage::PAGE_MENU; 
			load_menu(trunk_menu(), 1);
		}
		update_casted(_arrow);
		break;
	case Event::KEY_DOWN:
		update_cleared(_arrow);
		arrow_to_end();
		update_casted(_arrow);
		break;
	case Event::KEY_LEFT:
		global_check_marker_casted(e);
		update_cleared(_global.value_component(arrow_idx()));
		_global.option_slide_left(arrow_idx());
		update_casted(_global.value_component(arrow_idx()));
		global_check_marker_cleared();
		break;
	case Event::KEY_RIGHT:
		global_check_marker_casted(e);
		old_val = _global.value_component(arrow_idx());
		_global.option_slide_right(arrow_idx());
		update_casted(_global.value_component(arrow_idx()));
		global_check_marker_cleared();
		break;
	default:
		break;
	}
}

void SettingControl::global_check_marker_cleared() {
	auto option = _global.settings()[arrow_idx()];
	if (option.current_option_index == 0) {
		update_cleared(_global.get_marker(arrow_idx(), true));
	}
	else if (option.current_option_index == option.options.size() - 1) {
		update_cleared(_global.get_marker(arrow_idx(), false));
	}
}

void SettingControl::global_check_marker_casted(Event e) {
	auto option = _global.settings()[arrow_idx()];
	if (option.current_option_index == 0 && e == Event::KEY_RIGHT) {
		update_casted(_global.get_marker(arrow_idx(), true));
	}
	else if (option.current_option_index == option.options.size() - 1 && e == Event::KEY_LEFT) {
		update_casted(_global.get_marker(arrow_idx(), false));
	}
}

void SettingControl::on_hotkey(Event e) {
	assert(_menu.size() == _hotkey.menu().size() / 2);
	component old_val, new_val;
	switch (e) {
	case Event::KEY_UP:
		update_cleared(_arrow);
		arrow_to_front();
		if (_option_idx == 0) { 
			_arrow_at = SettingPage::PAGE_MENU; 
			load_menu(trunk_menu());
		}
		update_casted(_arrow);
		break;
	case Event::KEY_DOWN:
		update_cleared(_arrow);
		arrow_to_end();
		update_casted(_arrow);
		break;
	case Event::KEY_ENTER:
		old_val = _hotkey.value_component(arrow_idx());
		update_cleared(old_val);
		_changing_hotkey.store(true);
		//set_hotkey();
		//new_val = _hotkey.value_component(arrow_idx());
		//update_casted(new_val);
	default:
		break;
	}
}

void SettingControl::change_hotkey(Event e, key k) {
	if (e == Event::KEY_ESC) give_up_change_hotkey();
	else {
		_hotkey.set_key(_option_idx + 2, k);
		auto& temphkr = _hotkey.value_component(arrow_idx());
		temphkr = component(_hotkey.k2str(k), temphkr.x(), temphkr.y());
		update_casted(temphkr);
	}
	_changing_hotkey.store(false);;
}

void SettingControl::reset_page() {
	_page_showing = SettingPage::PAGE_HOTKEY;
	_arrow_at = SettingPage::PAGE_MENU;
	load_menu(trunk_menu());
}

void SettingControl::give_up_change_hotkey() {
	update_casted(_hotkey.value_component(arrow_idx()));
}

component SettingControl::arrow_point_value() {
	switch (_arrow_at) {
	case SettingPage::PAGE_HOTKEY:
		return _hotkey.value_component(_option_idx);
	case SettingPage::PAGE_GLOBAL:
		return _global.value_component(_option_idx);
	default:
		return component();
	}
}

