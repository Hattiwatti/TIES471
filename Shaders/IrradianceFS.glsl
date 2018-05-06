#version 400 core
#define M_PI 3.1415926535897932384626433832795
#define EPSILON 0.00001;

layout(location = 0) out vec4 FragColor;

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
uniform samplerCube skyboxTexture;

uniform int method;
uniform int brdfMethod;

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


const vec4 magic = vec4(1111.1111, 3141.5926, 2718.2818, 0);
float rand(vec2 co) {
  return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 GenerateSampleVector(float roughness, float i, float sampleCount)
{
  float rand1 = rand(vec2(i, sampleCount));
  float rand2 = rand(vec2(i*179, sampleCount));

  float phi = atan((roughness*sqrt(rand1))/sqrt(1-rand1));
  float theta = 2 * M_PI*rand2;

  float x = cos(theta)*sin(phi);
  float z = sin(theta)*sin(phi);
  float y = cos(phi);

  return vec3(x, y, z);
}

mat3 CalculateMatrix(vec3 up)
{
  vec3 first = vec3(up.y, up.x, 0);
  vec3 second = normalize(cross(up, first));

  return mat3(second, up, normalize(cross(second, up)));
}

const int SampleCount = 1;

vec3 GatherSpecular(vec3 N, vec3 V, float roughness, vec3 F0, out vec3 ks)
{
  vec3 reflection = reflect(-V, N);
  mat3 worldMatrix = CalculateMatrix(reflection);

  float a = roughness;
  float dotNV = clamp(dot(N, V), 0.0, 1.0);

  vec3 radiance = vec3(0);
  for (int i = 0; i < SampleCount; ++i)
  {
    vec3 sampleVector = reflection; // GenerateSampleVector(roughness, i, 10);
    //sampleVector = normalize(worldMatrix * sampleVector);

    vec3 H = normalize(sampleVector + V);
    float cosT = clamp(dot(sampleVector, N), 0.0, 1.0);
    float sinT = sqrt(1 - cosT * cosT);

    float G = GGGX(N, V, H, a);//GCookTorrance(N, V, sampleVector, H);
    vec3 F = FCookTorrance(V, H, F0);
    float dotNH = max(0, min(1, dot(N, H)));
    float denominator = clamp((4 * (dotNV * dotNH) + 0.005), 0.0, 1.0);

    vec3 specularColor = texture(skyboxTexture, sampleVector).rgb;

    ks += F;
    radiance += (specularColor * G * F * sinT) / denominator;
  }

  ks = clamp(ks / SampleCount, 0.0, 1.0);
  return clamp(radiance / SampleCount, 0.0, 1.0);
}

vec3 CalculateIrradiance(vec3 fragPos, vec3 fragNormal, vec3 fragAlbeido, float fragMetallic, float fragRoughness)
{
  vec3 viewDir = normalize(CameraPos - fragPos);
  vec3 irradiance = texture(skyboxTexture, fragNormal).rgb;
  vec3 diffuse = fragAlbeido * irradiance;

  float IOR = 1.5;
  vec3 F0 = vec3(abs((1.0 - IOR) / (1.0 + IOR)));
  F0 = F0 * F0;
  F0 = mix(F0, fragAlbeido, fragMetallic);

  vec3 ks = vec3(0);
  vec3 specular = GatherSpecular(fragNormal, viewDir, fragRoughness, F0, ks);
  vec3 kd = (vec3(1.0) - ks) * (1 - fragMetallic);

  return kd * diffuse + specular;
}

void main()
{
  vec3 fragPosition = texture(texture_position, texCoord).rgb;
  vec3 fragNormal = texture(texture_normal, texCoord).rgb;
  vec4 fragDiffuseMetallic = texture(texture_albedoMetal, texCoord);

  vec3 fragAlbeido = fragDiffuseMetallic.rgb * AlbeidoMultiplier;
  float fragMetallic = fragDiffuseMetallic.a * MetallicMultiplier;
  float fragRoughness = texture(texture_roughness, texCoord).r * RoughnessMultiplier;

  fragRoughness = clamp(fragRoughness - 0.001, 0.0, 1.0) + 0.001;

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