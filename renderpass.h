#include <SDL_opengles2.h>
#include <string>

class RenderPass
{
public:
    RenderPass();
    ~RenderPass();

    void render();

    GLuint getProgram();
    
private:
    GLuint      compileShader (GLenum type, const GLchar* source);
    std::string readFile(const std::string& name) const;

    GLuint vertexShader     = 0;
    GLuint fragmentShader   = 0;    
    GLuint program;
};