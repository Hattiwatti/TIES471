#version 400 core

layout (location = 0) out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D texture_position;
uniform sampler2D texture_normal;
uniform sampler2D texture_albedoMetal;
uniform sampler2D texture_roughness;

const vec3 globalLightDirection = normalize(vec3(0, -1, -1));

vec3 globalLightPass(vec3 diffuseColor, vec3 normal)
{
  float lightIntensity = dot(normal, -globalLightDirection);
  lightIntensity = max(0, lightIntensity);

  return diffuseColor* lightIntensity;
}

void main()
{
  vec3 fragPosition = texture(texture_position, texCoord).rgb;
  vec3 fragNormal = texture(texture_normal, texCoord).rgb;
  vec3 fragDiffuse = texture(texture_albedoMetal, texCoord).rgb;

  FragColor = vec4(globalLightPass(fragDiffuse, fragNormal), 1.0);
}