cmake_minimum_required(VERSION 3.21)

project(abyss_crown VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

file(GLOB ABYSS_CROWN_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(abyss_crown ${ABYSS_CROWN_SOURCES})
target_include_directories(abyss_crown PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(abyss_crown PRIVATE SDL2::SDL2 SDL2_ttf::SDL2_ttf)

if(TARGET SDL2::SDL2main)
    target_link_libraries(abyss_crown PRIVATE SDL2::SDL2main)
endif()

if(MSVC)
    target_compile_options(abyss_crown PRIVATE /W4 /utf-8)
else()
    target_compile_options(abyss_crown PRIVATE -Wall -Wextra -pedantic)
endif()

add_custom_command(TARGET abyss_crown POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:abyss_crown>/assets"
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/config"
        "$<TARGET_FILE_DIR:abyss_crown>/config"
    COMMENT "Copying game assets and config next to the executable"
)

if(WIN32)
    add_custom_command(TARGET abyss_crown POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            $<TARGET_RUNTIME_DLLS:abyss_crown>
            "$<TARGET_FILE_DIR:abyss_crown>"
        COMMAND_EXPAND_LISTS
        COMMENT "Copying Windows runtime DLLs"
    )
endif()
