set(
	HEADERS
	include/gui/Button.hpp
	include/gui/Guitext.hpp
	include/gui/Particles.hpp
	include/gui/Sfline.hpp
	include/gui/Endscreen.hpp	
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