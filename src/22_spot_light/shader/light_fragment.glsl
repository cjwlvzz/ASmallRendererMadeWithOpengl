#version 330 core
out vec4 FragColor;
in vec2 outTexCoord;
uniform vec3 lightColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
  FragColor = vec4(lightColor,1.0);
  FragColor = vec4(1.0);
}