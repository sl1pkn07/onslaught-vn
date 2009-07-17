cmake_minimum_required (VERSION 2.6) 
project (nsaio)

set(EXECUTABLE_OUTPUT_PATH bin-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
set(CMAKE_BUILD_TYPE Release)
add_definitions(-DNONS_SVN -DTOOLS_NSAIO)

#include_directories(/usr/include/boost-1_37)

set(to_link
	boost_filesystem
#boost_system
	bz2
)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR MINGW OR MSYS OR CYGWIN)
set(CMAKE_MODULE_LINKER_FLAGS ${CMAKE_MODULE_LINKER_FLAGS} -s)
endif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR MINGW OR MSYS OR CYGWIN)

add_executable(nsaio
	nsaio.cpp
	NONS_src/Archive.cpp
	NONS_src/ArchivedFile.cpp
	NONS_src/ArchivedFile.h
	NONS_src/Archive.h
	NONS_src/Common.h
	NONS_src/FileIO.cpp
	NONS_src/FileIO.h
	NONS_src/Functions.cpp
	NONS_src/Functions.h
	NONS_src/TreeNode.cpp
	NONS_src/TreeNode.h
	NONS_src/UTF.cpp
	NONS_src/UTF.h
	NONS_src/SJIS.table.cpp
)

target_link_libraries(nsaio
	${to_link}
)