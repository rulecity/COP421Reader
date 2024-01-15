cmake_minimum_required(VERSION 3.19)
project(COPDumper)

set(CMAKE_CXX_STANDARD 17)

add_executable(COPDumper main.cpp
		COPDumper.cpp
		COPDumper.h)

# to override default EXE name
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "COPDumper")

# dependencies
find_package(mpo 2.0 REQUIRED)
target_link_libraries(COPDumper PUBLIC mpo)

find_package(mposerial 1.0 REQUIRED)

if (WIN32)
	target_link_libraries(COPDumper PUBLIC mposerial -static)
else()
	target_link_libraries(COPDumper PUBLIC mposerial)
endif()

# to help me figure out what these vars are set to (handy!)
message("CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")
message("CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

if (MSVC)
	# see https://devblogs.microsoft.com/cppblog/msvc-now-correctly-reports-__cplusplus/
	target_compile_options(COPDumper PUBLIC "/Zc:__cplusplus")
endif()

# So that anything that links to our lib gets the headers for all dependencies
# (configured include files go to the build folder, not the source folder (by design))
target_include_directories(COPDumper PUBLIC

		# this will be populated when installing but blank when building
		# for client in install mode
		$<INSTALL_INTERFACE:${include_dest}>
		$<INSTALL_INTERFACE:${lib_dest}> # for config.h file

		# this will be populated when building but blank when installing
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>)

install(TARGETS COPDumper)