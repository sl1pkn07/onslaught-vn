cmake_minimum_required (VERSION 3.0.0)
project(svg_loader)

set(EXECUTABLE_OUTPUT_PATH .)
set(CMAKE_BUILD_TYPE Release)

include(FindPkgConfig)
pkg_check_modules(LIBSRVG librsvg-2.0)


include_directories(../include
	${LIBSRVG_INCLUDE_DIRS}
)

set(CMAKE_CXX_FLAGS
	${CMAKE_CXX_FLAGS}
)

set(LIBS
	${LIBS}
	SDL
	${LIBSRVG_LIBRARIES}
)

add_library(svg_loader SHARED
	svg_loader.cpp
)

target_link_libraries(svg_loader ${LIBS})

install(TARGETS svg_loader
        DESTINATION ${CMAKE_INSTALL_DATADIR}/ONSlaught
)
