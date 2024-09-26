#version 300 es

precision mediump float;

uniform vec3 lightDirection;
uniform vec3 lightColor;

uniform mat4 shadowVP;

uniform sampler2D depthTexture;

in vec3 Normal;             // Normal of the fragment
in vec3 fragPositionWorld;  // Fragment position in world space
in vec3 viewPositionWorld;  // Camera position
in vec3 color;              // Base color of the material

out vec4 FragColor;

uniform float metallic;   // Metallic factor [0, 1]
uniform float roughness;  // Roughness factor [0, 1]

// Constant normal incidence Fresnel factor for all dielectrics.
const vec3 Fdielectric = vec3(0.04);

const float M_PI = 3.1415926535897932384;
const float Epsilon = 0.00001;

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (M_PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    // Normalize vectors
    vec3 norm = normalize(Normal);

    // Negate lightdir for lighting calculations.
    // So the renderer indicates the direction of the light rays
    // Here we use the direction of the light from the surface to the light source.
    vec3 lightDir = -lightDirection;
    //vec3 viewDir = normalize(viewPositionWorld - fragPositionWorld);

   	// Outgoing light direction (vector from world-space fragment position to the "eye").
	vec3 Lo = normalize(viewPositionWorld - fragPositionWorld);

	// Get current fragment's normal and transform to world space.
	vec3 N = norm;
	
	// Angle between surface normal and outgoing light direction.
	float cosLo = max(0.0, dot(N, Lo));
		
	// Specular reflection vector.
	vec3 Lr = 2.0 * cosLo * N - Lo;

	// Fresnel reflectance at normal incidence (for metals use albedo color).
	vec3 F0 = mix(Fdielectric, color, vec3(metallic));

    vec3 directLighting = vec3(0);
    // Light
    {
        vec3 Li = -lightDirection;
		vec3 Lradiance = lightColor;

		// Half-vector between Li and Lo.
		vec3 Lh = normalize(Li + Lo);

		// Calculate angles between surface normal and various light vectors.
		float cosLi = max(0.0, dot(N, Li));
		float cosLh = max(0.0, dot(N, Lh));

		// Calculate Fresnel term for direct lighting. 
		vec3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
		// Calculate normal distribution for specular BRDF.
		float D = ndfGGX(cosLh, roughness);
		// Calculate geometric attenuation for specular BRDF.
		float G = gaSchlickGGX(cosLi, cosLo, roughness);

		// Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
		// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
		// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
		vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic);

		// Lambert diffuse BRDF.
		// We don't scale by 1/PI for lighting & material units to be more convenient.
		// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
		vec3 diffuseBRDF = kd * color;

		// Cook-Torrance specular microfacet BRDF.
		vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

		// Total contribution for this light.
		directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    }

    // Diffuse component (Lambertian)
    float diffuse = max(dot(norm, lightDir), 0.0);

    vec3 diffuseColor = color * lightColor * diffuse;

    // Final color
    vec3 finalColor = diffuseColor + directLighting;

	vec4 shadowPos = shadowVP * vec4(fragPositionWorld, 1.0);

	// Perform the perspective divide to get NDC and convert to texture coordinates
	shadowPos /= shadowPos.w;  // Perspective divide (from homogeneous coordinates)

	vec3 shadowCoord = shadowPos.xyz * 0.5 + 0.5;

	float shadowDepth = texture(depthTexture, shadowCoord.xy).r;

	float currentDepth = shadowCoord.z;

	float shadow  = 1.0;
	if (currentDepth > shadowDepth + 0.001) {
		shadow = 0.5;
	}

    FragColor = vec4(finalColor * shadow, 1.0);
}