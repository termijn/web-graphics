#pragma once

#include <SDL.h>
#include <SDL_opengles2.h>

#include <glm/glm.hpp>

#include <string>

class RenderTarget
{
public:
    virtual void   beginRender() = 0;
    virtual void   endRender()   = 0;

    virtual glm::vec2   getSize() const = 0;

};

class CanvasTarget: public RenderTarget
{
public:
    CanvasTarget(const std::string& canvasId);

    void        beginRender()   override;
    void        endRender()     override;
    glm::vec2   getSize() const override;

private:
    SDL_Window* window  = nullptr;

};

class TextureTarget: public RenderTarget
{
public:
    TextureTarget();

    void setSize(glm::vec2 size);

    void        beginRender()   override;
    void        endRender()     override;
    glm::vec2   getSize() const override;

    GLuint      getColorTexture() const;

private:
    glm::vec2 size = glm::vec2(10, 10);

    GLuint  colorTexture = 0;
    GLuint  depthTexture = 0;
    GLuint  framebuffer  = 0;

};

class DepthTarget: public RenderTarget
{
public:
    DepthTarget();

    void        beginRender()   override;
    void        endRender()     override;
    glm::vec2   getSize() const override;

    GLuint      getDepthTexture() const;
private:
    const int width  = 2048;
    const int height = 2048;

    GLuint depthTexture;
    GLuint framebuffer;

};
