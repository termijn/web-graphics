#version 300 es

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

layout(location = 0) in vec4 position;

void main()
{
    gl_Position = projection * view * model * position;
}