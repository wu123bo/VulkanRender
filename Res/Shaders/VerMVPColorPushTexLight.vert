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
} MvpUbo;

layout(binding = 1) uniform UnMyColor{
    float alpha;
    vec3 color;
} ColorUbo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;    
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;    // 新增法线输入

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal; // 输出法线
layout(location = 3) out vec3 fragPos;    // 世界坐标位置

void main() {
    fragColor = push.color;
    fragTexCoord = inTexCoord;

    // 世界坐标
    vec4 worldPos = push.model * vec4(inPosition, 1.0);
    fragPos = worldPos.xyz;

    // 变换法线到世界空间
    fragNormal = normalize(mat3(transpose(inverse(push.model))) * inNormal);

    gl_Position = MvpUbo.proj * MvpUbo.view * worldPos;
}