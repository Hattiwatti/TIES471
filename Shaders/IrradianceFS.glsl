#version 400 core
#define M_PI 3.1415926535897932384626433832795
#define EPSILON 0.00001;

layout(location = 0) out vec4 FragColor;

layout(std140) uniform ViewBlock
{
  mat4 ViewProj;
  vec3 CameraPos;
  float AlbeidoMultiplier;
  float MetallicMultiplier;
  float RoughnessMultiplier;
  int method;
  int brdfMethod;
};

in vec2 texCoord;

uniform sampler2D PositionTex;
uniform sampler2D NormalTex;
uniform sampler2D AlbedoTex;
uniform sampler2D SurfaceTex;

uniform samplerCube IrradianceTex;

float GGGX(vec3 N, vec3 V, vec3 H, float a)
{
  float dotVH = clamp(dot(V, H), 0.0, 1.0);
  float dotVN = clamp(dot(V, N), 0.0, 1.0);

  float chi = (dotVH / dotVN) > 0 ? 1 : 0;
  float dotVH2 = dotVH * dotVH;
  float tan2 = (1 - dotVH2) / dotVH2;

  return(chi * 2) / (1 + sqrt(1 + a * a*tan2));
}

float GCookTorrance(vec3 N, vec3 V, vec3 L, vec3 H)
{
  float dotNV = dot(N, V);
  float dotNH = dot(N, H);
  float dotVH = dot(V, H);
  float dotNL = dot(N, L);

  float first = (2 * dotNH*dotNV) / dotVH;
  float second = (2 * dotNH*dotNL) / dotVH;

  return min(1, min(first, second));
}

vec3 FCookTorrance(vec3 V, vec3 H, vec3 F0)
{
  vec3 rootF0 = sqrt(F0);
  vec3 n = (vec3(1) + rootF0) / (vec3(1) - rootF0);

  vec3 c = vec3(dot(V, H));
  vec3 g = sqrt(n*n + c * c - vec3(1));

  vec3 term1 = (g - c) / (g + c);
  vec3 term2 = ((g + c)*c - vec3(1)) / ((g - c)*c + vec3(1));

  return 0.5 * term1 * term1 * (vec3(1) + term2 * term2);
}

vec3 FSchlick(vec3 h, vec3 v, vec3 F0, float roughness)
{
  float dotVH = dot(v, h);
  return F0 + (max(vec3(1.0 - roughness), F0) - F0)*pow(1.0 - dotVH, 5.0);
}

vec3 CalculateIrradiance(vec3 fragPos, vec3 fragNormal, vec3 fragAlbeido, float fragMetallic, float fragRoughness)
{
  vec3 irradiance = texture(IrradianceTex, fragNormal).rgb;
  vec3 diffuse = fragAlbeido * irradiance;

  return diffuse;
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
    FragColor = vec4(CalculateIrradiance(fragPosition, fragNormal, fragAlbeido, fragMetallic, fragRoughness), 1.0f);
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