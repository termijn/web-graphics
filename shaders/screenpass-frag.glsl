#version 300 es

precision mediump float;

uniform vec3 lightPositionWorld;
uniform vec3 lightColor;

uniform mat4 shadowVP;

uniform sampler2D depthTexture;

in vec3 Normal;             // Normal of the fragment
in vec3 fragPositionWorld;  // Fragment position in world space
in vec3 viewPositionWorld;  // Camera position

out vec4 FragColor;

uniform vec3    baseColor;  // Base color of the material
uniform float   metallic;   // Metallic factor [0, 1]
uniform float   roughness;  // Roughness factor [0, 1]

// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04f);

const float M_PI = 3.1415926535897932384f;
const float Epsilon = 0.00001f;

const float texelSize = 1.0f / 2048.0f; // Texel size for 2048x2048 shadow map

const vec2 pcfOffsets[4] = vec2[](
    vec2(-texelSize * 0.5,  texelSize * 0.5),   // Top-left
    vec2( texelSize * 0.5,  texelSize * 0.5),    // Top-right
    vec2(-texelSize * 0.5, -texelSize * 0.5),    // Bottom-left
    vec2( texelSize * 0.5, -texelSize * 0.5)     // Bottom-right
);

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness) {
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0f) + 1.0f;
    return alphaSq / (M_PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k) {
    return cosTheta / (cosTheta * (1.0f - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness) {
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta) {
    return F0 + (vec3(1.0f) - F0) * pow(1.0f - cosTheta, 5.0f);
}

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898f, 78.233f))) * 43758.5453123f);
}

float shadow()
{
    vec4 shadowPos = shadowVP * vec4(fragPositionWorld, 1.0f);
    shadowPos /= shadowPos.w;  // Perspective divide

    vec3 shadowCoord = shadowPos.xyz * 0.5f + 0.5f; // Convert to [0, 1]

    float shadowDepth = 0.0f;
    float currentDepth = shadowCoord.z;
    int samples = 0;

    float h = random(shadowCoord.xy);

    for(int i = 0; i < 4; ++i) {
        vec2 offset = pcfOffsets[i] + (h - 0.5f) * texelSize * 2.0f;
        float sampleDepth = texture(depthTexture, shadowCoord.xy + offset).r;
        if(currentDepth > sampleDepth + 0.001f) {
            shadowDepth += 1.0f;
        }
        samples++;
    }
    // Average the shadow values
    float shadow = (samples > 0) ? shadowDepth / float(samples) : 1.0f;

    float shadowFactor = 0.3f;
    return (1.0 - (shadowFactor * shadow));
}

void main() {
    // Normalize vectors
    vec3 norm = normalize(Normal);

    vec3 lightDir = normalize(lightPositionWorld - fragPositionWorld);
    //vec3 lightDir = -normalize(vec3(-1,-1,-1));

       // Outgoing light direction (vector from world-space fragment position to the "eye").
    vec3 Lo = normalize(viewPositionWorld - fragPositionWorld);

    // Get current fragment's normal and transform to world space.
    vec3 N = norm;

    // Angle between surface normal and outgoing light direction.
    float cosLo = max(0.0f, dot(N, Lo));

    // Fresnel reflectance at normal incidence (for metals use albedo color).
    vec3 F0 = mix(Fdielectric, baseColor, vec3(metallic));

    vec3 directLighting = vec3(0);
    // Light
    {
        vec3 Li = lightDir;
        vec3 Lradiance = lightColor;

        // Half-vector between Li and Lo.
        vec3 Lh = normalize(Li + Lo);

        // Calculate angles between surface normal and various light vectors.
        float cosLi = max(0.0f, dot(N, Li));
        float cosLh = max(0.0f, dot(N, Lh));

        // Calculate Fresnel term for direct lighting. 
        vec3 F = fresnelSchlick(F0, max(0.0f, dot(Lh, Li)));
        // Calculate normal distribution for specular BRDF.
        float D = ndfGGX(cosLh, roughness);
        // Calculate geometric attenuation for specular BRDF.
        float G = gaSchlickGGX(cosLi, cosLo, roughness);

        // Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
        // Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
        // To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
        vec3 kd = mix(vec3(1.0f) - F, vec3(0.0f), metallic);

        // Lambert diffuse BRDF.
        // We don't scale by 1/PI for lighting & material units to be more convenient.
        // See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
        vec3 diffuseBRDF = kd * baseColor;

        // Cook-Torrance specular microfacet BRDF.
        vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0f * cosLi * cosLo);

        // Total contribution for this light.
        directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    }

    float ambientStrength = 0.3f;
    // Calculate the ambient light
    vec3 ambient = ambientStrength * baseColor * lightColor;

    vec3 finalColor =  directLighting + ambient;

    FragColor = vec4(finalColor * shadow(), 1.0f);
}