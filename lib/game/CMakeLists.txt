set(
	HEADERS
	include/game/Cell.hpp
	include/game/Game.hpp
	include/game/Grid.hpp
	include/game/HumanPlayer.hpp
	include/game/Marker.hpp
	include/game/Player.hpp
	include/game/Ship.hpp
)

set(
	SOURCES
)

add_library(
	Game
	${HEADERS}
	${SOURCES}
)

set_target_properties(
	Game
	PROPERTIES
	LINKER_LANGUAGE CXX
)

target_include_directories(
	Game
	PUBLIC include
)

target_link_libraries(
	Game
	Common
	Utilities
	sfml-graphics
	sfml-window
	sfml-system
)