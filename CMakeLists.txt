cmake_minimum_required(VERSION 3.8)

project(WebGL)

add_definitions(-std=c++20)
set(CMAKE_CXX_STANDARD 20)

if (DEFINED EMSCRIPTEN)
    add_executable(Viewer 
        viewer.cpp 
        renderpass.cpp
        vertexbuffer.cpp)

    # Set the output file extension to .js (for WebAssembly)
    set(CMAKE_EXECUTABLE_SUFFIX .html)

    # Target compile options and link options specific to WebAssembly with WebGL and SDL2
    target_compile_options(Viewer PUBLIC "-Os" "-sUSE_SDL=2")
    target_link_options(Viewer PUBLIC "-sUSE_WEBGL2=1" "-sUSE_SDL=2" "-sALLOW_MEMORY_GROWTH=1" "-sFULL_ES2=1" "-sWASM=1" "--shell-file=../index.html")
    
else()
    add_library(Viewer SHARED calc.cpp)
endif()
