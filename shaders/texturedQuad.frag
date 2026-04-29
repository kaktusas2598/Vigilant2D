#version 330 core
layout (location = 0) out vec4 fragColor;

in vec4 v_color;
in vec2 v_texCoords;

uniform sampler2D spriteTexture;
uniform vec4 color;
uniform vec2 uvMin;
uniform vec2 uvMax;

void main() {
    vec2 uv = mix(uvMin, uvMax, v_texCoords);
    vec4 texColor = texture(spriteTexture, uv);
    fragColor = texColor * v_color * color;
}
