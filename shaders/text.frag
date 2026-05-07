#version 330 core
layout(location = 0) out vec4 fragColor;

in vec2 v_texCoords;

uniform sampler2D glyphTexture;
uniform vec4 textColor;

void main() {
    float alpha = texture(glyphTexture, v_texCoords).r;
    fragColor = vec4(textColor.rgb, textColor.a * alpha);
}
