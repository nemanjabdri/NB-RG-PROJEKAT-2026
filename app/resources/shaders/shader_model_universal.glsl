//#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal; //osigurava da normale ostanu normalne nakon skaliranja
    TexCoords = vec2(aTexCoords.x, 1.0 - aTexCoords.y);
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

//#shader fragment
#version 330 core

out vec4 FragColor;

struct PointLight {
    vec3 position;

    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float linear, quadratic;
    vec3 ambient, diffuse, specular;
};

#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 5


in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform samplerCube depthMap;
uniform vec3 viewPos;
uniform float far_plane;
uniform bool fogEnabled;
uniform vec3 fogColor;
uniform float fogStart;
uniform float fogEnd;
//uniform bool shadows;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

float ShadowCalculation(vec3 fragPos) {

    vec3 fragToLight = fragPos - pointLights[2].position;

    float closestDepth = texture(depthMap, fragToLight).r;
    closestDepth *= far_plane;

    float currentDepth = length(fragToLight);

    float bias = 0.05;

    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor, int i) {
    vec3 lightDir = normalize(light.position - fragPos);


    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);


    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance +
    light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * texColor;
    vec3 diffuse = light.diffuse * diff * texColor;
    vec3 specular = light.specular * spec * texColor;

    float shadow = (i == 2) ? ShadowCalculation(FragPos) : 0.0;

    return (ambient + (1 - shadow) * (diffuse + specular)) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor) {
    vec3 lightDir = normalize(light.position - fragPos);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * texColor;
    vec3 diffuse = light.diffuse * diff * texColor;
    vec3 specular = light.specular * spec * texColor;

    return (ambient + (diffuse + specular) * intensity) * attenuation;
}

void main() {
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i], normal, FragPos, viewDir, color, i);
    }

    for (int i = 0; i < NR_SPOT_LIGHTS; i++) {
        result += CalcSpotLight(spotLights[i], normal, FragPos, viewDir, color);
    }

    if (fogEnabled) {
        float distance = length(viewPos - FragPos);

        float fogFactor = (fogEnd - distance) / (fogEnd - fogStart);
        fogFactor = clamp(fogFactor, 0.0, 1.0);

        vec3 finalColor = mix(fogColor, result, fogFactor);

        FragColor = vec4(finalColor, 1.0);
    } else {
        FragColor = vec4(result, 1.0);
    }
}
