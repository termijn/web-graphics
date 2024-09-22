#version 300 es

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

out vec3 color;
out vec3 Normal;

void main()
{
    Normal = mat3(transpose(inverse(mat4(1.0)))) * normal.xyz;  // Normal transformation    
    color = position.xyz + vec3(0.5);
    gl_Position = position;
}