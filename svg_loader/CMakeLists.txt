cmake_minimum_required (VERSION 2.4.7)
project(svg_loader)

set(EXECUTABLE_OUTPUT_PATH .)
set(CMAKE_BUILD_TYPE Release)

include(FindPkgConfig)
find_package(Qt5 COMPONENTS Widgets Svg)
pkg_check_modules(LIBSRVG librsvg-2.0)


include_directories(../include
	${QT5_INCLUDE_DIRS}
	${LIBSRVG_INCLUDE_DIRS}
)

set(CMAKE_CXX_FLAGS
	${CMAKE_CXX_FLAGS}
)

set(LIBS
	${LIBS}
	SDL
	${QT5_LIBRARIES}
	${LIBSRVG_LIBRARIES}
)

add_library(svg_loader SHARED
	svg_loader.cpp
)

target_link_libraries(svg_loader ${LIBS})