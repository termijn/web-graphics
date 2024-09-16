#include "renderpass.h"
#include <string>
#include <iostream>

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
    std::cout << "Create render pass \n";
    fragmentShader  = compileShader(GL_VERTEX_SHADER,   vertexShaderSource);
    vertexShader    = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);   
    glUseProgram(program);
}

RenderPass::~RenderPass()
{
    
}

void RenderPass::render()
{
    glUseProgram(program);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

GLuint RenderPass::getProgram()
{
    return program;
}

GLuint RenderPass::compileShader(GLenum type, const GLchar* source)
{   
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

    if (infoLen > 1) 
    {
        std::string message;
        message.resize(infoLen);

        glGetShaderInfoLog(shader, infoLen, NULL, message.data());
        std::cout << message << "\n";
    } 
    else 
    {
        std::cout << "shader compiled\n";
    }

    return shader;
}
