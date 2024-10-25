#version 300 es

struct TextureTransform 
{
    vec2    offset;
    vec2    scale;
    float   rotation;
};

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform TextureTransform textureTransform;

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec4 tangent;
layout(location = 4) in vec4 bitangent;

out vec3 Normal;
out vec3 viewPositionWorld;
out vec3 fragPositionWorld;
out vec2 uvBaseColor;

out vec3 fragTangent;
out vec3 fragBitangent;

vec2 textureTransforms(vec2 uv)
{
    mat3 translation = mat3(1,0,0, 0,1,0, textureTransform.offset.x, textureTransform.offset.y, 1);
    mat3 rotation = mat3(
         cos(textureTransform.rotation), sin(textureTransform.rotation), 0,
        -sin(textureTransform.rotation), cos(textureTransform.rotation), 0,
         0                             , 0                             , 1
    );
    mat3 scale = mat3(textureTransform.scale.x, 0, 0, 0,textureTransform.scale.y, 0, 0, 0, 1);

    mat3 transform = translation * rotation * scale;
    return (transform * vec3(uv.xy, 1)).xy;
}

void main()
{
    mat3 modelToWorld = mat3(transpose(inverse(model)));

    viewPositionWorld   = vec3(inverse(view)[3]);
    Normal              = modelToWorld * normal.xyz;
    fragPositionWorld   = vec3(model * position);
    gl_Position         = projection * view * model * position;
    uvBaseColor         = textureTransforms(uv);

    fragTangent     = modelToWorld * tangent.xyz;
    fragBitangent   = modelToWorld * bitangent.xyz;
}