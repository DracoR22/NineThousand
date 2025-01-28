#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
     vec3 lightColor = vec3(1.0, 1.0, 1.0);
     vec3 objectColor = vec3(1.0, 1.0, 1.0);

     float ambientStrength = 0.1;
     vec3 ambient = ambientStrength * lightColor;
     
     vec3 norm = normalize(Normal);
     vec3 lightDir = normalize(lightPos - FragPos);

     float specularStrength = 0.5;
     vec3 viewDir = normalize(viewPos - FragPos);
     vec3 reflectDir = reflect(-lightDir, norm);  

     float diff = max(dot(norm, lightDir), 0.0);
     vec3 diffuse = diff * lightColor;
     
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
     vec3 specular = specularStrength * spec * lightColor;  

     vec3 result = (ambient + diffuse + specular) * objectColor;
     vec4 textureColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);


     FragColor = vec4(textureColor.rgb * result, textureColor.a);
}