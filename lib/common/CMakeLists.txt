set(
	HEADERS
	include/common/Common.hpp
)

set(
	SOURCES
)

add_library(
	Common
	${HEADERS}
	${SOURCES}
)

set_target_properties(
	Common
	PROPERTIES
	LINKER_LANGUAGE CXX
)

target_include_directories(
	Common
	PUBLIC include
)

target_link_libraries(
	Common
	sfml-graphics
)