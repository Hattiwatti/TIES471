#version 400 core
#define M_PI 3.1415926535897932384626433832795

layout (location = 0) out vec4 FragColor;

layout(std140) uniform ViewBlock
{
  mat4 ModelViewProj;
  vec3 CameraPos;
  float AlbeidoMultiplier;
  float MetallicMultiplier;
  float RoughnessMultiplier;
};

in vec2 texCoord;

uniform sampler2D texture_position;
uniform sampler2D texture_normal;
uniform sampler2D texture_albedoMetal;
uniform sampler2D texture_roughness;
uniform sampler2D shadowMap;
uniform samplerCube skyboxTexture;

uniform mat4 LightMVP;
uniform int method;
uniform int brdfMethod;

const mat4 shadowBiasMatrix = mat4(vec4(0.5, 0, 0, 0),
  vec4(0, 0.5, 0, 0),
  vec4(0, 0, 0.5, 0),
  vec4(0.5, 0.5, 0.5, 1.0));

const vec3 globalLight = normalize(vec3(1.7, -1, 1));
const vec3 globalLightColor = vec3(1, 1, 1);

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
  vec3 viewDir = normalize(CameraPos - fragPos);

  vec3 diffuse = diffuseLighting(diffuseColor, fragNormal, lightDir);
  vec3 specular = specularLighting(fragNormal, lightDir, viewDir);

  return diffuse + specular;
}

//http://graphicrants.blogspot.fi/2013/08/specular-brdf-reference.html
// NORMAL DISTRIBUTION FUNCTIONS

float DBlinn(vec3 n, vec3 h, float a)
{
  float a2 = a * a;
  float dotNH = dot(n, h);
  float exponent = 2/a2 - 2;

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
  vec3 g = sqrt(n*n + c*c - vec3(1));

  vec3 term1 = (g - c) / (g + c);
  vec3 term2 = ((g + c)*c - vec3(1)) / ((g - c)*c + vec3(1));

  return 0.5 * term1 * term1 * (vec3(1) + term2* term2);
}

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

vec3 CalculateLighting(vec3 fragPos, vec3 fragNormal, vec3 fragAlbeido, float fragMetallic, float fragRoughness)
{
  vec4 fragLightProj = shadowBiasMatrix * LightMVP * vec4(fragPos, 1);
  float shadowDepth = texture(shadowMap, fragLightProj.xy).z;
  float fragDepth = fragLightProj.z;

  vec3 lightDir = -globalLight;
  vec3 viewDir = normalize(CameraPos - fragPos);
  vec3 halfway = normalize(lightDir + viewDir);

  float dotNL = dot(fragNormal, lightDir);
  dotNL = max(0, dotNL);

  vec3 AmbientColor = vec3(0.1) * fragAlbeido;
  vec3 SpecularColor = vec3(0);

  if (dotNL == 0 || fragDepth > (shadowDepth + 0.01))
    return AmbientColor;

  switch (brdfMethod)
  {
  case 0:
    SpecularColor = BlinnPhongBRDF(fragNormal,lightDir, halfway, 2/pow(fragRoughness, 4) - 2);
    break;
  case 1:
    SpecularColor = CookTorranceBRDF(fragNormal, viewDir, lightDir, halfway, fragAlbeido, fragMetallic, fragRoughness);
    break;
  }

  return AmbientColor + dotNL * fragAlbeido + SpecularColor;
}

void main()
{
  vec3 fragPosition = texture(texture_position, texCoord).rgb;
  vec3 fragNormal = texture(texture_normal, texCoord).rgb;
  vec4 fragDiffuseMetallic = texture(texture_albedoMetal, texCoord);

  vec3 fragAlbeido = fragDiffuseMetallic.rgb * AlbeidoMultiplier;
  float fragMetallic = fragDiffuseMetallic.a * MetallicMultiplier;
  float fragRoughness = texture(texture_roughness, texCoord).r * RoughnessMultiplier;

  // Debug switches
  switch (method)
  {
  case 0: case 6: case 7:
    FragColor = vec4(CalculateLighting(fragPosition, fragNormal, fragAlbeido, fragMetallic, fragRoughness), 1.0f);
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