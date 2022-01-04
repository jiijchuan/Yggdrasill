#include "interface.h"
#include "settings.h"

void Interface::cast_change(component c) {
	_flush_list.push_back(c);
}

void Interface::cast_change(std::vector<component> c) {
	for (auto& i : c) _flush_list.push_back(i);
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
	for (auto& i : _flush_list) {
		for (size_t j = 0; j < i.mat().size(); j++) {
			for (size_t k = 0; k < i.mat()[0].size(); k++) {
				if (i.mat()[j][k] != SPACE) cover_ch(i.mat()[j][k], (uint16_t)k + i.x(), (uint16_t)j + i.y());
			}
		}
	}
	_flush_list.clear();
}

void Interface::cover_ch(char ch, uint16_t x, uint16_t y) {
#ifdef _WIN64
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (SHORT)x, (SHORT)y });
	putchar(ch);
#endif
}

void StageMenu::main(Stage& s) {
	_menu.reset_assets();
	_running.store(true);
	clear_all(_menu.resolution());
	cast_change(_menu.arrow());
	cast_change(_menu.components());
	flush();

	while(_running){
		std::unique_lock<std::mutex> pw_lck(Keyboard::kbmtx);
		while (!Keyboard::on_hit) Keyboard::kbcv.wait(pw_lck);
		Keyboard::on_hit.store(false);

		auto key = _hotkey.k2evt(Keyboard::current_key);
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
			_running.store(false);
			break;
		default:
			break;
		}
		if ((!_running) || s != Stage::SNAKE_MENU) return;
		cast_change(_menu.arrow());
		flush();
	}
}

void StagePause::main(Stage& s) {
	// load_basic_assets
	clear_area(_pause.components());
	cast_change(_pause.components());
	cast_change(_pause.arrow());
	flush();
	Keyboard::current_key.store(key(KeyType::ASCII, 0));

	auto pause_window_running = true;
	while (pause_window_running) {

		std::unique_lock<std::mutex> pw_lck(Keyboard::kbmtx);
		while (!Keyboard::on_hit) Keyboard::kbcv.wait(pw_lck);
		Keyboard::on_hit.store(false);

		auto evt = _hotkey.k2evt(Keyboard::current_key);
		auto stage = _pause.on_event(evt);
		if (stage != Stage::NONE_STAGE) {
			switch (stage) {
			case Stage::GAME_CONTINUE:
			case Stage::SNAKE_GAME:
			case Stage::SNAKE_RESTART:
				clear_area(_pause.components());
				return;
			case Stage::SNAKE_MENU:
				s = stage;
				_running.store(false);
				return;
			case Stage::SNAKE_SETTINGS:
				s = stage;
				_running.store(false);
				return;
			case Stage::SNAKE_END:
				s = stage;
				_running.store(false);
				clear_all(_pause.resolution());
				return;
			default:break;
			}
		}
		clear_area(_pause.cleared());
		cast_change(_pause.casted());
		flush();
	}
}

void StageGameOver::main(Stage& s) {
	clear_area(_game_over.components());
	cast_change(_game_over.components());
	cast_change(_game_over.arrow());
	flush();

	auto game_over_window_running = true;
	while (game_over_window_running) {
		std::unique_lock<std::mutex> pw_lck(Keyboard::kbmtx);
		while (!Keyboard::on_hit) Keyboard::kbcv.wait(pw_lck);
		Keyboard::on_hit.store(false);

		auto evt = _hotkey.k2evt(Keyboard::current_key);
		auto stage = _game_over.on_event(evt);
		if (stage != Stage::NONE_STAGE) {
			switch (stage) {
			case Stage::SNAKE_RESTART:
				_running.store(false);
				return;
			case Stage::SNAKE_MENU:
				s = stage;
				_running.store(false);
				return;
			case Stage::SNAKE_END:
				s = stage;
				_running.store(false);
				clear_all(_game_over.resolution());
				return;
			default:break;
			}
		}
		clear_area(_game_over.cleared());
		cast_change(_game_over.casted());
		flush();
	}
}

void StageGame::main(Stage& s) {
	//std::thread show([&]() { game_show(s); });
	//std::thread key([&]() { game_keyboard(); });
	//show.join();
	//key.join();
	game_show(s);
	// TODO: pause window control thread
}

void StageGame::game_show(Stage& s) {
	if (rookie()) reset_all_assets();
	load_basic_assets();
	set_rookie(false);
	while (_running) {
		judge_key(s);
		if ((!_running) || 
			(s != Stage::SNAKE_GAME
			&& s != Stage::GAME_CONTINUE
			&& s!=Stage::SNAKE_RESTART)) break;

		if (check_died()) { 
			set_rookie(true);
			_game_over.main(s); 
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

void StageGame::judge_key(Stage& s) {
	auto last_key = Event::NONE_EVENT;
	auto event = _hotkey.k2evt(Keyboard::current_key);
	switch (event) {
	case Event::KEY_ESC:
	case Event::KEY_PAUSE:
		_pause.main(s);
		cast_change(_snake.components());
		cast_change(_map.foods());
		flush();
		Keyboard::current_key.store(key(KeyType::ASCII, 0));
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
}

void StageGame::load_basic_assets() {
	_running.store(true);
	clear_all(_map.resolution());
	_map.gen_food(_snake, _global);
	cast_change(_map.fence());
	cast_change(_snake.components());
	cast_change(_map.foods());
	flush();
	if(rookie())_snake.set_status(SnakeStatus::HEADING_RIGHT);
}

void StageSettings::main(Stage& s) {
	load_basic_assets();
	while (_running) {
		// TODO:: need to be fully rewrite
#ifdef _WIN64
		std::unique_lock<std::mutex> pw_lck(Keyboard::kbmtx);
		while (!Keyboard::on_hit) Keyboard::kbcv.wait(pw_lck);
		Keyboard::on_hit.store(false);

		auto k = Keyboard::current_key.load();
		auto e = _hotkey.k2evt(k);
		if (_setting_control.changing_hotkey()) {
			_setting_control.change_hotkey(e, k);
			cast_change(_setting_control.casted());
			flush();
			continue;
		}
	
		// main
		if (e == Event::KEY_ESC) return;
		else {
			_setting_control.on_event(e);
			clear_area(_setting_control.cleared());
			cast_change(_setting_control.casted());
			flush();
		}
#endif
	}
}


void StageSettings::load_basic_assets() {
	_running.store(true);
	_setting_control.reset_page();
	clear_all(_setting_control.resolution());
	cast_change(_setting_control.components());
	cast_change(_setting_control.current_page());
	cast_change(_setting_control.arrow());
	flush();
}