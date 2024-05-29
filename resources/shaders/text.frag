#version 450
layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragTextColor;
layout (location = 2) in float fragOutline;
layout (location = 3) in vec3 fragOutlineColor;
layout (location = 4) in float fragIsVisible;

layout(set = 0, binding = 0) uniform TextUbo {
   vec3 color;
} ubo;

layout(location = 0) out vec4 outColor;

void main() {
    // vec4 fragmentColor = texture(texSampler, fragTexCoord);
    // if (fragmentColor.r < 0.07) {
    //     discard;
    // }

    // if (fragmentColor.r < 0.2) {
    //     if (fragOutline == 0.0) {
    //         discard;
    //     }
    //     outColor = vec4(fragOutlineColor * vec3(1.0), 1.0);
    // } else {
    //     outColor = vec4(fragTextColor * vec3(1.0), 1.0);
    // }

    float distance = texture(texSampler, fragTexCoord).r;
    if (distance < 0.15 || fragIsVisible != 1) {
        discard;
    }
    float smoothWidth = fwidth(distance);	
    float alpha = smoothstep(0.2 - smoothWidth, 0.2 + smoothWidth, distance);
	vec3 rgb = vec3(alpha);
									 
	if (fragOutline > 0.0) 
	{
		float w = 0.1;
		alpha = smoothstep(w - smoothWidth, w + smoothWidth, distance);
        rgb += mix(vec3(alpha), fragOutlineColor, alpha);
    }									 
									 
    outColor = vec4(rgb, alpha);	
}