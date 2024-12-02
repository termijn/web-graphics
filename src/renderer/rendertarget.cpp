#include "rendertarget.h"
#include "errors.h"
#include <iostream>

using namespace glm;

void CanvasTarget::beginRender()
{
    vec2 size = getSize();

    glBindFramebuffer   (GL_FRAMEBUFFER, 0);
    glViewport          (0, 0, size.x, size.y);
}

void CanvasTarget::endRender()
{
    SDL_GL_SwapWindow(window);
}

glm::vec2 CanvasTarget::getSize() const
{
    int winWidth = 512, winHeight = 512;
    SDL_GL_GetDrawableSize(window, &winWidth, &winHeight);
    return vec2(winWidth, winHeight);
}

CanvasTarget::CanvasTarget(const std::string &canvasId)
{
    int winWidth = 512, winHeight = 512;
    window = 
        SDL_CreateWindow("Viewport", 
                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         winWidth, winHeight, 
                         SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    SDL_GL_SetAttribute     (SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute     (SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetSwapInterval  (1);
    SDL_GL_SetAttribute     (SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute     (SDL_GL_DEPTH_SIZE, 24);

    SDL_GLContext glc = SDL_GL_CreateContext(window);
    printf("INFO: GL version: %s\n", glGetString(GL_VERSION));

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    SDL_GL_GetDrawableSize(window, &winWidth, &winHeight);
    printf("INFO: GL window size = %dx%d\n", winWidth, winHeight);
}

DepthTarget::DepthTarget()
{
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24_OES, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers     (1, &framebuffer);
    glBindFramebuffer     (GL_FRAMEBUFFER, framebuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D,
                           depthTexture,
                            0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        std::cout << "Invalid framebuffer " << framebufferStatus << std::endl;
    }
}

void DepthTarget::beginRender()
{
    glBindFramebuffer   (GL_FRAMEBUFFER, framebuffer);
    glViewport          (0, 0, width, height);
}

void DepthTarget::endRender()
{
}

glm::vec2 DepthTarget::getSize() const
{
    return glm::vec2(width, height);
}

GLuint DepthTarget::getDepthTexture() const
{
    return depthTexture;
}

TextureTarget::TextureTarget()
{
    
}

void TextureTarget::setSize(glm::vec2 size_)
{
    if (framebuffer == 0)
    {
        glGenFramebuffers     (1, &framebuffer);
        glBindFramebuffer     (GL_FRAMEBUFFER, framebuffer);

        glGenTextures(1, &colorTexture);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_EXT, size_.x, size_.y, 0, GL_RGBA, GL_FLOAT, 0);

        glCheckError();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, colorTexture, 0);
        glCheckError();

        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24_OES, size_.x, size_.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
        glCheckError();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER,
                            GL_DEPTH_ATTACHMENT,
                            GL_TEXTURE_2D,
                            depthTexture,
                            0);
        glCheckError();
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            std::cout << "Invalid framebuffer " << framebufferStatus << std::endl;
        }
        size = size_;
    }

    if (size.x == size_.x && size.y == size_.y) return;

    size = size_;

    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_EXT, size_.x, size_.y, 0, GL_RGBA, GL_FLOAT, 0);

    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, size_.x, size_.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 0);
}

void TextureTarget::beginRender()
{
    glBindFramebuffer   (GL_FRAMEBUFFER, framebuffer);
    glViewport          (0, 0, size.x, size.y);
}

void TextureTarget::endRender()
{
}

glm::vec2 TextureTarget::getSize() const
{
    return size;
}

GLuint TextureTarget::getColorTexture() const
{
    return colorTexture;
}
