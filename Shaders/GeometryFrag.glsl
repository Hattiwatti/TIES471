#version 400 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoMetal;
layout (location = 3) out vec3 gRoughness;

in VertexData
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
} VertexIn;

uniform sampler2D texture_Diffuse;
uniform sampler2D texture_Metal;
uniform sampler2D texture_Roughness;

void main()
{
	gPosition = VertexIn.position;
	gNormal = VertexIn.normal;
	gAlbedoMetal.rgb = texture(texture_Diffuse, VertexIn.texCoord).rgb;
	gAlbedoMetal.a = texture(texture_Metal, VertexIn.texCoord).r;
	gRoughness.r = texture(texture_Roughness, VertexIn.texCoord).r;
}