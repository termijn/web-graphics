#version 300 es

precision mediump float;

// Define light properties in world space
uniform vec3 lightDirection;
uniform vec3 lightColor;

in vec3 fragPosition; // Fragment position in world space
in vec3 Normal;       // Normal of the fragment
in vec3 viewPos;      // Camera position
in vec3 color;        // Base color of the material

out vec4 FragColor;

// PBR parameters
uniform float metallic;   // Metallic factor [0, 1]
uniform float roughness;  // Roughness factor [0, 1]

#define M_PI 3.1415926535897932384626433832795

void main()
{
    // Normalize vectors
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightDirection);
    vec3 viewDir = normalize(viewPos - fragPosition);

    // Diffuse component (Lambertian)
    float diffuse = max(dot(norm, lightDir), 0.0);

    // GGX Specular component
    vec3 halfDir = normalize(lightDir + viewDir);

    // GGX microfacet distribution function
    float NdotH = max(dot(norm, halfDir), 0.0);
    float VdotH = max(dot(viewDir, halfDir), 0.0);
    float alpha = roughness * roughness;

    // GGX D function
    float D = (alpha * alpha) / (M_PI * pow(NdotH * NdotH * (alpha * alpha - 1.0) + 1.0, 2.0));

    // Fresnel-Schlick approximation
    float F0 = mix(0.04, 1.0, metallic); // Base reflectance
    float F = F0 + (1.0 - F0) * pow(1.0 - VdotH, 5.0);

    // Geometry function (Smith's method)
    float k = alpha / 2.0;
    float G = (NdotH * (1.0 - k) + k) / (NdotH * (1.0 - k) + k);
    G *= (VdotH * (1.0 - k) + k) / (VdotH * (1.0 - k) + k);

    // Combine results
    vec3 diffuseColor = color * lightColor * diffuse;

    vec3 specularColor = lightColor * (D * F * G) / (4.0 * max(dot(norm, lightDir), 0.000) * max(dot(norm, viewDir), 0.000));
    specularColor = smoothstep(specularColor, vec3(0.0), vec3(1.0));

    // Final color
    vec3 finalColor = diffuseColor + specularColor * (1.0 - metallic); // Adjust based on metallic
    FragColor = vec4(finalColor, 1.0);
} 