#version 400 core
#define M_PI 3.1415926535897932384626433832795

layout (location = 0) out vec3 FragColor;

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

//http://graphicrants.blogspot.fi/2013/08/specular-brdf-reference.html
// NORMAL DISTRIBUTION FUNCTIONS

float DBlinn(vec3 n, vec3 h, float a)
{
  float a2 = a * a;
  float dotNH = dot(n, h);
  float exponent = 2/a2 - 2;

  return 1 / (M_PI*a2) * pow(dotNH, exponent);
}

// GEOMETRIC SHADOWING
float GImplicit(vec3 n, vec3 h, vec3 l, vec3 v)
{
  float dotNV = dot(n, v);
  float dotNL = dot(n, l);
  return dotNV * dotNL;
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

vec3 CookTorranceBRDF(vec3 fragPos, vec3 fragNormal, vec3 albeido, float metalness, float roughness)
{
  float a = roughness * roughness;
  
  vec3 lightDir = normalize(fragNormal);
  vec3 viewDir = normalize(CameraPos - fragPos);
  vec3 halfway = normalize(lightDir + viewDir);

  float dotNL = 1.0;//dot(fragNormal, lightDir);
  float dotNV = dot(fragNormal, viewDir);
  float IOR = 1.5;
  vec3 F0 = vec3(abs((1.0 - IOR) / (1.0 + IOR)));
  F0 = F0 * F0;
  F0 = mix(F0, albeido, metalness);

  float D = DBlinn(fragNormal, halfway, a);
  vec3 F = FSchlick(halfway, viewDir, F0);
  float G = GImplicit(fragNormal, halfway, lightDir, viewDir);

  vec3 specular = (D*F*G) / (4 * dotNL*dotNV);
  vec3 irradiance = texture(skyboxTexture, fragNormal).rgb;
  vec3 diffuse = albeido * irradiance * 0.2;

  return diffuse; //+ specular;
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
  case 0: case 6: case 7:
    FragColor = CookTorranceBRDF(fragPosition,
      fragNormal,
      fragDiffuseMetalness.rgb * AlbeidoMultiplier,
      fragDiffuseMetalness.a * MetallicMultiplier,
      fragRoughness * RoughnessMultiplier);
    break;
  }
}