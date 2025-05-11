#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D baseTexture;
uniform float rotation;

void main() {
   // rotate texuture around its center
    vec2 center = vec2(0.5, 0.5);
    vec2 uv = TexCoords - center;

    float cosAngle = cos(rotation);
    float sinAngle = sin(rotation);

    uv = vec2(
        uv.x * cosAngle - uv.y * sinAngle,
        uv.x * sinAngle + uv.y * cosAngle
    );

    uv += center;

  FragColor = texture(baseTexture, uv);
}