#version 400 core
#define M_PI 3.141592653589
#define MAX_ENVLOD 7

layout(location = 0) out vec4 FragColor;

layout(std140) uniform ViewBlock
{
  mat4 ViewProj;
  vec3 CameraPos;
  float AlbeidoMultiplier;
  float MetallicMultiplier;
  float RoughnessMultiplier;
  float HardcodedSpecular;
  int method;
  int brdfMethod;
};

in vec2 texCoord;

uniform sampler2D PositionTex;
uniform sampler2D NormalTex;
uniform sampler2D AlbedoTex;
uniform sampler2D SurfaceTex;

uniform samplerCube IrradianceTex;
uniform sampler2D BRDFLutTex;

uniform samplerCube PrefilteredTex[MAX_ENVLOD];

// This shader is for calculating global illumination from pre-calculated textures
// https://learnopengl.com/PBR/IBL/Specular-IBL
// The textures are generated with cmftStudio, pre-calculated BRDF LUT is taken from the
// link above.

vec3 FSchlick(vec3 h, vec3 v, vec3 F0, float roughness)
{
  float dotVH = dot(v, h);
  return F0 + (max(vec3(1.0 - roughness), F0) - F0)*pow(1.0 - dotVH, 5.0);
}

vec3 RetrieveEnvLod(vec3 R, float roughness)
{
  // Retrieve pre-calculated irradiance from textures based on roughness.

  float lodLevel = (MAX_ENVLOD-1) * roughness;

  int first = int(lodLevel);
  int second = min(first + 1, MAX_ENVLOD-1);
  float lerp = lodLevel - first;

  vec3 color1 = texture(PrefilteredTex[first], R).rgb;
  vec3 color2 = texture(PrefilteredTex[second], R).rgb;

  return mix(color1, color2, lerp);
}

vec3 CalculateIrradiance(vec3 fragPos, vec3 fragNormal, vec3 albedo, float metallic, float roughness, float IOR)
{
  vec3 V = normalize(CameraPos - fragPos);
  vec3 N = normalize(fragNormal);
  vec3 R = normalize(reflect(-V, N));

  float dotNV = min(max(dot(N, V), 0), 0.99);

  vec3 irradiance = texture(IrradianceTex, fragNormal).rgb;
  vec3 diffuse = albedo * irradiance;

  vec3 F0 = vec3(abs((1.0 - IOR) / (1.0 + IOR)));
  F0 = F0 * F0;
  F0 = mix(F0, albedo, metallic);

  vec3 F = FSchlick(fragNormal, V, F0, roughness);
  vec3 kD = (1.0 - F) * (1.0-metallic);

  vec2 envBRDF = texture(BRDFLutTex, vec2(dotNV, roughness)).rg;
  vec3 prefilteredColor = RetrieveEnvLod(R, roughness);

  vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
  return (kD * diffuse + specular);
}

void main()
{
  vec3 fragPosition = texture(PositionTex, texCoord).rgb;
  vec3 fragNormal = texture(NormalTex, texCoord).rgb;
  vec3 surface = texture(SurfaceTex, texCoord).rgb;

  vec3 fragAlbeido = texture(AlbedoTex, texCoord).rgb * AlbeidoMultiplier;
  float fragMetallic = surface.r * MetallicMultiplier;
  float fragRoughness = surface.b * RoughnessMultiplier;
  float fragIOR = surface.g;

  // Debug switches
  switch (method)
  {
  case 0: case 6: case 7:
    FragColor = vec4(CalculateIrradiance(fragPosition, fragNormal, fragAlbeido, fragMetallic, fragRoughness, fragIOR), 1.0f);
    break;
  case 1:
    FragColor = vec4(fragPosition, 1.0);
    break;
  case 2:
    FragColor = vec4(fragNormal, 1.0);
    break;
  case 3:
    FragColor = vec4(fragAlbeido, 1.0);
    break;
  case 4:
    FragColor = vec4(fragMetallic, fragMetallic, fragMetallic, 1.0);
    break;
  case 5:
    FragColor = vec4(fragRoughness, fragRoughness, fragRoughness, 1.0);
    break;
  }
}