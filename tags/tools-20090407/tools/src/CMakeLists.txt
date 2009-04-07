cmake_minimum_required (VERSION 2.6) 
project (nsaio)

set(EXECUTABLE_OUTPUT_PATH bin-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
set(CMAKE_BUILD_TYPE Release)
add_definitions(-DNONS_SVN)

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
	nsaio_src/Archive.cpp
	nsaio_src/ArchivedFile.cpp
	nsaio_src/ArchivedFile.h
	nsaio_src/Archive.h
	nsaio_src/Common.h
	nsaio_src/Encryption.cpp
	nsaio_src/Encryption.h
	nsaio_src/FileIO.cpp
	nsaio_src/FileIO.h
	nsaio_src/Functions.cpp
	nsaio_src/Functions.h
	nsaio_src/TreeNode.cpp
	nsaio_src/TreeNode.h
	nsaio_src/UTF.cpp
	nsaio_src/UTF.h
	NONS_src/SJIS.table.cpp
)

target_link_libraries(nsaio
	${to_link}
)