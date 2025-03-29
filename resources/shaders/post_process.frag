#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float gamma;
uniform float exposure;

void main()
{
    vec4 fragment = texture(screenTexture, TexCoords);

    vec3 toneMapped = vec3(1.0) - exp(-fragment.rgb * exposure);
    toneMapped = pow(toneMapped, vec3(1.0 / gamma));

    FragColor = vec4(toneMapped, fragment.a);
}
