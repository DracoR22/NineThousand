#version 330 core

out vec4 FragColor;

#define MAX_POINT_LIGHTS 20
struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLight;

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int noPointLights;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 viewPos;
uniform sampler2D shadowMap;


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 textureColor);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, PointLight light, vec3 fragPos);

void main() {
     vec3 norm = normalize(Normal);
     vec3 viewDir = normalize(viewPos - FragPos);
   
     vec4 textureColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);

     vec3 lightingResult = vec3(0.0);

    for (int i = 0; i < noPointLights; i++) {
        lightingResult += CalcPointLight(pointLights[i], norm, FragPos, viewDir, textureColor.rgb);
    }

     // Shadow calculation
     float shadow = ShadowCalculation(FragPosLight, norm, pointLights[0], FragPos);
     //lightingResult *= (1.0 - shadow);
     lightingResult = mix(lightingResult, lightingResult * 0.3, shadow);

     vec3 color = lightingResult * textureColor.rgb;

     // Gamma Correction
     float gamma = 2.0;
     color = pow(color, vec3(1.0 / gamma));

     FragColor = vec4(color, textureColor.a);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 textureColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 reflectDir = reflect(-lightDir, normal);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32); // material shininess

    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
 
    vec3 ambient  = light.ambient  * textureColor;
    vec3 diffuse  = light.diffuse  * diff * textureColor;
    vec3 specular = light.specular * spec * textureColor;

    // ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, PointLight light, vec3 fragPos)
{
   vec3 lightDir = normalize(light.position - fragPos);
   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
   projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0) {
      return 0.0;
    }

   float closestDepth = texture(shadowMap, projCoords.xy).r; 
   float currentDepth = projCoords.z;

   float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}  