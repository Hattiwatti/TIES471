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

const mat4 shadowBiasMatrix = mat4(vec4(0.5, 0, 0, 0),
  vec4(0, 0.5, 0, 0),
  vec4(0, 0, 0.5, 0),
  vec4(0.5, 0.5, 0.5, 1.0));

const vec3 globalLight = normalize(vec3(1.7, -1, 1));
const vec3 globalLightColor = vec3(0.5, 0.5, 0.5);
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
  vec4 fragLightProj = shadowBiasMatrix * LightMVP * vec4(fragPos, 1);
  float shadowDepth = texture(shadowMap, fragLightProj.xy).z;
  float fragDepth = fragLightProj.z;

  float a = roughness * roughness;
  
  vec3 lightDir = normalize(-globalLight);
  vec3 viewDir = normalize(CameraPos - fragPos);
  vec3 halfway = normalize(lightDir + viewDir);

  float dotNL = dot(fragNormal, lightDir);
  if (dotNL < 0)
    return albeido * vec3(0.1);

  if (method == 7)
  {
    if(fragDepth > (shadowDepth+0.01))
      return albeido * vec3(0.1);
  }


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
  vec3 diffuse = albeido * irradiance * dotNL;

  if (method == 6)
    return irradiance;

  specular = max(specular, vec3(0));

  //specular = specular, vec3(0), vec3(1));
  vec3 finalColor = diffuse + specular;
  return pow(finalColor, vec3(1.0 / 2.2));
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
    //FragColor = vec4(globalLightPass(fragDiffuseMetalness.rgb, fragPosition, fragNormal), 1.0);
    FragColor = vec4( CookTorranceBRDF(fragPosition, 
                                       fragNormal, 
                                       fragDiffuseMetalness.rgb * AlbeidoMultiplier,
                                       fragDiffuseMetalness.a * MetallicMultiplier, 
                                       fragRoughness * RoughnessMultiplier), 1.0);
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