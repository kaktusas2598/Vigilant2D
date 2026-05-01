#version 330 core
layout (location = 0) out vec4 fragColor;

in vec4 v_color;
in vec2 v_texCoords;

uniform bool useTexture;
uniform sampler2D spriteTexture;
uniform vec4 color;
uniform vec2 uvMin;
uniform vec2 uvMax;
// TODO: dont like this
uniform bool remapUVs;

void main() {
    vec2 uv = v_texCoords;
    if (remapUVs) {
        uv = mix(uvMin, uvMax, v_texCoords);
    }
    
    vec4 baseColor = v_color * color;
    if (useTexture) {
        baseColor *= texture(spriteTexture, uv);
    }
    
    fragColor = baseColor;
}
