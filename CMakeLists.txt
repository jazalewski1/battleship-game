cmake_minimum_required(VERSION 3.17)
project(BattleshipGame LANGUAGES CXX)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(
	SFML
	GIT_REPOSITORY "https://github.com/SFML/SFML.git"
	GIT_TAG master
)
FetchContent_MakeAvailable(SFML)

set(CMAKE_CXX_STANDARD 17)
set(LINKER_LANGUAGE CXX)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(Battleship app/Main.cpp)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/game)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/gui)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/util)

target_link_libraries(
	Battleship
	Game
	GUI
	Utilities
	sfml-graphics
	sfml-window
	sfml-system
)