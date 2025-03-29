#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture0;

void main() {    
    vec3 color = texture(texture0, TexCoords).rgb;

    FragColor = vec4(color, 1.0);
}