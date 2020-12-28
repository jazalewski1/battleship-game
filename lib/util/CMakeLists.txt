set(
	HEADERS
	include/util/Sfutils.hpp
	include/util/Random.hpp	
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