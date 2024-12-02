#version 300 es

precision highp float;

out vec4 FragColor;

uniform sampler2D   hdrTexture;
uniform vec2        viewportSize;
uniform float       exposure;

float fxaaQualitySubpix = 0.75; // Sub-pixel quality, usually around 0.75
float fxaaQualityEdgeThreshold = 0.2; // Threshold for edge detection, usually around 0.2
float fxaaQualityEdgeThresholdMin = 0.05; // Minimum edge threshold, usually around 0.05

vec3 aces_approx(vec3 v)
{
    //v *= 0.6f;
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0f, 1.0f);
}

vec3 reinhard(vec3 v)
{
    return v / (1.0f + v);
}

vec3 reinhard_extended(vec3 v, float max_white)
{
    vec3 numerator = v * (1.0f + (v / vec3(max_white * max_white)));
    return numerator / (1.0f + v);
}

float luminance(vec3 v)
{
    return dot(v, vec3(0.2126f, 0.7152f, 0.0722f));
}

vec3 change_luminance(vec3 c_in, float l_out)
{
    float l_in = luminance(c_in);
    return c_in * (l_out / l_in);
}

vec3 reinhard_extended_luminance(vec3 v, float max_white_l)
{
    float l_old = luminance(v);
    float numerator = l_old * (1.0f + (l_old / (max_white_l * max_white_l)));
    float l_new = numerator / (1.0f + l_old);
    return change_luminance(v, l_new);
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

vec3 uncharted2_filmic(vec3 v)
{
    float exposure_bias = 2.0f;
    vec3 curr = uncharted2_tonemap_partial(v * exposure_bias);

    vec3 W = vec3(11.2f);
    vec3 white_scale = vec3(1.0f) / uncharted2_tonemap_partial(W);
    return curr * white_scale;
}

vec3 FXAA(sampler2D tex, vec2 fragCoord, vec2 resolution) {
    vec2 texelSize = 1.0 / resolution;
    
    float offset = 1.0;

    // Calculate the coordinates of the neighboring pixels
    vec3 color = texture(tex, fragCoord / resolution).rgb;
    vec3 colorN = texture(tex, (fragCoord + vec2(offset, 0.0)) / resolution).rgb;
    vec3 colorS = texture(tex, (fragCoord + vec2(-offset, 0.0)) / resolution).rgb;
    vec3 colorE = texture(tex, (fragCoord + vec2(0.0, offset)) / resolution).rgb;
    vec3 colorW = texture(tex, (fragCoord + vec2(0.0, -offset)) / resolution).rgb;
    
    // Calculate luminance (perceived brightness) for edge detection
    float luma = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
    float lumaN = 0.299 * colorN.r + 0.587 * colorN.g + 0.114 * colorN.b;
    float lumaS = 0.299 * colorS.r + 0.587 * colorS.g + 0.114 * colorS.b;
    float lumaE = 0.299 * colorE.r + 0.587 * colorE.g + 0.114 * colorE.b;
    float lumaW = 0.299 * colorW.r + 0.587 * colorW.g + 0.114 * colorW.b;
    
    // Calculate the difference (contrast) between neighboring pixels
    float h = max(abs(luma - lumaN), abs(luma - lumaS));
    float v = max(abs(luma - lumaE), abs(luma - lumaW));
    
    // Combine horizontal and vertical contrast
    float contrast = max(h, v);
    
    // Edge detection based on contrast
    if (contrast < fxaaQualityEdgeThreshold) {
       return color; // No need to apply FXAA
    }

    // Calculate the blend amount based on the contrast
    float blendFactor = fxaaQualitySubpix / max(contrast, fxaaQualityEdgeThresholdMin);
    
    // Smooth the edges by blending with the neighboring pixels
    vec3 blendedColor = (colorN + colorS + colorE + colorW) * 0.25;
    return mix(color, blendedColor, blendFactor); // 1.0 should be blendfactor
}

void main()
{
    vec2 uv = (gl_FragCoord.xy / viewportSize);

    //vec3 hdrInput = FXAA(hdrTexture, gl_FragCoord.xy, viewportSize);
    vec3 hdrInput = texture(hdrTexture, uv).rgb;

    hdrInput  = hdrInput * pow(2.0, exposure);

    vec3 finalColor = aces_approx(hdrInput);

    FragColor   = vec4(pow(finalColor, vec3(1.0 / 2.2)), 1.0f); 
}