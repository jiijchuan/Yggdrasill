#include "interface.h"
#include "settings.h"

void Interface::cast_change(component c) {
	flush_list_.push_back(c);
}

void Interface::cast_change(std::vector<component> c) {
	for (auto& i : c) flush_list_.push_back(i);
}

void Interface::clear_area(component& c) {
	for (size_t i = 0; i < c.mat().size(); i++) {
		for (size_t j = 0; j < c.mat()[0].size(); j++) {
			cover_ch(' ', (uint16_t)j + c.x(), (uint16_t)i + c.y());
		}
	}
}

void Interface::clear_area(snake_vec vec) {
	for (auto& i : vec) clear_area(i);
}

void Interface::clear_all(uint16_t res) {
	for (uint16_t i = 0; i < res / 2; i++) {
		for (uint16_t j = 0; j < res; j++) {
			cover_ch(' ', j, i);
		}
	}
}

void Interface::flush() {
	for (auto& i : flush_list_) {
		for (size_t j = 0; j < i.mat().size(); j++) {
			for (size_t k = 0; k < i.mat()[0].size(); k++) {
				if (i.mat()[j][k] != SPACE) cover_ch(i.mat()[j][k], (uint16_t)k + i.x(), (uint16_t)j + i.y());
			}
		}
	}
	flush_list_.clear();
}

void Interface::cover_ch(char ch, uint16_t x, uint16_t y) {
#ifdef _WIN64
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (SHORT)x, (SHORT)y });
	putchar(ch);
#endif
}

void StageMenu::main(Stage& s) {
	_menu.reset_assets();
	running_.store(true);
	clear_all(_menu.resolution());
	keyboard_.set_keyboard_running(true);
	cast_change(_menu.arrow());
	cast_change(_menu.components());
	flush();

	keyboard_.main([&](Event key) {
		switch (key) {
		case Event::KEY_UP:
			clear_area(_menu.arrow());
			_menu.arrow_up();
			break;
		case Event::KEY_DOWN:
			clear_area(_menu.arrow());
			_menu.arrow_down();
			break;
		case Event::KEY_ENTER:
			// check arrow position and set correspond stage factor
			s = _menu.to_stage();
			keyboard_.set_keyboard_running(false);
			running_.store(false);
			break;
		default:
			break;
		}
		if ((!running_) || s != Stage::SNAKE_MENU) return;
		if (keyboard_.get_keyboard_running()) cast_change(_menu.arrow());
		flush();
		});
}

void StageGame::main(Stage& s) {
	std::thread show([&]() { game_show(s); });
	std::thread key([&]() { game_keyboard(); });
	show.join();
	key.join();
}

void StageGame::game_show(Stage& s) {
	if (rookie()) reset_all_assets();
	load_basic_assets();
	set_rookie(false);
	while (running_) {
		judge_keys(s);
		if ((!running_) || 
			(s != Stage::SNAKE_GAME 
			&& s != Stage::GAME_CONTINUE 
			&& s!=Stage::SNAKE_RESTART)) break;

		if (check_died()) { 
			set_rookie(true);
			keyboard_.set_current_event(Event::KEY_ESC);
			show_go_window(s); 
			keyboard_.set_current_event(Event::NONE_EVENT);
			keyboard_.resume_keyboard();
			return; 
		}
		check_grown();
		post_changes();
#ifdef _WIN64
		Sleep(500 / _global.speed_val());
#endif
#ifdef __linux__
		usleep(500 / _global.speed_val());
#endif
	}
}

void StageGame::judge_keys(Stage& s) {
	auto last_key = Event::NONE_EVENT;
	auto event = keyboard_.current_event();
	// void judge_keys(Event key, Stage& s)
	switch (event) {
	case Event::KEY_ESC:
	case Event::KEY_PAUSE:
		show_pause_window(s);
		keyboard_.set_current_event(last_key);
		keyboard_.resume_keyboard();
		break;
	case Event::KEY_UP:
		_snake.set_status(SnakeStatus::HEADING_UP);
		last_key = event;
		break;
	case Event::KEY_DOWN:
		_snake.set_status(SnakeStatus::HEADING_DOWN);
		last_key = event;
		break;
	case Event::KEY_LEFT:
		_snake.set_status(SnakeStatus::HEADING_LEFT);
		last_key = event;
		break;
	case Event::KEY_RIGHT:
		_snake.set_status(SnakeStatus::HEADING_RIGHT);
		last_key = event;
		break;
	default:
		break;
	}
}

bool StageGame::check_died() {
	auto body = _snake.body();
	return _snake.touches({ _snake.tail(), _map.fence() }) || _snake.touches(snake_vec(body.begin() + 1, body.end()));
}

void StageGame::check_grown() {
	auto foods = _map.foods();
	for (size_t i = 0; i < foods.size(); i++) {
		// if touch any food, snake grows
		if (_snake.touches(foods[i])) {
			clear_area(foods[i]);
			_map.erase_food(i);
			_snake.grow();
		}
	}
}

void StageGame::post_changes() {
	clear_area(_snake.tail());
	_snake.step();
	_map.gen_food(_snake, _global);
	cast_change(_snake.components());
	cast_change(_map.foods());
	flush();
}

void StageGame::reset_all_assets() {
	_map.reset_assets();
	_snake.reset_assets();
	_pause.reset_assets();
}

void StageGame::load_basic_assets() {
	running_.store(true);
	keyboard_.set_keyboard_running(true);
	clear_all(_map.resolution());
	_map.gen_food(_snake, _global);
	cast_change(_map.fence());
	cast_change(_snake.components());
	cast_change(_map.foods());
	flush();
	if(rookie()) keyboard_.set_current_event(Event::KEY_RIGHT);
}

void StageGame::game_keyboard() {
	keyboard_.set_keyboard_running(true);
	keyboard_.main([this](Event key) {});
}

void StageGame::show_pause_window(Stage& s) {
	// load_basic_assets
	clear_area(_pause.components());
	cast_change(_pause.components());
	cast_change(_pause.arrow());
	flush();

	auto pause_window_running = true;
	// while (pause_window_running) {
	// 	std::unique_lock<std::mutex> pause_window_lock(Keyboard::kbmtx);
	// 	while (!Keyboard::on_hit) Keyboard::kbcv.wait(pause_window_lock);
	// }

	Keyboard window_kbd(HotKeys(_map.resolution()));
	window_kbd.main([&](Event key) {
		auto stage = _pause.on_event(key);
		if (stage != Stage::NONE_STAGE) {
			window_kbd.set_keyboard_running(false);
			switch (stage) {
			case Stage::GAME_CONTINUE:
			case Stage::SNAKE_GAME:
			case Stage::SNAKE_RESTART:
				clear_area(_pause.components());
				cast_change(_snake.components());
				cast_change(_map.foods());
				flush();
				return;
			case Stage::SNAKE_MENU:
				s = stage;
				end_stage();
				return;
			case Stage::SNAKE_SETTINGS:
				s = stage;
				end_stage();
				return;
			case Stage::SNAKE_END:
				s = stage;
				end_stage();
				clear_all(_map.resolution());
				return;
			default:break;
			}
		}
		clear_area(_pause.cleared());
		cast_change(_pause.casted());
		flush();
		});
}

void StageGame::end_stage() {
	running_.store(false);
	keyboard_.set_keyboard_running(false);
}

void StageGame::show_go_window(Stage& s) {
	// load game over window assets
	clear_area(_game_over.components());
	cast_change(_game_over.components());
	cast_change(_game_over.arrow());
	flush();

	Keyboard window_kbd(HotKeys(_map.resolution()));
	window_kbd.main([&](Event key) {
		auto stage = _game_over.on_event(key);
		if (stage != Stage::NONE_STAGE) {
			window_kbd.set_keyboard_running(false);
			switch (stage) {
			case Stage::SNAKE_RESTART:
				end_stage();
				return;
			case Stage::SNAKE_MENU:
				s = stage;
				end_stage();
				return;
			case Stage::SNAKE_END:
				s = stage;
				end_stage();
				clear_all(_map.resolution());
				return;
			default:break;
			}
		}
		clear_area(_game_over.cleared());
		cast_change(_game_over.casted());
		flush();
		});
}

void StageSettings::main(Stage& s) {
	load_basic_assets();
	while (keyboard_.get_keyboard_running()) {
		// TODO:: need to be fully rewrite
#ifdef _WIN64
		if (_kbhit()) {
			auto key = _getch();
			auto k = keyboard_.get_key(key);
			auto e = keyboard_.hotkey().k2evt(k);
			if (_setting_control.changing_hotkey()) {
				_setting_control.change_hotkey(e, k);
				cast_change(_setting_control.casted());
				flush();
				continue;
			}
	
			// main
			if (e == Event::KEY_ESC) {
				keyboard_.set_keyboard_running(false);
				keyboard_.set_current_event(Event::NONE_EVENT);
				return;
			}
			else {
				_setting_control.on_event(e);
				auto cleared = _setting_control.cleared();
				clear_area(cleared);
				cast_change(_setting_control.casted());
				flush();
			}
		}
#endif
	}
}


void StageSettings::load_basic_assets() {
	keyboard_.set_keyboard_running(true);
	running_.store(true);

	_setting_control.reset_page();
	clear_all(_setting_control.resolution());
	cast_change(_setting_control.components());
	cast_change(_setting_control.current_page());
	cast_change(_setting_control.arrow());
	flush();
}