#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

uniform float sampleDistance;

void main()
{
    vec2 tex_offset = 1.0 / textureSize(image, 0);
    vec3 result = texture(image, TexCoords).rgb * weight[0];

    result += texture(image, TexCoords + vec2(tex_offset.x * 1 * sampleDistance, 0.0)).rgb * weight[1];
    result += texture(image, TexCoords - vec2(tex_offset.x * 1 * sampleDistance, 0.0)).rgb * weight[1];

    result += texture(image, TexCoords + vec2(tex_offset.x * 2 * sampleDistance, 0.0)).rgb * weight[2];
    result += texture(image, TexCoords - vec2(tex_offset.x * 2 * sampleDistance, 0.0)).rgb * weight[2];

    result += texture(image, TexCoords + vec2(tex_offset.x * 3 * sampleDistance, 0.0)).rgb * weight[3];
    result += texture(image, TexCoords - vec2(tex_offset.x * 3 * sampleDistance, 0.0)).rgb * weight[3];

    result += texture(image, TexCoords + vec2(tex_offset.x * 4 * sampleDistance, 0.0)).rgb * weight[4];
    result += texture(image, TexCoords - vec2(tex_offset.x * 4 * sampleDistance, 0.0)).rgb * weight[4];

    FragColor = vec4(result, 1.0);
}
