cmake_minimum_required (VERSION 3.0.0)
project(video_player)

include_directories(../../include ./include)

set(EXECUTABLE_OUTPUT_PATH .)
set(CMAKE_BUILD_TYPE Release)

set(LIBS
	${LIBS}
	avformat
	avcodec
	avutil
	SDL
	openal
)

add_library(video_player SHARED
	video_player.cpp
	../common.cpp
	../font.cpp
	../../src/Thread.cpp
)

target_link_libraries(video_player ${LIBS})

install(TARGETS video_player
        DESTINATION ${CMAKE_INSTALL_DATADIR}/ONSlaught
)
