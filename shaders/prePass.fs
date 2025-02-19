#version 330 core
in vec3 FragNormal;
in vec2 TexCoord;
layout (location = 1) out vec4 NormalColor;
layout (location = 2) out vec4 albedo;

uniform sampler2D currentTexture;

void main() {
    NormalColor = vec4(FragNormal, 1.0);
    albedo = texture(currentTexture, TexCoord);
}