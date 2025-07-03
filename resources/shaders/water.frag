#version 330 core

out vec4 FragColor;

#define MAX_LIGHTS 20
struct Light {
 float posX;
 float posY;
 float posZ;
 float colorR;

 float colorG;
 float colorB;
 float radius;
 float _padding0;
};

in vec4 WorldPos;
in vec4 ClipSpacePos;
in vec2 TexCoords;

uniform vec3 camPos;

uniform Light lights[MAX_LIGHTS];
uniform int lightsNr;

uniform float moveFactor;

uniform sampler2D baseTexture; // aka distortion texture
uniform sampler2D normalTexture;
uniform sampler2D refractionColor;
uniform sampler2D depthTexture;

const float waveStrength = 0.02;
const float shineDamper = 20.0;
const float reflectivity = 0.5;

void main() {
    vec2 ndc = ClipSpacePos.xy / ClipSpacePos.w;   
    vec2 refractTexCoords = ndc * 0.5 + 0.5;     
    
    vec2 distortedTexCoords = texture(baseTexture, vec2(TexCoords.x + moveFactor, TexCoords.y)).rg * 0.1;
	distortedTexCoords = TexCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
	vec2 totalDistortion = (texture(baseTexture, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

    refractTexCoords += totalDistortion;
    refractTexCoords = clamp(refractTexCoords, 0.001, 0.999); 

    vec4 normalMapColor = texture(normalTexture, distortedTexCoords);
    vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0);
    normal = normalize(normal);

    vec3 viewVector = normalize(camPos - WorldPos.xyz);

    vec3 specularHighlights = vec3(0.0);
   
   for(int i = 0; i < lightsNr; i++) {
    Light light = lights[i];
    vec3 lightPosition = vec3(light.posX, light.posY, light.posZ);
    vec3 lightColor = vec3(light.colorR, light.colorG, light.colorB);

    float distanceToLight = length(lightPosition - WorldPos.xyz);
    float attenuation = smoothstep(light.radius, 0, length(lightPosition - WorldPos.xyz));

    vec3 reflectedLight = reflect(normalize(WorldPos.xyz - lightPosition), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular, shineDamper);
	specularHighlights += lightColor * specular * reflectivity * attenuation;
   }

    FragColor = texture(refractionColor, refractTexCoords);
    FragColor = mix(FragColor, vec4(0.0, 0.1, 0.2, 1.0), 0.1) + vec4(specularHighlights, 0.0);
}