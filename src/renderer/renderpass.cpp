#include "renderer/renderpass.h"
#include "errors.h"

#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "renderpass.h"

using namespace glm;

RenderPass::RenderPass(const std::string &vertexShaderFileName_, const std::string &fragmentShaderFileName_, GpuPool& gpuPool_)
    : gpuPool               (gpuPool_)
    , vertexShaderFileName  (vertexShaderFileName_)
    , fragmentShaderFileName(fragmentShaderFileName_)
{
}

RenderPass::~RenderPass() = default;

void RenderPass::init()
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

void RenderPass::renderPre(const glm::mat4 &view_, const glm::mat4 &projection_)
{
    view        = view_;
    projection  = projection_;

    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void RenderPass::render(const std::vector<const Renderable *> &renderables) const
{
    for (const Renderable* renderable : renderables)
    {
        VertexBuffer& vertexBuffer = gpuPool.get(renderable);
        vertexBuffer.setMesh(&renderable->mesh);
        vertexBuffer.bind(program);

        setUniforms(*renderable);
    
        glDrawElements(GL_TRIANGLES, vertexBuffer.getMesh().indices().size() * 3, GL_UNSIGNED_INT, 0);
        glCheckError();
    }
}

GLuint RenderPass::compileShader(GLenum type, const GLchar *source)
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
