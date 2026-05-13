#version 330 core

in vec2 v_texCoords;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform float vignetteStrength;
uniform float contrast;
uniform vec3 tint;

void main() {
    vec4 color = texture(sceneTexture, v_texCoords);

    // contrast around 0.5 midpoint
    color.rgb = (color.rgb - 0.5) * contrast + 0.5;

    // add subtle tint
    color.rgb *= tint;

    // Calculate vignette
    vec2 p = v_texCoords * 2.0 - 1.0;
    float vignette = 1.0 - dot(p, p) * vignetteStrength;
    vignette = clamp(vignette, 0.0, 1.0);

    color.rgb *= vignette;

    FragColor = color;
}
