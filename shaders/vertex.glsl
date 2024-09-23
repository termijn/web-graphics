#version 300 es

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

out vec3 color;
out vec3 Normal;
out vec3 viewPos;
out vec3 fragPosition;

void main()
{
    viewPos         = vec3(inverse(view)[3]);
    Normal          = mat3(transpose(inverse(view * model))) * normal.xyz;
    color           = vec3(0.0,0.5,0.6);
    gl_Position     = projection * view * model * position;
    fragPosition    = vec3(model * position);
}