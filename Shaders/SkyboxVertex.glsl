#version 400 core

uniform mat4 MVPMatrix;

in vec3 vertex;
out vec3 texCoord;

void main()
{
  gl_Position = MVPMatrix * vec4(vertex, 1);
  texCoord = vertex;
}