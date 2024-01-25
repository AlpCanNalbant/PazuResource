cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(PazuResource LANGUAGES CXX)
set(PAZU_RESGEN_PROJECT_NAME "PazuResourceGenerator" CACHE STRING "The name of the Pazu resource generator project." FORCE)

set(PazuResource_DEPS_HOME "${CMAKE_CURRENT_SOURCE_DIR}/deps")
add_subdirectory("${PazuResource_DEPS_HOME}/WinAPICommon")

set(PazuResource_INSTALL_HOME "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}" CACHE STRING "The installation base directory of Pazu resource generator project." FORCE)

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
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})
install(TARGETS ${PROJECT_NAME} ${PAZU_RESGEN_PROJECT_NAME}
    RUNTIME DESTINATION ${PazuResource_INSTALL_HOME}
	ARCHIVE DESTINATION ${PazuResource_INSTALL_HOME})

file(GLOB_RECURSE PazuResourceGenerator_EXAMPLE_DATA CONFIGURE_DEPENDS "samples/*")

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
	message("Installing sample resource files of the Pazu resource generator project")
	install(DIRECTORY samples/ DESTINATION "bin/Debug/data")
endif()
