cmake_minimum_required(VERSION 3.20)

project(Match3Prototype)

set(CMAKE_CXX_STANDARD 26)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.14
)

FetchContent_MakeAvailable(SDL3)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    Match3Prototype/*.cpp
)

add_executable(Match3Prototype ${SOURCES})

target_include_directories(Match3Prototype PRIVATE
    ${CMAKE_SOURCE_DIR}/Match3Prototype
    ${CMAKE_SOURCE_DIR}/Match3Prototype/Utils
    ${CMAKE_SOURCE_DIR}/Match3Prototype/Render
    ${CMAKE_SOURCE_DIR}/Match3Prototype/EntityCore
)

target_link_libraries(Match3Prototype PRIVATE SDL3::SDL3)

set_target_properties(Match3Prototype PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/bin/Release
)

add_custom_command(TARGET Match3Prototype POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL3::SDL3>
    $<TARGET_FILE_DIR:Match3Prototype>
)