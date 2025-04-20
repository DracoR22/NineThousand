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

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLight;
in mat3 TBN;

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int noPointLights;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D normal0;

uniform vec3 viewPos;

uniform bool u_flipLights;

vec3 RotateLight(vec3 lightPos, vec3 pivot, float angleRadians);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);

void main() {
      vec3 normal = texture(normal0, TexCoords).rgb;
	  normal = normal * 2.0 - 1.0;   
      normal = normalize(TBN * normal);

      //vec3 normal = normalize(Normal);

     vec3 viewDir = normalize(viewPos - FragPos);

     vec3 diffuseColor = texture(diffuse0, TexCoords).rgb;
     vec3 specularColor = texture(specular0, TexCoords).rgb;

     vec3 lightingResult = vec3(0.0);
     for (int i = 0; i < noPointLights; i++) {
        lightingResult += CalcPointLight(pointLights[i], normal, FragPos, viewDir, diffuseColor, specularColor);
     }

     vec3 color = lightingResult;

     FragColor = vec4(color, 1.0);
}

vec3 RotateLight(vec3 lightPos, vec3 pivot, float angleRadians) {
    float cosA = cos(angleRadians);
    float sinA = sin(angleRadians);

    vec3 pos = lightPos - pivot;

    vec3 rotated;
    rotated.x = pos.x * cosA - pos.z * sinA;
    rotated.z = pos.x * sinA + pos.z * cosA;
    rotated.y = pos.y;

    return rotated + pivot;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightPos = light.position;

    if (u_flipLights) {
      lightPos = RotateLight(light.position, fragPos, radians(180.0));
    }

    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 reflectDir = reflect(-lightDir, normal);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32); // material shininess

    float distance    = length(lightPos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
 
    vec3 ambient  = light.ambient  * diffuseColor;
    vec3 diffuse  = light.diffuse  * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;

    // ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 
