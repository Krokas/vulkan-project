#version 450
layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) in vec2 fragTexCoord;

layout(set = 0, binding = 0) uniform TextUbo {
   vec3 color;
} ubo;

layout(location = 0) out vec4 outColor;

void main() {
    //outColor = vec4(1.0, 0.0, 0.0, 1.0);
    outColor = texture(texSampler, fragTexCoord);
}