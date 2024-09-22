#version 300 es

precision mediump float;

in vec3 color;
in vec3 Normal;

out vec4 FragColor;

void main()
{
    FragColor = vec4(color, 1.0);
} 