#version 450
layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) in vec2 fragTexCoord;

layout(set = 0, binding = 0) uniform TextUbo {
   vec3 color;
} ubo;

layout(location = 0) out vec4 outColor;

void main() {

    float distance = texture(texSampler, fragTexCoord).r;
    float smoothWidth = fwidth(distance);	
    float alpha = smoothstep(0.5 - smoothWidth, 0.5 + smoothWidth, distance);
	vec3 rgb = vec3(alpha);
									 
	// if (ubo.outline > 0.0) 
	// {
	// 	float w = 1.0 - ubo.outlineWidth;
	// 	alpha = smoothstep(w - smoothWidth, w + smoothWidth, distance);
    //     rgb += mix(vec3(alpha), ubo.outlineColor.rgb, alpha);
    // }									 
									 
    outColor = vec4(rgb, alpha);	
}