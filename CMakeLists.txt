cmake_minimum_required (VERSION 3.0.0)
project(onslaught)

include(GNUInstallDirs)

include_directories(include)

set(EXECUTABLE_OUTPUT_PATH .)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)
add_definitions(-DNONS_SVN)
add_definitions(-DBUILD_ONSLAUGHT -DHAVE_INTTYPES_H -DSDL_HAS_64BIT_TYPE -w -fno-exceptions)

set(LIBS
    ${LIBS}
    SDL_image
    SDL
    bz2
    freetype
    FLAC
    vorbisfile
    vorbis
    ogg
    jpeg
    png
    tiff
    mikmod
    openal
    mpg123
    dl
    z
    pthread
)

add_executable(ONSlaught
               src/Archive.cpp
               src/Audio.cpp
               src/AudioFormats.cpp
               src/ErrorCodes.cpp
               src/ExpressionParser.tab.cpp
               src/FileLog.cpp
               src/Functions.cpp
               src/GFX.cpp
               src/GUI.cpp
               src/Image.cpp
               src/INIfile.cpp
               src/INIParser.tab.cpp
               src/IOFunctions.cpp
               src/libFLAC++/metadata.cpp
               src/libFLAC++/stream_decoder.cpp
               src/libFLAC++/stream_encoder.cpp
               src/libtimidity/common.c
               src/libtimidity/instrum.c
               src/libtimidity/instrum_dls.c
               src/libtimidity/mix.c
               src/libtimidity/output.c
               src/libtimidity/playmidi.c
               src/libtimidity/readmidi.c
               src/libtimidity/resample.c
               src/libtimidity/stream.c
               src/libtimidity/tables.c
               src/libtimidity/timidity.c
               src/LZMA.c
               src/ONSlaught.cpp
               src/OpenAL.cpp
               src/Options.cpp
               src/Plugin/LibraryLoader.cpp
               src/ScreenSpace.cpp
               src/Script.cpp
               src/ScriptInterpreter.cpp
               src/sha1.cpp
               src/SJIS.table.cpp
               src/Thread.cpp
               src/ThreadManager.cpp
               src/tinyxml/tinyxml.cpp
               src/tinyxml/tinyxmlerror.cpp
               src/tinyxml/tinyxmlparser.cpp
               src/VariableStore.cpp
               src/VirtualScreen.cpp
)

if(UNIX)
  add_definitions(-DREAD_LINKING_WARNING)
  add_library(plugin SHARED src/Plugin/Plugin.cpp)
  add_subdirectory(svg_loader)
  add_subdirectory(video_player/FFmpeg)
  # FUCK YOU VLC!
endif()

target_link_libraries(ONSlaught ${LIBS})

install(TARGETS ONSlaught
        DESTINATION ${CMAKE_INSTALL_DATADIR}/ONSlaught
)

if(UNIX)
  install(TARGETS plugin
          DESTINATION ${CMAKE_INSTALL_DATADIR}/ONSlaught
  )

  configure_file("Launcher.sh.in" "Launcher.sh" @ONLY)

  install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/Launcher.sh
          DESTINATION ${CMAKE_INSTALL_BINDIR}
          RENAME ONSlaught
  )
endif()