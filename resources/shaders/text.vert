#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 4) in vec3 offset;
layout(location = 5) in vec3 size;
layout(location = 6) in vec2 textureCoords;
layout(location = 7) in uint isVisible;

// layout(location = 0) out vec2 fragTexCoord;

// layout(set = 0, binding = 0) uniform TextUbo {
//    vec3 color;
// } ubo;

// vec2 vertices[6] =
//     {
//         // Top Left
//         transform.pos,
//         // Bottom Left
//         vec2(transform.pos + vec2(0.0, transform.size.y)),
//         // Top Right
//         vec2(transform.pos + vec2(transform.size.x, 0.0)),
//         // Top Right
//         vec2(transform.pos + vec2(transform.size.x, 0.0)),
//         // Bottom Left
//         vec2(transform.pos + vec2(0.0, transform.size.y)),
//         // Bottom Right
//         transform.pos + transform.size
//     };


layout (location = 0) out vec2 fragTextCoords;

//vec2 size = vec2(0.1, 0.2);

vec2 positions[6] = vec2[](
    // TOP - LEFT
    vec2(position.x + offset.x, position.z + offset.y),
    // TOP - RIGHT
    vec2(position.x + offset.x + size.x, position.z + offset.y),
    // BOTTOM - RIGHT
    vec2(position.x + offset.x + size.x, position.z + offset.y + size.y),
    // BOTTOM - RIGHT
    vec2(position.x + offset.x + size.x, position.z + offset.y + size.y),
    // BOTTOM - LEFT
    vec2(position.x + offset.x, position.z + offset.y + size.y),
    // TOP - LEFT
    vec2(position.x + offset.x, position.z + offset.y)
);


vec2 texCoords[6] = {
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0),

    vec2(1.0, 1.0),
    vec2(0.0, 1.0),
    vec2(0.0, 0.0),
};

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragTextCoords = texCoords[gl_VertexIndex];
}