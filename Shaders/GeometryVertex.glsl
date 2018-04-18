#version 450 core

layout(location=0) uniform mat4 MVPMatrix;
layout(location=1) uniform mat3 normalMatrix;

layout(location=0)in vec3 position;
layout(location=1)in vec3 normal;
layout(location=2)in vec2 texCoord;

out VertexData {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
} VertexOut;

void main()
{
  gl_Position = MVPMatrix * vec4(position, 1.0);
  VertexOut.position = position;
  VertexOut.normal = normalize(normalMatrix * normal);
  VertexOut.texCoord = texCoord;
}