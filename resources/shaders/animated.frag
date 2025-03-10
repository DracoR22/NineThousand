#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture0;

uniform float gamma;

void main() {    
    vec3 color = texture(texture0, TexCoords).rgb;

    // Apply Gamma Correction
    color = pow(color, vec3(1.0 / gamma));

    FragColor = vec4(color, 1.0);
}