cmake_minimum_required(VERSION 3.24)

project(Sapper LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not supported. Use a CMake preset or point CMake at a build directory.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SAPPER_SFML_VERSION "3.0.2")
set(SAPPER_SFML_SOURCE_DIR "${CMAKE_SOURCE_DIR}/.deps/SFML-${SAPPER_SFML_VERSION}" CACHE PATH "Path to a local SFML source checkout")

if(NOT EXISTS "${SAPPER_SFML_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "SFML ${SAPPER_SFML_VERSION} was not found at '${SAPPER_SFML_SOURCE_DIR}'. "
        "Run build.bat or build.sh to bootstrap it automatically, or clone SFML there manually."
    )
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_WINDOW ON CACHE BOOL "" FORCE)
set(SFML_BUILD_GRAPHICS ON CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_CONFIGURE_EXTRAS OFF CACHE BOOL "" FORCE)
set(SFML_GENERATE_PDB OFF CACHE BOOL "" FORCE)

add_subdirectory("${SAPPER_SFML_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/_deps/sfml")

message(STATUS "SFML ${SAPPER_SFML_VERSION} is pinned for the build and the game sources target SFML 3 while keeping the original gameplay logic largely intact.")

add_executable(sapper WIN32
    Saper/main.cpp
    Saper/cell.cpp
    Saper/globals.cpp
    Saper/cell.h
    Saper/globals.h
    Saper/Sapper1.rc
    Saper/resource1.h
)

target_include_directories(sapper PRIVATE "${CMAKE_SOURCE_DIR}/Saper")
target_link_libraries(sapper PRIVATE SFML::Graphics)

if(WIN32)
    target_link_libraries(sapper PRIVATE SFML::Main)
endif()

set_target_properties(sapper PROPERTIES
    OUTPUT_NAME "Sapper"
)

add_custom_command(TARGET sapper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/Saper/assets"
            "$<TARGET_FILE_DIR:sapper>/assets"
)
