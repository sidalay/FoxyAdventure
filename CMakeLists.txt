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
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

# building an executable
add_executable(Cryptex 
    src/background.cpp
    src/basecharacter.cpp
    src/character.cpp
    src/enemy.cpp
    src/game.cpp
    src/gametextures.cpp
    src/hud.cpp
    src/main.cpp
    src/prop.cpp
    src/sprite.cpp
    src/treasure.cpp
)
target_link_libraries(Cryptex PRIVATE raylib)
target_include_directories(Cryptex PRIVATE headers ${raylib_INCLUDE_DIRS})
target_compile_features(Cryptex PRIVATE cxx_std_20)
target_compile_options(Cryptex PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Wpedantic -Wno-missing-field-initializers>)

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()

# set up assets
file(COPY sprites DESTINATION ${CMAKE_BINARY_DIR})