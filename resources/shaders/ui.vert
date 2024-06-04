#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

// layout(location = 4) in vec3 instancePos;
// layout(location = 5) in vec3 size;
// layout(location = 6) in vec2 textureSize;
// layout(location = 7) in vec2 textureCoords;

layout (location = 0) out vec4 fragColor;

vec2 positions[6] = vec2[](
    // TOP - LEFT
    vec2(position.x, position.z),
    // TOP - RIGHT
    vec2(position.x + uv.x, position.z),
    // BOTTOM - RIGHT
    vec2(position.x + uv.x, position.z + uv.y),
    // BOTTOM - RIGHT
    vec2(position.x + uv.x, position.z + uv.y),
    // BOTTOM - LEFT
    vec2(position.x, position.z + uv.y),
    // TOP - LEFT
    vec2(position.x, position.z)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = vec4(color, 1.0);
}