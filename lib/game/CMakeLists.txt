set(
	HEADERS
	include/game/Ship.hpp
	include/game/Cell.hpp
	include/game/Marker.hpp
	include/game/Players.hpp
	include/game/Grid.hpp
	include/game/Common.hpp
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