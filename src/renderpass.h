#include <SDL_opengles2.h>
#include <string>
#include <glm/glm.hpp>

#include "vertexbuffer.h"
#include "renderable.h"
#include "vertexbufferpool.h"

class RenderPass
{
public:
    RenderPass();
    ~RenderPass();

    void init();

    void render(float aspectRatio, const glm::mat4& view, const std::vector<const Renderable*>& renderables);

    GLuint getProgram();
    
private:
    GLuint      compileShader (GLenum type, const GLchar* source);
    std::string readFile(const std::string& name) const;
    void        setUniforms(float aspectRatio, const Renderable& renderable);

    VertexBufferPool vertexBufferPool;

    GLuint vertexShader     = 0;
    GLuint fragmentShader   = 0;

    GLint       locationViewUniform;
    GLint       locationLightDirection;
    GLint       locationLightColor;
    GLint       locationRoughness;
    GLint       locationMetallic;
    GLint       locationProjection;
    GLint       locationModel;

    glm::mat4   view;
    glm::mat4   projection;
    glm::mat4   model;

    GLuint program;
};