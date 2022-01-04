#include "interface.h"
#include "settings.h"

class App {
public:
	App(uint16_t res) : 
		_stage(Stage::SNAKE_MENU), 
		_running(true), 
		_mainkb(),
		_menu(res),
		_game(res),
		_settings(res)
	{
#ifdef _WIN64
		// hide cursor
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO CursorInfo;
		GetConsoleCursorInfo(handle, &CursorInfo);
		CursorInfo.bVisible = false;
		SetConsoleCursorInfo(handle, &CursorInfo);
#endif
	};

	void main();

private:
	void game();

private:
	std::atomic<Stage> _stage;
	std::atomic<bool> _running;
	StageMenu _menu;
	StageGame _game;
	StageSettings _settings;
	Keyboard _mainkb;
};

void App::main() {
	std::thread game([&]() { game(); });
	std::thread kb([&]() { _mainkb.main(); });
	game.join();
	kb.join();
}

void App::game() {
	auto previous_stage = Stage::SNAKE_MENU;
	while (_running) {
		auto stage = _stage.load();
		switch (stage) {
		case Stage::SNAKE_MENU:
			if (_game.rookie()) _menu.fresh();
			else _menu.old();
			_menu.main(stage);
			previous_stage = Stage::SNAKE_MENU;
			break;
		case Stage::SNAKE_RESTART:
			_game.set_rookie(true);
		case Stage::GAME_CONTINUE:
		case Stage::SNAKE_GAME:
			_game.main(stage);
			previous_stage = Stage::SNAKE_GAME;
			break;
		case Stage::SNAKE_SETTINGS:
			_settings.main(stage);
			_game.set_hotkey(_settings.hotkey());
			_game.set_global(_settings.global());
			stage = previous_stage;
			break;
		case Stage::SNAKE_END:
			_running = false;
			break;
		default:
#ifdef _WIN64
			throw std::exception("invalid stage variable!");
#endif
#ifdef __linux__
			throw std::exception();
#endif
			break;
		}
		_stage.store(stage);
	}
}



int main()
{
	App app(50);
	app.main();
}


