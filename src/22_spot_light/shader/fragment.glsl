#version 330 core
out vec4 FragColor;
in vec2 outTexCoord;
in vec3 outNormal;
in vec3 outFragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;

//define material struct
struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D specularColor;
  sampler2D emission;
  float shininess;
};

//define directional light struct
struct Light {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

//define point light struct
struct PointLight {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float constant;
  float linear;
  float quadratic;
};

//define spot light struct
struct SpotLight {
  vec3 position;
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float constant;
  float linear;
  float quadratic;
  float cutOff;
  float outerCutOff;
};

uniform Light light;

uniform PointLight pointLight;

uniform SpotLight spotLight;

uniform Material material;

void main() {

  vec3 spotLightDir = normalize(spotLight.position - outFragPos);

  vec3 diffuseColor = texture(material.diffuse, outTexCoord).rgb;

  vec3 specularColor = texture(material.specularColor, outTexCoord).rgb;

  vec3 specularRange = texture(material.specular, outTexCoord).rgb;

  float theta = dot(spotLightDir, normalize(-spotLight.direction));

  float epsilon = (spotLight.cutOff - spotLight.outerCutOff);

  float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

  //set an constant ambient light
  vec3 ambient = spotLight.ambient * diffuseColor;

  vec3 norm = normalize(outNormal);

  float diff = max(dot(norm, spotLightDir), 0.0);

  //the diffuse part of the phong model
  vec3 diffuse = spotLight.diffuse * diff * diffuseColor;

  vec3 viewDir = normalize(viewPos - outFragPos);

  vec3 reflectDir = reflect(-spotLightDir, norm);

  //the specular part of the phong model
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  vec3 specular = light.specular * spec * specularRange;

  vec3 baseColor = vec3(1.0f, 1.0f, 1.0f);
  // vec4 baseColor = mix(texture(texture1, outTexCoord), texture(texture2, outTexCoord), 0.1);

  //soften the edge of the spot light
  diffuse *= intensity;
  specular *= intensity;

  //calculate the attenuation
  float distance = length(spotLight.position - outFragPos);
  float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));

  diffuse *= attenuation;
  specular *= attenuation;

  //enlighten the scene a little bit
  vec3 outPutColor = (ambient + diffuse + specular) * vec3(baseColor) * 2.0;

  FragColor = vec4(outPutColor, 1.0);
}