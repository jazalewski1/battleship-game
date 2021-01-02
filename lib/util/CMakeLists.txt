set(
	HEADERS
	include/util/HashVector2.hpp
	include/util/RandomGenerator.hpp
)

set(
	SOURCES
)

add_library(
	Utilities
	${HEADERS}
	${SOURCES}
)

set_target_properties(
	Utilities
	PROPERTIES
	LINKER_LANGUAGE CXX
)

target_include_directories(
	Utilities
	PUBLIC include
)

target_link_libraries(
	Utilities
	sfml-graphics
	sfml-window
	sfml-system
)