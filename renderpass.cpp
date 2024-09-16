#include "renderpass.h"

const GLchar* vertexShaderSource =
    "attribute vec4 position;                      \n"
    "varying vec3 color;                           \n"
    "void main()                                   \n"
    "{                                             \n"
    "    gl_Position = vec4(position.xyz, 1.0);    \n"
    "    color = gl_Position.xyz + vec3(0.5);      \n"
    "}                                             \n";

// Fragment/pixel shader
const GLchar* fragmentShaderSource =
    "precision mediump float;                     \n"
    "varying vec3 color;                          \n"
    "void main()                                  \n"
    "{                                            \n"
    "    gl_FragColor = vec4 ( color, 1.0 );      \n"
    "}                                            \n";

RenderPass::RenderPass()
{    
    fragmentShader  = compileShader(GL_VERTEX_SHADER,   vertexShaderSource);
    vertexShader    = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);    

    GLint posAttrib = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void RenderPass::render()
{
    glUseProgram(program);
}

GLuint RenderPass::compileShader(GLenum type, const GLchar* source)
{   GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}
