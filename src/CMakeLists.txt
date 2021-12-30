add_library(snake_lib STATIC
	base.cpp
	game.cpp
	interface.cpp
	settings.cpp
)

link_libraries(snake_lib)

find_package(Threads)

ADD_EXECUTABLE(snake snake.cpp)

target_link_libraries(snake ${CMAKE_THREAD_LIBS_INIT})
