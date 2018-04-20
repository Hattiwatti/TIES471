#version 400 core

layout(std140) uniform ViewBlock
{
  mat4 ModelViewProj;
  vec3 CameraPos;
};

in vec3 vertex;
out vec3 texCoord;

void main()
{
  gl_Position = ModelViewProj * vec4(vertex, 1);
  texCoord = vertex;
}