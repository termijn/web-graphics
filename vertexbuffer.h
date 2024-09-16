#include <SDL_opengles2.h>

class VertexBuffer
{
public:
    VertexBuffer();
    void bind(GLuint program);
    
private:
    GLuint vbo;
};