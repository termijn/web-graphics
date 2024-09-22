#version 300 es

uniform mat4 view;

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

out vec3 color;
out vec3 Normal;
out vec3 viewPos;
out vec3 fragPosition;

void main()
{
    viewPos = (vec4(0.0,0.0,0.0,1.0) * view).xyz;
    Normal = mat3(transpose(inverse(mat4(1.0)))) * normal.xyz;  // Normal transformation
    color = position.xyz + vec3(0.5);
    gl_Position = position * view;
    fragPosition = vec3(/*model * */ position);
}