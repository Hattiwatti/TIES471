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

uniform sampler2D AlbedoTex;
uniform sampler2D NormalTex;
uniform sampler2D MetallicIORTex;
uniform sampler2D RoughnessTex;

uniform bool UseTextures;
uniform vec3 MaterialColor;
uniform float MaterialMetallic;
uniform float MaterialRoughness;
uniform float MaterialIOR;

void main()
{
  gPosition = VertexIn.position;

  if (UseTextures)
  {
    vec3 bump = 2 * texture(NormalTex, VertexIn.texCoord).rgb - vec3(1.0);

    gNormal = normalize(VertexIn.TBN * bump);
    gAlbedo.rgb = texture(AlbedoTex, VertexIn.texCoord).rgb;
    gSurface.r = texture(MetallicIORTex, VertexIn.texCoord).r;
    gSurface.g = texture(MetallicIORTex, VertexIn.texCoord).g;
    gSurface.b = texture(RoughnessTex, VertexIn.texCoord).r;
  }
  else
  {
    gNormal = normalize(VertexIn.TBN * vec3(0, 0, 1));
    gAlbedo = MaterialColor;
    gSurface.r = MaterialMetallic;
    gSurface.g = MaterialIOR;
    gSurface.b = MaterialRoughness;
  }
}