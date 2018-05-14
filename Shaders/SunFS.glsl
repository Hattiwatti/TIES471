#version 400 core
#define M_PI 3.1415926535897932384626433832795

layout(location = 0) out vec4 FragColor;

layout(std140) uniform ViewBlock
{
  mat4 ModelViewProj;
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

const vec3 SunColor = vec3(2, 2, 2);
const vec3 SunDirection = normalize(vec3(0, -1, -1));


float DGGX(vec3 N, vec3 H, float roughness)
{
  float a = roughness * roughness;
  float a2 = a * a;

  float dotNH = max(dot(N, H), 0.0);
  float dotNH2 = dotNH * dotNH;

  float denominator = (dotNH2*(a2 - 1.0) + 1.0);
  denominator = M_PI * denominator * denominator;

  return a2 / denominator;
}

float GSchlickGGX(vec3 N, vec3 V, float roughness)
{
  float dotNV = max(dot(N, V), 0.0);

  float r = roughness + 1.0;
  float k = (r*r) / 8.0;

  float denominator = dotNV * (1.0 - k) + k;

  return dotNV / denominator;
}

float GSmith(vec3 N, vec3 V, vec3 L, float roughness)
{
  float ggx1 = GSchlickGGX(N, V, roughness);
  float ggx2 = GSchlickGGX(N, L, roughness);

  return ggx1 * ggx2;
}

vec3 FSchlick(vec3 V, vec3 H, vec3 F0)
{
  float dotVH = max(dot(V, H), 0);
  return F0 + (1.0 - F0) * pow(1.0 - dotVH, 5.0);
}

// BRDF Functions

vec3 CookTorranceBRDF(vec3 fragPos, vec3 N, vec3 albedo, float metallic, float roughness, float IOR)
{
  vec3 L = -SunDirection;
  vec3 V = normalize(CameraPos - fragPos);
  vec3 H = normalize(L + V);

  float dotNL = max(dot(N, L), 0.0);
  float dotNV = max(dot(N, V), 0.0);

  if (dotNL <= 0)
    return vec3(0, 0, 0);

  vec3 F0 = vec3(abs((1.0 - IOR) / (1.0 + IOR)));
  F0 = F0 * F0;
  F0 = mix(F0, albedo, metallic);

  float D = DGGX(N, H, roughness);
  float G = GSmith(N, V, L, roughness);
  vec3 F = FSchlick(V, H, F0);

  vec3 specular = (D*F*G) / (4 * dotNL * dotNV);
  vec3 kS = F;
  vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

  return (kD * albedo / M_PI + specular) * SunColor * dotNL;
}

vec3 BlinnPhongBRDF(vec3 fragPos, vec3 N, vec3 albedo, float metallic)
{
  vec3 L = -SunDirection;
  vec3 V = normalize(CameraPos - fragPos);
  vec3 H = normalize(L + V);

  float dotNL = max(dot(N, L), 0.0);
  float dotNH = max(dot(N, H), 0.0);

  float specular = pow(dotNH, HardcodedSpecular) * dotNL;

  vec3 diffuse = dotNL * albedo * SunColor*vec3(0.5);
  return diffuse + specular * vec3(1.0);
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

  vec3 fragLight = vec3(0);
  switch (brdfMethod)
  {
  case 0:
    fragLight = BlinnPhongBRDF(fragPosition, fragNormal, fragAlbeido, fragMetallic);
    break;
  case 1:
    fragLight = CookTorranceBRDF(fragPosition, fragNormal, fragAlbeido, fragMetallic, fragRoughness, fragIOR);
    break;
  }

  FragColor = vec4(fragLight, 1.0);
}