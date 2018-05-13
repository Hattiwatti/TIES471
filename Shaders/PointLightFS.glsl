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
  int method;
  int brdfMethod;
};

in vec2 texCoord;

uniform sampler2D texture_position;
uniform sampler2D texture_normal;
uniform sampler2D texture_albedo;
uniform sampler2D texture_surface;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightRadius;

//http://graphicrants.blogspot.fi/2013/08/specular-brdf-reference.html
// NORMAL DISTRIBUTION FUNCTIONS

float DBlinn(vec3 n, vec3 h, float a)
{
  float a2 = a * a;
  float dotNH = dot(n, h);
  float exponent = 2 / a2 - 2;

  return 1 / (M_PI*a2) * pow(dotNH, exponent);
}

float DGCX(vec3 N, vec3 H, float a)
{
  float a2 = a * a;
  float dotNH = dot(N, H);

  float denominator = dotNH * dotNH*(a2 - 1) + 1;
  return a2 / (M_PI*denominator*denominator);
}

// GEOMETRIC SHADOWING
float GImplicit(vec3 n, vec3 h, vec3 l, vec3 v)
{
  float dotNV = dot(n, v);
  float dotNL = dot(n, l);
  return dotNV * dotNL;
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

// FRESNEL
float FNone(vec3 h, vec3 v, float F0)
{
  return F0;
}

vec3 FSchlick(vec3 h, vec3 v, vec3 F0)
{
  float dotVH = dot(v, h);
  return F0 + (vec3(1) - F0)*pow(1 - dotVH, 5);
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

// BRDF Functions

vec3 CookTorranceBRDF(vec3 N, vec3 V, vec3 L, vec3 H, vec3 albeido, float metallic, float roughness)
{
  float a = roughness * roughness;

  float dotNL = dot(N, L);
  float dotNV = dot(N, V);

  float IOR = 1.1;
  vec3 F0 = vec3(abs((1.0 - IOR) / (1.0 + IOR)));
  F0 = F0 * F0;
  F0 = mix(F0, albeido, metallic);

  float D = DGCX(N, H, a);
  vec3 F = FCookTorrance(V, H, F0);//FSchlick(H, V, F0);
  float G = GCookTorrance(N, V, L, H);

  vec3 specular = (D*F*G) / (4 * dotNL*dotNV);
  return max(specular, vec3(0));
}

vec3 BlinnPhongBRDF(vec3 N, vec3 L, vec3 H, float metallic)
{
  float dotNH = dot(N, H);
  float dotNL = dot(N, L);
  dotNH = max(0.0001, min(1, dotNH));

  float intensity = pow(dotNH, metallic) * sqrt(dotNL);
  return intensity * vec3(1.0);
}

vec3 CalculateLighting(vec3 fragPos, vec3 fragNormal, vec3 fragAlbeido, float fragMetallic, float fragRoughness, float fragIOR)
{
  vec3 lightDir = lightPosition - fragPos;
  float distance = length(lightDir);
  if (distance > lightRadius)
    return vec3(0, 0, 0);

  lightDir /= distance;
  vec3 viewDir = normalize(CameraPos - fragPos);
  vec3 halfway = normalize(lightDir + viewDir);

  float dotNL = dot(fragNormal, lightDir);
  dotNL = max(0, dotNL);

  vec3 SpecularColor = vec3(0);
  if (dotNL == 0)
    return vec3(0);

  switch (brdfMethod)
  {
  case 0:
    SpecularColor = BlinnPhongBRDF(fragNormal, lightDir, halfway, 2 / pow(fragRoughness, 4) - 2);
    break;
  case 1:
    SpecularColor = CookTorranceBRDF(fragNormal, viewDir, lightDir, halfway, fragAlbeido, fragMetallic, fragRoughness);
    break;
  }

  float d2 = distance * distance;
  float r2 = lightRadius * lightRadius;
  float denominator = 1 + 2 * distance / lightRadius + d2 / r2;

  float attenuation = 1 / denominator;

  return attenuation * dotNL * fragAlbeido * lightColor + SpecularColor;
}

void main()
{
  vec3 fragPosition = texture(texture_position, texCoord).rgb;
  vec3 fragNormal = texture(texture_normal, texCoord).rgb;
  vec3 surface = texture(texture_surface, texCoord).rgb;

  vec3 fragAlbeido = texture(texture_albedo, texCoord).rgb * AlbeidoMultiplier;
  float fragMetallic = surface.r * MetallicMultiplier;
  float fragRoughness = surface.b * RoughnessMultiplier;
  float fragIOR = surface.g;

  vec3 fragLight = CalculateLighting(fragPosition, fragNormal, fragAlbeido, fragMetallic, fragRoughness, fragIOR);
  FragColor = vec4(fragLight, 1.0);
}