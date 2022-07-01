cmake_minimum_required(VERSION 3.16)
project(CryptexAdventure)

# set up library
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
        FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 4.0.0
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
        set_directory_properties(PROPERTIES COMPILE_OPTIONS -Wno-missing-field-initializers)
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

# building an executable
add_executable(${PROJECT_NAME} 
    src/background.cpp
    src/character.cpp
    src/enemy.cpp
    src/game.cpp
    src/gameaudio.cpp
    src/gametextures.cpp
    src/hud.cpp
    src/main.cpp
    src/prop.cpp
    src/randomizer.cpp
    src/sprite.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_include_directories(${PROJECT_NAME} PRIVATE headers ${raylib_INCLUDE_DIRS})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

# set up assets
file(COPY sprites audio DESTINATION ${CMAKE_BINARY_DIR})