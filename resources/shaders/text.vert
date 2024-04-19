#version 450

// layout(location = 0) in vec3 position;
// layout(location = 1) in vec3 color;
// layout(location = 2) in vec3 normal;
// layout(location = 3) in vec2 uv;

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

vec2 positions[6] = vec2[](
    // TOP - LEFT
    vec2(-0.5, -0.5),
    // TOP - RIGHT
    vec2(0.5, -0.5),
    // BOTTOM - RIGHT
    vec2(0.5, 0.5),
    // BOTTOM - RIGHT
    vec2(0.5, 0.5),
    // BOTTOM - LEFT
    vec2(-0.5, 0.5),
    // TOP - LEFT
    vec2(-0.5, -0.5)
);


vec2 textureCoords[6] = {
    vec2(1.0, 0.0),
    vec2(0.0, 0.0),
    vec2(0.0, 1.0),

    vec2(0.0, 1.0),
    vec2(1.0, 1.0),
    vec2(1.0, 0.0),
};

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragTextCoords = textureCoords[gl_VertexIndex];
}