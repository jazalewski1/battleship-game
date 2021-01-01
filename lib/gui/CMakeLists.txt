set(
	HEADERS
	include/gui/Button.hpp
	include/gui/Endscreen.hpp	
	include/gui/Text.hpp
)

set(
	SOURCES
)

add_library(
	GUI
	${HEADERS}
	${SOURCES}
)

set_target_properties(
	GUI
	PROPERTIES
	LINKER_LANGUAGE CXX
)

target_include_directories(
	GUI
	PUBLIC include
)

target_link_libraries(
	GUI
	Common
	Utilities
	sfml-graphics
	sfml-window
	sfml-system
)