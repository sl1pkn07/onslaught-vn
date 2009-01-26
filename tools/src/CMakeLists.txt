cmake_minimum_required (VERSION 2.6) 
project (nsaio)

set(to_link
	boost_filesystem
#boost_system
	bz2
)

add_executable(nsaio.exe
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

target_link_libraries(nsaio.exe
	${to_link}
)