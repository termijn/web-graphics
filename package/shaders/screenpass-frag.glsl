#version 300 es

precision highp float;

uniform vec3        lightPositionWorld;
uniform vec3        lightColor;
uniform mat4        shadowVP;   // Transforms from world to the light space frustum
uniform bool        shaded;
uniform vec3        baseColor;  // Base color of the material
uniform float       metallic;   // Metallic factor [0, 1]
uniform float       roughness;  // Roughness factor [0, 1]
uniform int         nrPoissonSamples;

uniform sampler2D   depthTexture;
uniform sampler2D   baseColorTexture;
uniform sampler2D   metallicRoughnessTexture;
uniform sampler2D   poissonTexture; // A texture of size [nrPoissonSamples, 1] with 2d points distributed according to a poisson pattern

uniform bool hasBaseColorTexture;
uniform bool hasMetallicRoughnessTexture;

in vec3 Normal;             // Normal of the fragment
in vec3 fragPositionWorld;  // Fragment position in world space
in vec3 viewPositionWorld;  // Camera position
in vec2 uvBaseColor;

out vec4 FragColor;

// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04f);

const float M_PI = 3.1415926535897932384f;
const float Epsilon = 0.00001f;

const float texelSize = 1.0f / 2048.0f; // Texel size for 2048x2048 shadow map
const float lightSize = 0.1f;

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness) 
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0f) + 1.0f;
    return alphaSq / (M_PI * denom * denom);
}

// Single term for separable Schlick-GGX.
float gaSchlickG1(float cosTheta, float k) 
{
    return cosTheta / (cosTheta * (1.0f - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness) 
{
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
    return F0 + (vec3(1.0f) - F0) * pow(1.0f - cosTheta, 5.0f);
}

// Percentage-Closer Filtering (PCF)
float pcf(vec3 shadowCoord, int dimensions) 
{
    float shadow = 0.0;
    int sampleCount = 1;

    for (int i = 0; i < nrPoissonSamples; i++)
    {
        vec2 offsetInPixels     = texture(poissonTexture, vec2(float(i) / float(nrPoissonSamples), 0)).xy - 0.5;
        offsetInPixels          = offsetInPixels * 32.0;
        
        vec2 offset = offsetInPixels * texelSize;

        float shadowMapDepth = texture(depthTexture, shadowCoord.xy + offset).r;
        
        shadow += shadowCoord.z > shadowMapDepth ? 1.0 : 0.0;
        sampleCount++;
    }
    return shadow / float(sampleCount -1);
}

vec4 toLightSpace(vec4 coord)
{
    vec4 result = shadowVP * coord;
    result /= result.w;
    result = result * 0.5 + 0.5;
    return result;
}

void main() 
{
    vec3 finalColor = baseColor;
    if (hasBaseColorTexture)
        finalColor = baseColor * texture(baseColorTexture, uvBaseColor).rgb;

    vec3 albedo = finalColor;

    if (shaded) // TODO: compile separate shaders for performance.
    {
        vec4 metallicRoughness = vec4(1.0);
        if (hasMetallicRoughnessTexture)
             metallicRoughness = texture(metallicRoughnessTexture, uvBaseColor);

        float finalMetallic  = metallicRoughness.r * metallic;
        float finalRoughness = metallicRoughness.g * roughness;

        vec3 Lo         = normalize(viewPositionWorld - fragPositionWorld);
        vec3 Li         = normalize(lightPositionWorld - fragPositionWorld);
        vec3 N          = normalize(Normal);
        float cosLo     = max(0.0f, dot(N, Lo));

        // Fresnel reflectance at normal incidence (for metals use albedo color).
        vec3 F0 = mix(Fdielectric, albedo, vec3(finalMetallic));

        vec3 directLighting = vec3(0);
        
        // For each light
        {
            vec3 Lradiance = lightColor;

            // Half-vector between Li and Lo.
            vec3 Lh = normalize(Li + Lo);

            float cosLi = max(0.0f, dot(N, Li));
            float cosLh = max(0.0f, dot(N, Lh));

            vec3 F  = fresnelSchlick(F0, max(0.0f, dot(Lh, Li)));
            float D = ndfGGX(cosLh, finalRoughness);
            float G = gaSchlickGGX(cosLi, cosLo, finalRoughness);
            vec3 kd = mix(vec3(1.0f) - F, vec3(0.0f), finalMetallic);
            vec3 diffuseBRDF = kd * albedo;

            vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0f * cosLi * cosLo);

            // Total contribution for this light.
            directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
        }
        
        // Add some ambient light, todo: make uniform
        float ambientStrength = 0.2f;
        vec3 ambient = ambientStrength * albedo * lightColor;

        finalColor =  directLighting + ambient;
    }

    vec3 biasedFragPos = fragPositionWorld + (Normal * 0.02);
    vec4 fragInLightSpace = toLightSpace(vec4(biasedFragPos, 1.0));
    
    float shadow = pcf(fragInLightSpace.xyz, 16) * 0.4;

    finalColor = finalColor * (1.0 - shadow);
    FragColor = vec4(finalColor, 1.0f); 
}