#include "renderpass.h"
#include "errors.h"
#include <string>
#include <iostream>
#include <fstream>

RenderPass::RenderPass(VertexBuffer& vertexBuffer_)
    : vertexBuffer(vertexBuffer_)
{    
    std::string vertexSource    = readFile("/shaders/vertex.glsl");
    std::string fragmentSource  = readFile("/shaders/frag.glsl");

    fragmentShader  = compileShader(GL_VERTEX_SHADER,   vertexSource.c_str());
    vertexShader    = compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());

    program = glCreateProgram();
    glAttachShader  (program, vertexShader);
    glAttachShader  (program, fragmentShader);
    glLinkProgram   (program);
    glUseProgram    (program);
}

RenderPass::~RenderPass()
{
    
}

void RenderPass::render()
{
    glUseProgram(program);
    vertexBuffer.bind(program);

    glDrawElements(GL_TRIANGLES, vertexBuffer.getMesh().indices.size() * 3, GL_UNSIGNED_SHORT, 0);
    glCheckError(); 
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

std::string RenderPass::readFile(const std::string &name) const
{
    std::string result;
    std::ifstream stream(name);
    if (!stream.is_open()) 
    {
        std::cerr << "Error: Could not open " << name  << std::endl;
        return result;
    }
    std::string line;

    while (std::getline(stream, line)) 
    {
        result += line + "\n";
    }
    stream.close();
    return result;
}
