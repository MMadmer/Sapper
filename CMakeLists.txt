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
set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/.deps" CACHE PATH "" FORCE)

set(SAPPER_RUNTIME_DIR "${SAPPER_RUNTIME_DIR}" CACHE PATH "Directory for the runnable game output")
if(NOT SAPPER_RUNTIME_DIR)
    set(SAPPER_RUNTIME_DIR "${CMAKE_BINARY_DIR}/runtime" CACHE PATH "Directory for the runnable game output" FORCE)
endif()

add_subdirectory("${SAPPER_SFML_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/.deps/sfml")

add_executable(sapper WIN32
    Saper/src/main.cpp
    Saper/src/app/GameApp.cpp
    Saper/src/game/Board.cpp
    Saper/src/render/Assets.cpp
    Saper/include/sapper/app/GameApp.hpp
    Saper/include/sapper/game/Board.hpp
    Saper/include/sapper/game/Difficulty.hpp
    Saper/include/sapper/render/Assets.hpp
    Saper/resources/windows/Sapper.rc
    Saper/resources/windows/resource.h
)

target_include_directories(sapper PRIVATE "${CMAKE_SOURCE_DIR}/Saper/include")
target_link_libraries(sapper PRIVATE SFML::Graphics)

if(WIN32)
    target_link_libraries(sapper PRIVATE SFML::Main)
endif()

set_target_properties(sapper PROPERTIES
    OUTPUT_NAME "Sapper"
    RUNTIME_OUTPUT_DIRECTORY "${SAPPER_RUNTIME_DIR}"
    PDB_OUTPUT_DIRECTORY "${SAPPER_RUNTIME_DIR}"
    COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/.pdb"
)

add_custom_command(TARGET sapper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rm -rf "$<TARGET_FILE_DIR:sapper>/assets"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:sapper>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/Saper/assets/tileset.png"
            "$<TARGET_FILE_DIR:sapper>/assets/tileset.png"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/Saper/assets/Context_Reprise_SSi.ttf"
            "$<TARGET_FILE_DIR:sapper>/assets/Context_Reprise_SSi.ttf"
)
