#include <SDL_opengles2.h>
#include <string>
#include "vertexbuffer.h"

class RenderPass
{
public:
    RenderPass(VertexBuffer& vertexBuffer);
    ~RenderPass();

    void render();

    GLuint getProgram();
    
private:
    GLuint      compileShader (GLenum type, const GLchar* source);
    std::string readFile(const std::string& name) const;

    VertexBuffer& vertexBuffer;
    GLuint vertexShader     = 0;
    GLuint fragmentShader   = 0;
    GLuint program;
};