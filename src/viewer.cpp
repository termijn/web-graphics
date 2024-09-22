#include <emscripten.h>
#include <SDL.h>
#include <SDL_opengles2.h>
#include <chrono>

#include "renderpass.h"
#include "vertexbuffer.h"

#include "mesh.h"

#include <iostream>
#include <fstream>

class App
{
public:
    App() { }
    ~App() { }

    VertexBuffer    vertexBuffer;
    RenderPass      renderPass = RenderPass(vertexBuffer);

};

std::unique_ptr<App> app = nullptr;
Mesh mesh;
std::chrono::time_point<std::chrono::steady_clock> startTime;

void mainLoop(void* mainLoopArg) 
{
    SDL_Window* window = (SDL_Window*) mainLoopArg;

    int winWidth = 512, winHeight = 512;
    SDL_GL_GetDrawableSize(window, &winWidth, &winHeight);
    glViewport(0, 0, winWidth, winHeight);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto time = std::chrono::steady_clock::now() - startTime;
    auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();

    app->renderPass.render(milli / 1000.0);

    SDL_GL_SwapWindow(window);
}

int main(int argc, char** argv)
{
    startTime = std::chrono::steady_clock::now();
    int winWidth = 512, winHeight = 512;

    // Create SDL window
    SDL_Window *pWindow = 
        SDL_CreateWindow("Hello Triangle Minimal", 
                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         winWidth, winHeight, 
                         SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    // Create OpenGLES 2 context on SDL window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetSwapInterval(1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GLContext glc = SDL_GL_CreateContext(pWindow);
    printf("INFO: GL version: %s\n", glGetString(GL_VERSION));

    // Set clear color to black
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    // Get actual GL window size in pixels, in case of high dpi scaling
    SDL_GL_GetDrawableSize(pWindow, &winWidth, &winHeight);
    printf("INFO: GL window size = %dx%d\n", winWidth, winHeight);

    glViewport(0, 0, winWidth, winHeight);

    app = std::make_unique<App>();

    //mesh.noisySphere(0.5f, 20, 20, 0.2f);
    mesh.knot(0.3, 100, 100);

    app->vertexBuffer.setMesh(&mesh);

    // Start the main loop
    void* mainLoopArg = pWindow;

    int fps = 0; // Use browser's requestAnimationFrame
    emscripten_set_main_loop_arg(mainLoop, mainLoopArg, fps, true);
 
    return 0;
}