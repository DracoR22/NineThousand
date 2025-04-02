#version 330 core

out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform float gamma;
uniform float exposure;

void main()
{
    vec3 fragment = texture(screenTexture, TexCoords).rgb;
    vec3 bloom = texture(bloomTexture, TexCoords).rgb;

    vec3 colorWithBloom = fragment + bloom;

    vec3 toneMapped = vec3(1.0) - exp(-fragment * exposure);
    toneMapped = pow(toneMapped, vec3(1.0 / gamma));

     FragColor = vec4(fragment, 1.0);
     //FragColor = texture(bloomTexture, TexCoords); 
}
