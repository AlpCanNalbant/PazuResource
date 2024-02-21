cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(PazuResource LANGUAGES CXX)
set(PAZU_RESGEN_PROJECT_NAME "PazuResourceGenerator" CACHE STRING "The name of the Pazu resource generator project." FORCE)

set(PAZU_RESOURCE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "Include directory of the PazuResource project." FORCE)
set(PazuResource_DEPS_HOME "${CMAKE_CURRENT_SOURCE_DIR}/deps")
set(WINAPICOMMON_SKIP_HEADERS TRUE)
add_subdirectory("${PazuResource_DEPS_HOME}/WinAPICommon")

# The function definition for the setting base directories of the Pazu Resource project library output files.
macro(set_pazures_install_dirs)
	set(options OVERWRITE)
	set(multiValueArgs TARGETS DESTINATIONS)
	cmake_parse_arguments(PARSED_ARGS "${options}" "dummy" "${multiValueArgs}" ${ARGN})
	foreach(target destination IN ZIP_LISTS PARSED_ARGS_TARGETS PARSED_ARGS_DESTINATIONS)
		if("${target}" STREQUAL "resource")
			set(CURRENT_TARGET PazuResource_INSTALL_HOME)
		elseif("${target}" STREQUAL "generator")
			set(CURRENT_TARGET PazuResourceGenerator_INSTALL_HOME)
		endif()
		if(${PARSED_ARGS_OVERWRITE})
			set(OVERWRITE_OPTION FORCE)
		else()
			set(OVERWRITE_OPTION)
		endif()

		set(${CURRENT_TARGET} "${destination}" CACHE STRING "The installation base directory of Pazu Resource project." ${OVERWRITE_OPTION})
	endforeach()
endmacro()

# Default installation directories of the Pazu Resource library and its generator's executable file.
set(PAZU_RESOURCE_DEFAULT_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/obj/${CMAKE_BUILD_TYPE}")
set(PAZU_RESOURCE_GENERATOR_DEFAULT_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
# Set the base directories of to their default values if they are not already setted.
set_pazures_install_dirs(TARGETS resource generator DESTINATIONS "${PAZU_RESOURCE_DEFAULT_INSTALL_DIR};${PAZU_RESOURCE_GENERATOR_DEFAULT_INSTALL_DIR}")

# If the user set the global property variable's value PAZU_RESOURCE_INSTALL_DIR and if PazuResource_INSTALL_HOME cached string variable equals to its default value which is
# PAZU_RESOURCE_DEFAULT_INSTALL_DIR in this case. Then set its value to the user provided global property value otherwise if not equals to the its default value,
# for an example edited from in the cmake gui application by the user and then even the global property variable is setted(not empty) before adding this sub
# directory to your cmake project, will be do not make any attempt to override cached string variable's value with the value of the global property variable
# which is PAZU_RESOURCE_INSTALL_DIR in this case.
get_property(PazuResourceInstallDir GLOBAL PROPERTY PAZU_RESOURCE_INSTALL_DIR)
if(NOT "${PazuResourceInstallDir}" STREQUAL "")
	if("${PazuResource_INSTALL_HOME}" STREQUAL "${PAZU_RESOURCE_DEFAULT_INSTALL_DIR}")
		set_pazures_install_dirs(TARGETS resource DESTINATIONS "${PazuResourceInstallDir}" OVERWRITE)
	endif()
endif()
# Same rules as PAZU_RESOURCE_INSTALL_DIR for the global property variable PAZU_RESOURCE_GENERATOR_INSTALL_DIR is also applied.
get_property(PazuResourceGeneratorInstallDir GLOBAL PROPERTY PAZU_RESOURCE_GENERATOR_INSTALL_DIR)
if(NOT "${PazuResourceGeneratorInstallDir}" STREQUAL "")
	if("${PazuResourceGenerator_INSTALL_HOME}" STREQUAL "${PAZU_RESOURCE_GENERATOR_DEFAULT_INSTALL_DIR}")
		set_pazures_install_dirs(TARGETS generator DESTINATIONS "${PazuResourceGeneratorInstallDir}" OVERWRITE)
	endif()
endif()

file(GLOB PazuResource_SOURCE CONFIGURE_DEPENDS "Pazu/*.hpp" "Pazu/*.inl" "Pazu/*.cpp")

add_library(${PROJECT_NAME} STATIC ${PazuResource_SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD ${WINAPICOMMON_CPP_VERSION} CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF LINKER_LANGUAGE CXX)
target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(${PROJECT_NAME} INTERFACE WinAPICommon)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

file(GLOB PazuResourceGenerator_SOURCE CONFIGURE_DEPENDS "Pazu/ResourceGenerator/*.hpp" "Pazu/ResourceGenerator/*.inl" "Pazu/ResourceGenerator/*.cpp")

add_executable(${PAZU_RESGEN_PROJECT_NAME} ${PazuResourceGenerator_SOURCE})
set_target_properties(${PAZU_RESGEN_PROJECT_NAME} PROPERTIES OUTPUT_NAME ResourceGenerator CXX_STANDARD ${WINAPICOMMON_CPP_VERSION} CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
target_link_libraries(${PAZU_RESGEN_PROJECT_NAME} PRIVATE mingw32 ${PROJECT_NAME} -static-libgcc -static-libstdc++ -static Threads::Threads)

message("Installing Pazu resource generator project ...")
install(TARGETS ${PROJECT_NAME} ${PAZU_RESGEN_PROJECT_NAME}
	ARCHIVE DESTINATION ${PazuResource_INSTALL_HOME}
	RUNTIME DESTINATION ${PazuResourceGenerator_INSTALL_HOME})
