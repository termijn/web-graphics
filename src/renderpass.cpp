#include "renderpass.h"
#include "errors.h"
#include <string>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> 

using namespace glm;

RenderPass::RenderPass()
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
    locationProjection      = glGetUniformLocation(program, "projection");
    locationModel           = glGetUniformLocation(program, "model");
}

void RenderPass::render(float aspectRatio, const glm::mat4& view_, const std::vector<const Renderable*>& renderables)
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

void RenderPass::setUniforms(float aspectRatio, const Renderable& renderable)
{
    glUniformMatrix4fv(locationViewUniform, 1, GL_FALSE, value_ptr(view));

    glm::vec4 lightDir(-0.3, -0.3, -1.0, 0.0);
    lightDir = lightDir;

    glUniform3f(locationLightDirection, lightDir.x, lightDir.y, lightDir.z);
    glUniform3f(locationLightColor, 1.0f, 1.0f, 1.0f);

    glUniform1f(locationMetallic, renderable.material.metallic);
    glUniform1f(locationRoughness, renderable.material.roughness);

    projection = glm::perspective(glm::radians(35.0f), aspectRatio, 0.1f, 100.0f);
    glUniformMatrix4fv(locationProjection, 1, GL_FALSE, value_ptr(projection));

    model = renderable.object.getSpace().toRoot;
    glUniformMatrix4fv(locationModel, 1, GL_FALSE, value_ptr(model));

}
