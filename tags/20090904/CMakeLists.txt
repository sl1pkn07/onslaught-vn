cmake_minimum_required (VERSION 2.6) 
project (onslaught)

include_directories(include)

set(EXECUTABLE_OUTPUT_PATH bin-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
set(CMAKE_BUILD_TYPE Release)
add_definitions(-DNONS_SVN)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR MINGW OR MSYS OR CYGWIN)
set(CMAKE_MODULE_LINKER_FLAGS ${CMAKE_MODULE_LINKER_FLAGS} -s)
endif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR MINGW OR MSYS OR CYGWIN)

set(LIBS
	${LIBS}
	SDL
	SDL_image
	SDL_mixer
	bz2
	freetype
)

add_executable(ONSlaught
	src/CommandLineOptions.cpp
	src/Everything.cpp
	src/Functions.cpp
	src/Globals.cpp
	src/ONSlaught.cpp
	src/SJIS.table.cpp
	src/UTF.cpp
	src/ThreadManager.cpp
	src/IO_System/ConfigFile.cpp
	src/IO_System/FileIO.cpp
	src/IO_System/INIfile.cpp
	src/IO_System/INIParser.tab.cpp
	src/IO_System/InputHandler.cpp
	src/IO_System/IOFunctions.cpp
	src/IO_System/SaveFile.cpp
	src/IO_System/StdOut.cpp
	src/IO_System/Audio/Audio.cpp
	src/IO_System/Audio/CachedSound.cpp
	src/IO_System/Audio/Music.cpp
	src/IO_System/Audio/SoundCache.cpp
	src/IO_System/Audio/SoundEffect.cpp
	src/IO_System/Graphics/SDL_ttf.c
	src/IO_System/Graphics/SDL_bilinear.c
	src/IO_System/Graphics/Button.cpp
	src/IO_System/Graphics/ButtonLayer.cpp
	src/IO_System/Graphics/Cursor.cpp
	src/IO_System/Graphics/FontCache.cpp
	src/IO_System/Graphics/GFX.cpp
	src/IO_System/Graphics/Layer.cpp
	src/IO_System/Graphics/Lookback.cpp
	src/IO_System/Graphics/Menu.cpp
	src/IO_System/Graphics/ScreenSpace.cpp
	src/IO_System/Graphics/StandardOutput.cpp
	src/IO_System/Graphics/VirtualScreen.cpp
	src/IO_System/SAR/Archive.cpp
	src/IO_System/SAR/FileLog.cpp
	src/IO_System/SAR/Image.cpp
	src/IO_System/SAR/ImageLoader.cpp
	src/IO_System/Script/commandPreParser.tab.cpp
	src/IO_System/Script/MacroParser.cpp
	src/IO_System/Script/MacroParser.tab.cpp
	src/IO_System/Script/Preprocessor.cpp
	src/IO_System/Script/Script.cpp
	src/IO_System/Script/sha1.cpp
	src/Processing/ExpressionParser.tab.cpp
	src/Processing/ParsedLine.cpp
	src/Processing/ScriptInterpreter.cpp
	src/Processing/ScriptInterpreter_commandsAI.cpp
	src/Processing/ScriptInterpreter_commandsJR.cpp
	src/Processing/ScriptInterpreter_commandsSZ.cpp
	src/Processing/ScriptInterpreter_savegame.cpp
	src/Processing/Variable.cpp
	src/Processing/VariableMember.cpp
	src/Processing/VariableStore.cpp
)

target_link_libraries(ONSlaught ${LIBS})
