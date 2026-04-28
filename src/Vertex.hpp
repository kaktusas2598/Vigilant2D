#pragma once

#include "glm/glm.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f}; // White by default to avoid tint with UVs
};
