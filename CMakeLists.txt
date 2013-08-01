cmake_minimum_required(VERSION 2.6)

project(advspec)

set(PACKAGE "advspec")
set(PACKAGE_VERSION "b3")
set(PACKAGE_BUGREPORT "https://github.com/SBlue/advspec/issues")
set(PACKAGE_NAME "${PACKAGE}")
set(PACKAGE_STRING "${PACKAGE} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME "${PACKAGE}_${PACKAGE_VERSION}")
set(PACKAGE_URL "https://github.com/SBlue/advspec")
set(VERSION "${PACKAGE_VERSION}")

include_directories(${PROJECT_BINARY_DIR})
# Put final product in 'bin' folder
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Include source and hl2sdk
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${HL2SDK}/common)
include_directories(${HL2SDK}/public)
include_directories(${HL2SDK}/public/tier0)
include_directories(${HL2SDK}/public/tier1)
include_directories(${HL2SDK}/game/client)
include_directories(${HL2SDK}/game/shared)

set(HDR_PUBLIC
	src/advspec.h
	src/offsets.h
	src/stdafx.h
	src/vfuncs.h
	)

set(SOURCES
	src/advspec.cpp
	src/offsets.cpp
	src/vfuncs.cpp
	)

source_group("Header Files" FILES ${HDR_PUBLIC})
source_group("Source FIles" FILES ${SOURCES})

# Setup defines, compiler options and linker options for each platform
if(UNIX) # Common Unix settings
	#Not supported, exit with message
	message( FATAL_ERROR "Linux/OS X not supported yet." )
else() # Windows
	# Force use of static libs
	# Credit SteveL & brofield on StackOverflow
	foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINOFO)
		string(REGEX REPLACE "/MT" "/MD" ${flag_var} "${${flag_var}}")
	endforeach()
	
	add_definitions(/D_CRT_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_DEPRECATE)
	set(COMPILER_FLAGS "/EHsc /DR- /W3 /nologo /Zi /TP")
	set(LINKER_FLAGS "/MACHINE:X86 /subsystem:windows /NODEFAULTLIB:libc /NODEFAULTLIB:libcd")
	set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:libcmt")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libcmt")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${LINKER_FLAGS}")

# Create the plugin
add_library(advspec SHARED
	${HDR_PUBLIC}
	${SOURCES})

# Remove 'lib' prefix
set_target_properties(advspec PROPERTIES PREFIX "")

# Link sdk libraries
if(WIN32)
	target_link_libraries(advspec ${HL2SDK}/lib/public/tier0.lib)
	target_link_libraries(advspec ${HL2SDK}/lib/public/tier1.lib)
else()
	if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		target_link_libraries(advspec ${HL2SDK}/lib/osx32/libtier0.dylib)
	else()
		target_link_libraries(advspec ${HL2SDK}/lib/public/linux32/libtier0.so)
	endif()
endif()
