#include "renderer.h"
#include "errors.h"

#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 

Renderer::Renderer(const std::string &vertexShaderFileName_, const std::string &fragmentShaderFileName_, VertexBufferPool&  vertexBufferPool_)
    : vertexBufferPool      (vertexBufferPool_)
    , vertexShaderFileName  (vertexShaderFileName_)
    , fragmentShaderFileName(fragmentShaderFileName_)
{
}

Renderer::~Renderer() = default;

void Renderer::init()
{
    std::string vertexSource    = readFile(vertexShaderFileName);
    std::string fragmentSource  = readFile(fragmentShaderFileName);

    fragmentShader  = compileShader(GL_VERTEX_SHADER,   vertexSource.c_str());
    vertexShader    = compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());

    program = glCreateProgram();
    glAttachShader  (program, vertexShader);
    glAttachShader  (program, fragmentShader);
    glLinkProgram   (program);
}

void Renderer::render(float aspectRatio, const glm::mat4 &view_, const std::vector<const Renderable *> &renderables) const
{
    view = view_;

    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    for (const Renderable* renderable : renderables)
    {
        VertexBuffer& vertexBuffer = vertexBufferPool.get(renderable);
        vertexBuffer.setMesh(&renderable->mesh);
        vertexBuffer.bind(program);

        setUniforms(aspectRatio, *renderable);
    
        glDrawElements(GL_TRIANGLES, vertexBuffer.getMesh().indices.size() * 3, GL_UNSIGNED_SHORT, 0);
        glCheckError();
    }
}

GLuint Renderer::compileShader(GLenum type, const GLchar *source)
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

std::string Renderer::readFile(const std::string &name) const
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
