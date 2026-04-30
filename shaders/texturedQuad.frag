#version 330 core
layout (location = 0) out vec4 fragColor;

in vec4 v_color;
in vec2 v_texCoords;

uniform bool useTexture;
uniform sampler2D spriteTexture;
uniform vec4 color;
uniform vec2 uvMin;
uniform vec2 uvMax;

void main() {
    vec4 baseColor = v_color * color;

    if (useTexture) {
        vec2 uv = mix(uvMin, uvMax, v_texCoords);
        baseColor *= texture(spriteTexture, uv);
    }
    
    fragColor = baseColor * v_color * color;
}
