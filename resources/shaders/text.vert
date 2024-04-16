#version 450

// layout(location = 0) in vec3 position;
// layout(location = 1) in vec3 color;
// layout(location = 2) in vec3 normal;
// layout(location = 3) in vec2 uv;

// layout(location = 0) out vec2 fragTexCoord;

// layout(set = 0, binding = 0) uniform TextUbo {
//    vec3 color;
// } ubo;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}