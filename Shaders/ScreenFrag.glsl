#version 400 core

layout (location = 0) out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D texture_position;
uniform sampler2D texture_normal;
uniform sampler2D texture_albedoMetal;
uniform sampler2D texture_roughness;
uniform sampler2D texture_depth;

uniform int method;
uniform vec3 viewPos;

const vec3 globalLight = normalize(vec3(1.7, -1, -1));
vec3 diffuseLighting(vec3 color, vec3 N, vec3 L)
{
  float I_d = max(0, dot(N, L));
  return I_d * color;
}

vec3 specularLighting(vec3 N, vec3 L, vec3 V)
{
  float I_s = 0;
  if (dot(N, L) > 0)
  {
    vec3 R = normalize(reflect(-L, N));
    I_s = pow(dot(V, R), 50.0);
    I_s = max(0, I_s);
  }

  return vec3(1, 1, 1)*I_s;
}

vec3 lambert(vec3 color)
{
  return color / 3.1413;
}

vec3 globalLightPass(vec3 diffuseColor, vec3 fragPos, vec3 fragNormal)
{
  vec3 lightDir = -globalLight;
  vec3 viewDir = normalize(viewPos - fragPos);

  vec3 diffuse = diffuseLighting(diffuseColor, fragNormal, lightDir);
  vec3 specular = specularLighting(fragNormal, lightDir, viewDir);

  return diffuse + specular;
}

void main()
{
  vec3 fragPosition = texture(texture_position, texCoord).rgb;
  vec3 fragNormal = texture(texture_normal, texCoord).rgb;
  vec4 fragDiffuseMetalness = texture(texture_albedoMetal, texCoord);
  float fragRoughness = texture(texture_roughness, texCoord).r;

  // Debug switches
  switch (method)
  {
  case 0:
    FragColor = vec4(globalLightPass(fragDiffuseMetalness.rgb, fragPosition, fragNormal), 1.0);
    break;
  case 1:
    FragColor = vec4(fragPosition, 1.0);
    break;
  case 2:
    FragColor = vec4(fragNormal, 1.0);
    break;
  case 3:
    FragColor = vec4(fragDiffuseMetalness.rgb, 1.0);
    break;
  case 4:
    FragColor = vec4(fragDiffuseMetalness.a, fragDiffuseMetalness.a, fragDiffuseMetalness.a, 1.0);
    break;
  case 5:
    FragColor = vec4(fragRoughness, fragRoughness, fragRoughness, 1.0);
    break;
  }
}