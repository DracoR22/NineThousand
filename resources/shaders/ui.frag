#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D baseTexture;
uniform vec3 tintColor;

void main() {
    vec4 texColor = texture(baseTexture, TexCoord);
    FragColor = texColor * vec4(tintColor, 1.0);
}