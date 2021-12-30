// #include "interface.h"
// #include "settings.h"

// class App {
// public:
// 	App(uint16_t res) : 
// 		_stage(Stage::SNAKE_MENU), 
// 		_running(true), 
// 		_menu(res), 
// 		_game(res),
// 		_settings(res)
// 	{
// 		// TODO: develop linux ver
// 		// hide cursor
// 		#ifdef WIN32
// 		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
// 		CONSOLE_CURSOR_INFO CursorInfo;
// 		GetConsoleCursorInfo(handle, &CursorInfo);
// 		CursorInfo.bVisible = false;
// 		SetConsoleCursorInfo(handle, &CursorInfo);
// 		#endif
// 	};

// 	void main();

// private:
// 	std::atomic<Stage> _stage;
// 	std::atomic<bool> _running;
// 	StageMenu _menu;
// 	StageGame _game;
// 	StageSettings _settings;
	
// };

// void App::main() {
// 	auto previous_stage = Stage::SNAKE_MENU;
// 	while (_running) {
// 		auto stage = _stage.load();
// 		switch (stage) {
// 		case Stage::SNAKE_MENU:
// 			if (_game.rookie()) _menu.fresh();
// 			else _menu.old();
// 			_menu.main(stage);
// 			previous_stage = Stage::SNAKE_MENU;
// 			break;
// 		case Stage::SNAKE_RESTART:
// 			_game.set_rookie(true);
// 		case Stage::GAME_CONTINUE:
// 		case Stage::SNAKE_GAME:
// 			_game.main(stage);
// 			previous_stage = Stage::SNAKE_GAME;
// 			break;
// 		case Stage::SNAKE_SETTINGS:
// 			_settings.main(stage);
// 			_game.set_hotkey(_settings.hotkey());
// 			_game.set_global(_settings.global());
// 			stage = previous_stage;
// 			break;
// 		case Stage::SNAKE_END:
// 			_running = false;
// 			break;
// 		default:
// 			break;
// 		}
// 		_stage.store(stage);
// 	}
// }


// int main()
// {
// 	App app(50);
// 	app.main();
// }

#include <string>
#include <iostream>
#include <unistd.h>
#include <iomanip>
using namespace std;

// a,b,c表示进度，为0-1之间的小数
void refresh(float a, float b, float c)
{
    if (a > 1)
        a = 1;
    if (b > 1)
        b = 1;
    if (c > 1)
        c = 1;
    // 整体放大
    int pa = a * 50;
    int pb = b * 50;
    int pc = c * 50;
    cout << "\33[3A"; // 终端光标向上移动三行
    cout << "[" + string(pa, '=') + ">" + string(50 - pa, ' ') << "]  " << a*100 << "%" << endl;
    cout << "[" + string(pb, '=') + ">" + string(50 - pb, ' ') << "]  " << b*100 << "%" << endl;
    cout << "[" + string(pc, '=') + ">" + string(50 - pc, ' ') << "]  " << c*100 << "%" << endl;
    fflush(stdout); // 刷新缓冲区
}

int main()
{
    cout << "\n\n\n";
    // cout << setiosflags(ios::fixed) << setprecision(2);
    float a=0,  b=0, c=0;
    for(int i = 0; i <= 100; i++)
    {
        a += 0.01;
        b += 0.02;
        c += 0.03;
        refresh(a,b,c);
        usleep(1000 * 100);
    }
    return 0;
}
