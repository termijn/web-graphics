#version 300 es

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 uv;

out vec3 Normal;
out vec3 viewPositionWorld;
out vec3 fragPositionWorld;
out vec2 uvBaseColor;

void main()
{
    viewPositionWorld   = vec3(inverse(view)[3]);
    Normal              = mat3(transpose(inverse(model))) * normal.xyz;
    fragPositionWorld   = vec3(model * position);
    gl_Position         = projection * view * model * position;
    uvBaseColor         = uv;
}