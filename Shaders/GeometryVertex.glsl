#version 400 core

uniform mat4 MVPMatrix;
uniform mat3 normalMatrix;

layout(location = 0)in vec3 position;
layout(location = 1)in vec3 normal;
layout(location = 2)in vec3 tangent;
layout(location = 3)in vec2 texCoord;

out VertexData {
	vec3 position;
	vec3 normal;
	vec2 texCoord;
  mat3 TBN;
} VertexOut;

void main()
{
  gl_Position = MVPMatrix * vec4(position, 1.0);
  VertexOut.position = position;
  VertexOut.normal = normalize(normal);//normalize(normalMatrix * normal);
  VertexOut.texCoord = texCoord;

  vec3 T = normalize(tangent);
  vec3 B = normalize(cross(VertexOut.normal, T));
  VertexOut.TBN = mat3(T, B, VertexOut.normal);
}