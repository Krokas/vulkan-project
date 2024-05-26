#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 4) in vec3 instancePos;
layout(location = 5) in vec3 size;
layout(location = 6) in vec2 textureSize;
layout(location = 7) in vec2 textureCoords;
layout(location = 8) in uint isVisible;


layout (location = 0) out vec2 fragTextCoords;

//vec2 size = vec2(0.1, 0.2);

vec2 positions[6] = vec2[](
    // TOP - LEFT
    vec2(position.x + instancePos.x, position.z + instancePos.y),
    // TOP - RIGHT
    vec2(position.x + instancePos.x + size.x, position.z + instancePos.y),
    // BOTTOM - RIGHT
    vec2(position.x + instancePos.x + size.x, position.z + instancePos.y + size.y),
    // BOTTOM - RIGHT
    vec2(position.x + instancePos.x + size.x, position.z + instancePos.y + size.y),
    // BOTTOM - LEFT
    vec2(position.x + instancePos.x, position.z + instancePos.y + size.y),
    // TOP - LEFT
    vec2(position.x + instancePos.x, position.z + instancePos.y)
);


vec2 texCoords[6] = {
    vec2(textureCoords.x, textureCoords.y),
    vec2(textureCoords.x + textureSize.x, textureCoords.y),
    vec2(textureCoords.x + textureSize.x, textureCoords.y + textureSize.y),

    vec2(textureCoords.x + textureSize.x, textureCoords.y + textureSize.y),
    vec2(textureCoords.x, textureCoords.y + textureSize.y),
    vec2(textureCoords.x, textureCoords.y),
};

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragTextCoords = texCoords[gl_VertexIndex];
}