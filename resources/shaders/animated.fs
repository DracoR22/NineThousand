#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main() {    
    vec3 color = texture(texture1, TexCoords).rgb;

    // Apply Gamma Correction
    float gamma = 2.2;
    color = pow(color, vec3(1.0 / gamma));

    FragColor = vec4(color, 1.0);
}