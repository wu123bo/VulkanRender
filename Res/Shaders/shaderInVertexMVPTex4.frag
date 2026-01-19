#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform UnMyColor{
	float alpha;
	vec3 color;
}ColorUbo;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(ColorUbo.color * texture(texSampler,fragTexCoord).rgb, ColorUbo.alpha);
}