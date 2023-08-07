#version 330 core
out vec4 FragColor;
in vec2 outTexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 lightColor;

void main() {
  FragColor = mix(texture(texture1, outTexCoord), texture(texture2, outTexCoord), 0.1);
  // The rgb value will be (0.0, 0.5, 0.0), so it will be a dark green
  // FragColor = vec4(vec3(1.0f, 0.5f, 0.31f) * lightColor, 1.0);
}