#version 300 es

precision mediump float;

// Define light properties
uniform vec3 lightDirection;
uniform vec3 lightColor;

in vec3 fragPosition; // Fragment position
in vec3 Normal;       // Normal of the fragment
in vec3 viewPos;     // Camera position
in vec3 color;       // Base color of the material

out vec4 FragColor;

// PBR parameters
uniform float metallic; // Metallic factor [0, 1]
uniform float roughness; // Roughness factor [0, 1]

void main()
{
    // Normalize vectors
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightDirection);
    vec3 viewDir = normalize(viewPos - fragPosition);

    // Diffuse component (Lambertian)
    float diffuse = max(dot(norm, lightDir), 0.0);

    // Specular component (Blinn-Phong)
    vec3 halfDir = normalize(lightDir + viewDir);
    float specular = pow(max(dot(norm, halfDir), 0.0), 1.0 / roughness);

    // Combine results
    vec3 diffuseColor = color * lightColor * diffuse;
    vec3 specularColor = lightColor * specular * metallic; // Metallic affects specular

    // Final color
    vec3 finalColor = diffuseColor + specularColor;
    FragColor = vec4(finalColor, 1.0);
} 