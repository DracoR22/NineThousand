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

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int noPointLights;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 viewPos;


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 textureColor);

void main() {
     vec3 norm = normalize(Normal);
     vec3 viewDir = normalize(viewPos - FragPos);
   
     vec4 textureColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);

     vec3 lightingResult = vec3(0.0);

    for (int i = 0; i < noPointLights; i++) {
        lightingResult += CalcPointLight(pointLights[i], norm, FragPos, viewDir, textureColor.rgb);
    }

     FragColor = vec4(lightingResult, textureColor.a);
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