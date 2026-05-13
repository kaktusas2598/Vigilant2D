#version 330 core

in vec2 v_texCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform float vignetteStrength;
uniform float contrast;
uniform float saturation;
uniform float brightness;
uniform vec3 tint;
uniform vec4 fadeColor;
uniform float fadeAmount;

void main() {
    vec4 color = texture(sceneTexture, v_texCoords);

    // Brightness
    color.rgb += vec3(brightness);
    
    // contrast around 0.5 midpoint
    color.rgb = (color.rgb - 0.5) * contrast + 0.5;

    // Saturation
    float luminance = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    vec3 grayscale = vec3(luminance);
    color.rgb = mix(grayscale, color.rgb, saturation);

    // Tint
    color.rgb *= tint;

    // Vignette
    vec2 p = v_texCoords * 2.0 - 1.0;
    float vignette = 1.0 - dot(p, p) * vignetteStrength;
    vignette = clamp(vignette, 0.0, 1.0);
    color.rgb *= vignette;

    // Fade    
    color.rgb = mix(color.rgb, fadeColor.rgb, clamp(fadeAmount, 0.0, 1.0));

    FragColor = color;
}
