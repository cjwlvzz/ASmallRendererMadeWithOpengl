#version 330 core
out vec4 FragColor;
in vec2 outTexCoord;
in vec3 outNormal;
in vec3 outFragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D texture1;
uniform sampler2D texture2;

//define material struct
struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

//define light struct
struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform Light light;

uniform Material material;

void main() {

  vec3 diffuseColor = texture(material.diffuse, outTexCoord).rgb;
  vec3 specularRange = texture(material.specular, outTexCoord).rgb;

  //set an constant ambient light
  vec3 ambient = light.ambient * diffuseColor;

  vec3 norm = normalize(outNormal);

  vec3 lightDir = normalize(lightPos - outFragPos);

  float diff = max(dot(norm, lightDir), 0.0);

  //the diffuse part of the phong model
  vec3 diffuse = light.diffuse * diff * diffuseColor;

  float specularStrength = 1.5;

  vec3 viewDir = normalize(viewPos - outFragPos);

  vec3 reflectDir = reflect(-lightDir, norm);

  //the specular part of the phong model
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  vec3 specular = light.specular * spec * specularRange;

  vec3 baseColor = vec3(1.0f, 1.0f, 1.0f);
  // vec4 baseColor = mix(texture(texture1, outTexCoord), texture(texture2, outTexCoord), 0.1);

  vec3 outPutColor = (ambient + diffuse + specular) * vec3(baseColor);

  FragColor = vec4(outPutColor, 1.0);
}