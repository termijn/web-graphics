#include <SDL_opengles2.h>

class RenderPass
{
public:
    RenderPass();

    void render();
    
private:
    GLuint compileShader (GLenum type, const GLchar* source);

    GLuint vertexShader     = 0;
    GLuint fragmentShader   = 0;    
    GLuint program          = 0;
};