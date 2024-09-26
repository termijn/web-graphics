cmake_minimum_required(VERSION 3.8)

project(WebGL)

add_definitions(-std=c++20)
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE ViewerSRC ${CMAKE_CURRENT_SOURCE_DIR}/src *.cpp)

if (DEFINED EMSCRIPTEN)
    add_executable(Viewer ${ViewerSRC})

    set(CMAKE_EXECUTABLE_SUFFIX .html)

    target_compile_options(Viewer PUBLIC 
        -Os 
        -sUSE_SDL=2)
    target_link_options(Viewer PUBLIC
        -sMIN_WEBGL_VERSION=2
        -sMAX_WEBGL_VERSION=2
        -sUSE_WEBGL2=1 
        -sUSE_SDL=2 
        -sALLOW_MEMORY_GROWTH=1 
        -sFULL_ES3=1 
        -sWASM=1 
        --shell-file=../index.html
        --preload-file=${CMAKE_CURRENT_SOURCE_DIR}/shaders@/shaders
        )

    file(GLOB_RECURSE SHADER_REFERENCES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.glsl)
    set_property(TARGET Viewer PROPERTY LINK_DEPENDS ${SHADER_REFERENCES})

    # set_property(TARGET Viewer PROPERTY LINK_DEPENDS 
    #     ${CMAKE_CURRENT_SOURCE_DIR}/shaders/frag.glsl
    #     ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex.glsl
    #)
    target_compile_definitions(Viewer PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

endif()

target_include_directories(Viewer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ext)
