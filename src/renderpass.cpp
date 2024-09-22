#include "renderpass.h"
#include "errors.h"
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 

using namespace glm;

RenderPass::RenderPass(VertexBuffer &vertexBuffer_)
    : vertexBuffer(vertexBuffer_)
{
}

RenderPass::~RenderPass()
{
}

void RenderPass::init()
{
    std::string vertexSource    = readFile("/shaders/vertex.glsl");
    std::string fragmentSource  = readFile("/shaders/frag.glsl");

    fragmentShader  = compileShader(GL_VERTEX_SHADER,   vertexSource.c_str());
    vertexShader    = compileShader(GL_FRAGMENT_SHADER, fragmentSource.c_str());

    program = glCreateProgram();
    glAttachShader  (program, vertexShader);
    glAttachShader  (program, fragmentShader);
    glLinkProgram   (program);

    locationViewUniform     = glGetUniformLocation(program, "view");
    locationLightColor      = glGetUniformLocation(program, "lightColor");
    locationLightDirection  = glGetUniformLocation(program, "lightDirection");
    locationMetallic        = glGetUniformLocation(program, "metallic");
    locationRoughness       = glGetUniformLocation(program, "roughness");
}

void RenderPass::render(double elapsed)
{
    glUseProgram(program);
    vertexBuffer.bind(program);
    setUniforms(elapsed);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

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

void RenderPass::setUniforms(double elapsed)
{
    view = glm::rotate(glm::mat4(1.0f), glm::radians((float)elapsed * 90.0f), glm::vec3(1.0f, 1.0f, 0.0f));
    view = glm::rotate(view, glm::radians((float)elapsed * 60.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    glUniformMatrix4fv(locationViewUniform, 1, GL_FALSE, value_ptr(view));

    glm::vec4 lightDir(-0.3,-0.3,-1.0, 0.0);
    lightDir = lightDir * glm::inverse(view);

    glUniform3f(locationLightDirection, lightDir.x, lightDir.y, lightDir.z); // Light coming from above
    glUniform3f(locationLightColor, 1.0f, 1.0f, 1.0f);

    glUniform1f(locationMetallic, 0.3f);
    glUniform1f(locationRoughness, 0.4f);
}
