#version 450

layout(push_constant) uniform PushObject
{
    mat4 model;
		vec3 color;
} push;

layout(binding = 0) uniform UniformBufferObject{
	mat4 model;
	mat4 view;
	mat4 proj;
}MvpUbo;

layout(binding = 1) uniform UnMyColor{
	float alpha;
	vec3 color;
}ColorUbo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;	
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
		fragColor = push.color;
		fragTexCoord = inTexCoord;
    gl_Position = MvpUbo.proj * MvpUbo.view * push.model * vec4(inPosition, 1.0);
}