cmake_minimum_required(VERSION 3.16)
project(chip8 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chip8
    src/main.cpp
    src/chip8.cpp
    src/platform.cpp
    src/beeper.cpp
    src/chip8.hpp
    src/platform.hpp
    src/constants.hpp
    src/beeper.hpp
)

if(EMSCRIPTEN)
    # SDL2 comes from emscripten's own port, not find_package.
    set(CMAKE_EXECUTABLE_SUFFIX ".js")

    target_compile_options(chip8 PRIVATE -sUSE_SDL=2)
    target_link_options(chip8 PRIVATE
        -sUSE_SDL=2
        -sMODULARIZE=1
        -sEXPORT_ES6=1
        -sEXPORT_NAME=createChip8
        -sALLOW_MEMORY_GROWTH=1
        -sEXIT_RUNTIME=0
        -sEXPORTED_RUNTIME_METHODS=ccall,cwrap,FS
        -sEXPORTED_FUNCTIONS=_main,_chip8_load_rom
        --preload-file "${CMAKE_SOURCE_DIR}/roms@/roms"
    )

    set_target_properties(chip8 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/web/public/wasm")
else()
    find_package(SDL2 REQUIRED)

    if(TARGET SDL2::SDL2main)
        target_link_libraries(chip8 PRIVATE SDL2::SDL2main)
    endif()

    target_link_libraries(chip8 PRIVATE SDL2::SDL2)

    add_custom_command(TARGET chip8 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/roms $<TARGET_FILE_DIR:chip8>/roms
    )
endif()
