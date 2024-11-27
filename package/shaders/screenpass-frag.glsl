#version 300 es

precision highp float;

uniform vec3        lightPositionWorld;
uniform vec3        lightColor;

uniform mat4        shadowVP;   // Transforms from world to the light space frustum
uniform bool        shaded;     // Indicates whether to apply the lighting model
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

struct NormalMap
{
    bool        hasNormalMap;
    sampler2D   texture;
    float       scale;
};

struct Emissive
{
    bool        hasEmissiveTexture;
    sampler2D   emissiveTexture;
};

struct ReflectionMap
{
    bool        hasReflectionMap;
    samplerCube texture;
    int         maxMipLevel;
};

struct Occlusion
{
    bool        hasOcclusionMap;
    sampler2D   occlusionMap;
};

uniform Emissive        emissive;
uniform NormalMap       normalMap;
uniform ReflectionMap   reflectionMap;
uniform Occlusion       occlusion;

in vec3 Normal;             // Normal of the fragment
in vec3 fragPositionWorld;  // Fragment position in world space
in vec3 viewPositionWorld;  // Camera position
in vec2 uvBaseColor;
in vec3 fragTangent;
in vec3 fragBitangent;

out vec4 FragColor;

// Constant normal incidence Fresnel factor for all dielectrics.
// 4% reflectance for dielectrics
const vec3 Fdielectric = vec3(0.4f);

const float M_PI = 3.1415926535897932384f;
const float Epsilon = 0.00001f;

const float texelSize = 1.0f / 2048.0f; // Texel size for 2048x2048 shadow map

const vec3 rimColor = vec3(1.0, 1.0, 1.0); // Color of the rim light
const float rimStrength = 0.5;             // Intensity of the rim effect
const float rimWidth = 8.0;                // Controls how wide the rim effect is

// GGX normal distribution function.
float ndfGGX(float cosLh, float alpha) {
    float alpha2 = alpha * alpha;
    float denom = (cosLh * cosLh * (alpha2 - 1.0) + 1.0);
    return alpha2 / (M_PI * denom * denom);
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

float linearizeDepth(float depth, float near, float far) {
    return (2.0 * near) / (far + near - depth * (far - near));
}

vec4 sampleTextureWithBorder(sampler2D tex, vec2 uv, vec4 borderColor) 
{
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)
        return borderColor;
    return texture(tex, uv);
}

float pcf(vec3 shadowCoord, int dimensions) 
{
    float shadow = 0.0;
    int sampleCount = 1;

    for (int i = 0; i < nrPoissonSamples; i++)
    {
        vec2 offsetInPixels     = texture(poissonTexture, vec2(float(i) / float(nrPoissonSamples), 0)).xy - 0.5;
        offsetInPixels          = offsetInPixels * (float(dimensions) * 0.5) ;

        vec2 offset = offsetInPixels * texelSize;

        vec4 depthValue = sampleTextureWithBorder(depthTexture, shadowCoord.xy + offset, vec4(1.0));

        float shadowMapDepth    = depthValue.r;
        float fragDepth         = shadowCoord.z;

        shadow += fragDepth > shadowMapDepth ? 1.0 : 0.0;
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

vec3 getNormalMap(vec3 surfaceNormal)
{
    vec3 result = surfaceNormal;
    if (normalMap.hasNormalMap)
    {
        vec3 tangentNormal = texture(normalMap.texture, uvBaseColor).xyz * 2.0 - 1.0;
        tangentNormal *= vec3(normalMap.scale, normalMap.scale, 1.0);

        mat3 TBN = mat3(normalize(fragTangent), normalize(fragBitangent), normalize(surfaceNormal));
        result = normalize(TBN * tangentNormal);
    }
    return result;
}

vec3 romBinDaHouseToneMapping(vec3 color, float gamma)
{
	color = exp(-1.0 / (2.72 * color + 0.15));
	color = pow(color, vec3((1.0 / gamma)));
	return color;
}

vec3 uncharted2_tonemap_partial(vec3 x)
{
    float A = 0.15f;
    float B = 0.50f;
    float C = 0.10f;
    float D = 0.20f;
    float E = 0.02f;
    float F = 0.30f;
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

vec3 aces_approx(vec3 v)
{
    v *= 0.6f;
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0f, 1.0f);
}

void main() 
{
    vec3 albedo = baseColor;
    if (hasBaseColorTexture)
        albedo = baseColor * pow(texture(baseColorTexture, uvBaseColor).rgb, vec3(2.2));

    vec3 finalColor = albedo;

    vec3 Lo         = normalize(viewPositionWorld - fragPositionWorld);
    vec3 Li         = normalize(lightPositionWorld - fragPositionWorld);
    vec3 N          = getNormalMap(Normal);

    if (shaded) // TODO: compile separate shaders for performance.
    {
        vec4 metallicRoughness = vec4(1.0);
        if (hasMetallicRoughnessTexture)
             metallicRoughness = texture(metallicRoughnessTexture, uvBaseColor);

        float finalMetallic  = metallicRoughness.b * metallic;
        float finalRoughness = metallicRoughness.g * roughness;

        float cosLo     = max(0.0f, dot(N, Lo));

        vec3 F0 = mix(Fdielectric, albedo, vec3(finalMetallic));

        vec3 directLighting = vec3(0);
        
        // For each light
        {
            vec3 Lh = normalize(Li + Lo);

            float cosLi = max(0.0f, dot(N, Li));
            float cosLh = max(0.0f, dot(N, Lh));

            vec3  F  = fresnelSchlick(F0, max(0.0f, dot(Lh, Li)));
            float D  = ndfGGX(cosLh, finalRoughness);
            float G  = gaSchlickGGX(cosLi, cosLo, finalRoughness);
            vec3  kd = mix(vec3(1.0f) - F, vec3(0.0f), finalMetallic);

            vec3 diffuseBRDF    = kd * albedo;
            vec3 specularBRDF   = F * D * G / (4.0 * cosLi * cosLo + 0.001);
            specularBRDF        = mix(specularBRDF, vec3(0.0), finalRoughness);

            float   fresnel         = 1.0 - max(dot(N, Lo), 0.0);
            vec3    envReflection   = vec3(0.0);
            if (reflectionMap.hasReflectionMap)
            {
                vec3  reflected  = reflect(-Lo, N);
                float  mipLevel  = finalRoughness * float(reflectionMap.maxMipLevel);
                vec3 specColor   = textureLod(reflectionMap.texture, reflected, mipLevel).rgb;
                envReflection    = F * pow(specColor, vec3(2.2));

                directLighting += (mix(diffuseBRDF, envReflection, finalMetallic * fresnel) + specularBRDF) * lightColor * cosLi;
            } 
            else
            {
                directLighting += (diffuseBRDF +  specularBRDF) * lightColor * cosLi;
            }

            
            float rimFactor = pow(fresnel, rimWidth);
            vec3   rimLight = rimColor * rimFactor * rimStrength;

            directLighting += rimLight;
        }

        finalColor =  directLighting;

        if (emissive.hasEmissiveTexture)
            finalColor += pow(texture(emissive.emissiveTexture, uvBaseColor).rgb, vec3(2.2));
    }

    float angleBias         = max(0.001 * (1.0 - dot(N, Li)), 0.005);
    float normalOffset      = angleBias;
    vec3 biasedFragPos      = fragPositionWorld + (normalize(Normal) * normalOffset);
    vec4 fragInLightSpace   = toLightSpace(vec4(biasedFragPos, 1.0));
    
    float shadow = pcf(fragInLightSpace.xyz, 16) * 0.8;

    finalColor  = finalColor * (1.0 - shadow);

    if (occlusion.hasOcclusionMap)
    {
        float occlusionFactor = texture(occlusion.occlusionMap, uvBaseColor).r;
        float strength = 1.0;
        finalColor *= mix(1.0, occlusionFactor, strength);
    }

    const float exposureStops = 2.0;
    finalColor  = finalColor * pow(2.0, exposureStops);
    finalColor  = aces_approx(finalColor); 
    FragColor   = vec4(pow(finalColor, vec3(1.0 / 2.2)), 1.0f); 
}