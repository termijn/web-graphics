#include <SDL_opengles2.h>
#include <string>
#include "vertexbuffer.h"
#include <glm/glm.hpp>

class RenderPass
{
public:
    RenderPass(VertexBuffer& vertexBuffer);
    ~RenderPass();

    void render(double elapsed);

    GLuint getProgram();
    
private:
    GLuint      compileShader (GLenum type, const GLchar* source);
    std::string readFile(const std::string& name) const;
    void        setUniforms(double elapsed);

    VertexBuffer& vertexBuffer;
    GLuint vertexShader     = 0;
    GLuint fragmentShader   = 0;

    GLint       locationViewUniform;
    glm::mat4   view;

    GLuint program;
};