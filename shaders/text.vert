#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoords;
layout(location = 2) in vec4 a_color;

out vec2 v_texCoords;

uniform mat4 viewProjection;
uniform mat4 model;

void main()
{
    gl_Position = viewProjection * model * vec4(a_position, 1.0);
    v_texCoords = a_texCoords;
    // Flipt coords upside down due to freetype loading format
    v_texCoords = vec2(a_texCoords.x, 1.0 - a_texCoords.y);
}
