#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragPos;

layout(binding = 2) uniform sampler2D texSampler;

layout(binding = 3) uniform Light {
    vec3 lightPos;    // 光源位置
    vec3 lightColor;  // 光源颜色
    vec3 viewPos;     // 摄像机位置
} light;

layout(location = 0) out vec4 outColor;

void main() {
    // 纹理颜色
    vec3 texColor = texture(texSampler, fragTexCoord).rgb;

    // 环境光
    vec3 ambient = 0.1 * texColor * fragColor;

    // 漫反射
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(light.lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.lightColor * texColor;

    // 简单镜面高光（可选）
    vec3 viewDir = normalize(light.viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * light.lightColor;

    vec3 result = ambient + diffuse + specular;
    outColor = vec4(result, 1.0);
}