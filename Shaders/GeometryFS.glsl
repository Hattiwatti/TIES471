#version 400 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gSurface;

in VertexData
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
  mat3 TBN;
} VertexIn;

uniform sampler2D texture_Diffuse;
uniform sampler2D texture_Normal;
uniform sampler2D texture_MetallicIOR;
uniform sampler2D texture_Roughness;

uniform bool useTextures;
uniform vec3 color;
uniform float metallic;
uniform float roughness;
uniform float IOR;

void main()
{
  gPosition = VertexIn.position;

  if (useTextures)
  {
    vec3 bump = 2 * texture(texture_Normal, VertexIn.texCoord).rgb - vec3(1.0);

    gNormal = normalize(VertexIn.TBN * bump);
    gAlbedo.rgb = texture(texture_Diffuse, VertexIn.texCoord).rgb;
    gSurface.r = texture(texture_MetallicIOR, VertexIn.texCoord).r;
    gSurface.g = texture(texture_MetallicIOR, VertexIn.texCoord).g;
    gSurface.b = texture(texture_Roughness, VertexIn.texCoord).r;
  }
  else
  {
    gNormal = normalize(VertexIn.TBN * vec3(0, 0, 1));
    gAlbedo = color;
    gSurface.r = metallic;
    gSurface.g = IOR;
    gSurface.b = roughness;
  }
}