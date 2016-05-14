cmake_minimum_required(VERSION 2.8.12)
project(Dawn)

set(DAWN_VERSION_MAJOR 0)
set(DAWN_VERSION_MINOR 0)
set(DAWN_VERSION_PATCH 1)
set(DAWN_VERSION ${DAWN_VERSION_MAJOR}.${DAWN_VERSION_MINOR}.${DAWN_VERSION_PATCH})


list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/libs/cmake")
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if(UNIX)
	#force C++11 flag and 32bit compilation
	set(CMAKE_CXX_FLAGS "-m32 -std=c++11 -fvisibility=hidden")
	set(CMAKE_C_FLAGS "-m32 -fvisibility=hidden")
	set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)
endif()


set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.57 QUIET)

if(NOT Boost_FOUND)
	set(BOOST_ROOT "${BOOST_ROOT}" CACHE PATH "Set the Boost root folder.")
	set(BOOST_LIBRARYDIR "${BOOST_LIBRARYDIR}" CACHE PATH "Set the Boost library folder.")
	mark_as_advanced(CLEAR BOOST_ROOT BOOST_LIBRARYDIR)
	message(FATAL_ERROR "Could NOT find Boost; specify additional paths.")
endif()
mark_as_advanced(BOOST_ROOT BOOST_LIBRARYDIR Boost_DIR)

include_directories(${Boost_INCLUDE_DIR})
if(MSVC)
	#needed for Boost's auto-link feature
	link_directories(${BOOST_LIBRARYDIR})
endif()


find_package(MySQLCAPI 5.5.40 REQUIRED)
include_directories(${MYSQLCAPI_INCLUDE_DIR})


if(MSVC)
	set(YAML_CPP_ROOT "${YAML_CPP_ROOT}" CACHE PATH "Path to yaml-cpp")
	if(NOT YAML_CPP_ROOT)
		message(FATAL_ERROR "YAML-CPP path required")
	endif()
	find_library(
		YAML_CPP_LIBRARY
		NAMES
		"libyaml-cppmd.lib"
		HINTS
		${YAML_CPP_ROOT}
		PATH_SUFFIXES
		lib
	)
	if(NOT YAML_CPP_LIBRARY)
		message(FATAL_ERROR "Could not find libyaml-cppmd.lib")
	endif()
else()
	find_library(yaml-cpp REQUIRED)
endif()
include_directories("${YAML_CPP_ROOT}/include")


#set(CRYPTOPP_INCLUDE "${CRYPTOPP_INCLUDE}" CACHE PATH "Path to cryptopp include directory")
#if(NOT CRYPTOPP_INCLUDE)
#	message(FATAL_ERROR "Cryptopp include path required")
#endif()
#set(CRYPTOPP_LIBRARY "${CRYPTOPP_LIBRARY}" CACHE FILEPATH "Filepath to cryptopp library")
#if(NOT CRYPTOPP_LIBRARY)
#	message(FATAL_ERROR "Cryptopp library filepath required")
#endif()
#include_directories(${CRYPTOPP_INCLUDE})


set(SAMP_SDK_ROOT "${PROJECT_SOURCE_DIR}/libs/sdk")
find_package(SAMPSDK REQUIRED)
find_package(SAMPGDK REQUIRED)
include_directories(${SAMPGDK_INCLUDE_DIRS} ${SAMPSDK_INCLUDE_DIR})
link_directories(${SAMPGDK_LIBRARY_DIRS})
add_definitions(-DSAMPGDK_CPP_WRAPPERS -D_CRT_SECURE_NO_WARNINGS -DNOMINMAX)


add_subdirectory(src)
