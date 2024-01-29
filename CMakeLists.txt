cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(PazuResource LANGUAGES CXX)
set(PAZU_RESGEN_PROJECT_NAME "PazuResourceGenerator" CACHE STRING "The name of the Pazu resource generator project." FORCE)

set(PAZU_RESOURCE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE STRING "Include directory of the PazuResource project." FORCE)
set(PazuResource_DEPS_HOME "${CMAKE_CURRENT_SOURCE_DIR}/deps")
add_subdirectory("${PazuResource_DEPS_HOME}/WinAPICommon")

# Default installation directory of the resource generator executable file.
set(PAZU_RESOURCE_GENERATOR_DEFAULT_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
# The function definition for the setting base directory of the resource generator executable file.
function(set_resgen_install_dir dir)
	set(PazuResourceGenerator_INSTALL_HOME "${dir}" CACHE STRING "The installation base directory of Pazu resource generator project." FORCE)
endfunction()

set(PazuResource_INSTALL_HOME "${CMAKE_CURRENT_SOURCE_DIR}/obj/${CMAKE_BUILD_TYPE}" CACHE STRING "The installation base directory of Pazu resource generator project." FORCE)
# Set the base directory of the resource generator to its default value.
set_resgen_install_dir("${PAZU_RESOURCE_GENERATOR_DEFAULT_INSTALL_DIR}")

# If user set the global property variable's value PAZU_RESOURCE_GENERATOR_INSTALL_DIR and if PazuResourceGenerator_INSTALL_HOME equals to its default value
# then set its value to user provided global property otherwise(edited from in cmake gui application by the user) do not override cache string variable.
get_property(PazuResourceGeneratorInstallDir GLOBAL PROPERTY PAZU_RESOURCE_GENERATOR_INSTALL_DIR)
if(NOT "${PazuResourceGeneratorInstallDir}" STREQUAL "")
	if("${PazuResourceGenerator_INSTALL_HOME}" STREQUAL "${PAZU_RESOURCE_GENERATOR_DEFAULT_INSTALL_DIR}")
		set_resgen_install_dir("${PazuResourceGeneratorInstallDir}")
	endif()
endif()

file(GLOB PazuResource_SOURCE CONFIGURE_DEPENDS "./Pazu/*.hpp" "./Pazu/*.inl" "./Pazu/*.cpp")

add_library(${PROJECT_NAME} STATIC ${PazuResource_SOURCE})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD ${WINAPICOMMON_CPP_VERSION} CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF LINKER_LANGUAGE CXX)
target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(${PROJECT_NAME} INTERFACE WinAPICommon)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

file(GLOB PazuResourceGenerator_SOURCE CONFIGURE_DEPENDS "./Pazu/Generator/*.hpp" "./Pazu/Generator/*.inl" "./Pazu/Generator/*.cpp")

add_executable(${PAZU_RESGEN_PROJECT_NAME} ${PazuResourceGenerator_SOURCE})
set_target_properties(${PAZU_RESGEN_PROJECT_NAME} PROPERTIES OUTPUT_NAME ResourceGenerator CXX_STANDARD ${WINAPICOMMON_CPP_VERSION} CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
target_link_libraries(${PAZU_RESGEN_PROJECT_NAME} PRIVATE mingw32 ${PROJECT_NAME} -static-libgcc -static-libstdc++ -static Threads::Threads)

message("Installing Pazu resource generator project")
install(TARGETS ${PROJECT_NAME} ${PAZU_RESGEN_PROJECT_NAME}
	ARCHIVE DESTINATION ${PazuResource_INSTALL_HOME}
	RUNTIME DESTINATION ${PazuResourceGenerator_INSTALL_HOME})

file(GLOB_RECURSE PazuResourceGenerator_EXAMPLE_DATA CONFIGURE_DEPENDS "samples/*")

if(${CMAKE_BUILD_TYPE} STREQUAL Debug)
	message("Installing sample resource files of the Pazu resource generator project ...")
	install(DIRECTORY samples/ DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug/data")
endif()
