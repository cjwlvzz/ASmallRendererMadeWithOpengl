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
struct DirectionalLight {
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

#define POINT_LIGHTS 4

uniform PointLight pointLights[POINT_LIGHTS];

uniform DirectionalLight directionalLight;

uniform PointLight pointLight;

uniform SpotLight spotLight;

uniform Material material;

void main() {
    vec3 viewDir = normalize(viewPos - outFragPos);
    vec3 normal = normalize(outNormal);

    //directional light
    vec3 outPut = Calc_DirectionalLight(directionalLight, normal, viewDir);

    //point light
    for(int i = 0; i < POINT_LIGHTS; i++)
    {
      outPut += Calc_PointLight(pointLights[i], normal, outFragPos, viewDir);
    }

    //spot light
    outPut += Calc_SpotLight(spotLight, normal, outFragPos, viewDir);

    FragColor = vec4(outPut, 1.0);

}

// calculate the attributes of the directional light
vec3 Calc_DirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
  //----------Vector and scalar calculations----------
  vec3 lightDir = normalize(-light.direction);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  //----------Calculate the directional light attribution----------
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, outTexCoord));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, outTexCoord));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, outTexCoord));

  return (ambient + diffuse + specular);
}

// calculate the attributes of the point light
vec3 Calc_PointLight(PointLight light,vec3 normal, vec3 fragPos, vec3 viewDir)
{
  //----------Vector and scalar calculations----------
  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  //----------Calculate the point light attribution----------
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, outTexCoord));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, outTexCoord));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, outTexCoord));

  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  return (ambient + diffuse + specular);

}

// calculate the attributes of the spot light
vec3 Calc_SpotLight(SpotLight light,vec3 normal, vec3 fragPos, vec3 viewDir)
{
  //----------Vector and scalar calculations----------
  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

  float distance = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = (light.cutOff - light.outerCutOff);
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

  //----------Calculate the spot light attribution----------
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, outTexCoord));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, outTexCoord));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, outTexCoord));

  ambient *= attenuation * intensity;
  diffuse *= attenuation * intensity;
  specular *= attenuation * intensity;

  return (ambient + diffuse + specular);
}