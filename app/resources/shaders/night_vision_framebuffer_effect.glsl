//#shader vertex
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}


//#shader fragment
#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

float random(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {

    vec3 col = texture(screenTexture, TexCoords).rgb;

    // 1. Night Vision
    float luminance = dot(col, vec3(0.299, 0.587, 0.114));
    vec3 visionColor = vec3(0.0, luminance * 2.0, 0.0);

    // 2. vignette
    vec2 center = TexCoords - 0.5;
    float dist = length(center);
    float vignette = smoothstep(0.7, 0.2, dist);

    // 3. Noise
    float noise = random(TexCoords) * 0.1;

    FragColor = vec4((visionColor + noise) * vignette, 1.0);
}