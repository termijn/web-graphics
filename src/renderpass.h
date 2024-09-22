#include <SDL_opengles2.h>
#include <string>
#include "vertexbuffer.h"
#include <glm/glm.hpp>

class RenderPass
{
public:
    RenderPass(VertexBuffer& vertexBuffer);
    ~RenderPass();

    void init();

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
    GLint       locationLightDirection;
    GLint       locationLightColor;
    GLint       locationRoughness;
    GLint       locationMetallic;

    glm::mat4   view;

    GLuint program;
};