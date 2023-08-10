#version 330 core
out vec4 FragColor;
in vec2 outTexCoord;
in vec3 outNormal;
in vec3 outFragPos;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float ambientLightIntensity;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {

  //set an constant ambient light
  vec3 ambient = ambientLightIntensity * lightColor;

  vec3 norm = normalize(outNormal);

  vec3 lightDir = normalize(lightPos - outFragPos);

  float diff = max(dot(norm, lightDir), 0.0);

  //the diffuse part of the phong model
  vec3 diffuse = diff * lightColor;

  float specularStrength = 1.5;

  vec3 viewDir = normalize(viewPos - outFragPos);

  vec3 reflectDir = reflect(-lightDir, norm);

  //the specular part of the phong model
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

  vec3 specular = specularStrength * spec * lightColor;

  vec3 baseColor = vec3(1.0f, 0.5f, 0.31f);
  // vec4 baseColor = mix(texture(texture1, outTexCoord), texture(texture2, outTexCoord), 0.1);

  vec3 outPutColor = (ambient + diffuse + specular) * vec3(baseColor);

  FragColor = vec4(outPutColor, 1.0);
}