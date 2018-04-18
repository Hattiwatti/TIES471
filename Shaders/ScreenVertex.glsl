#version 450 core

in vec3 vertex;
out vec2 texCoord;

void main()
{
  gl_Position = vec4(vertex, 1);
  texCoord = (vertex.xy + vec2(1, 1)) / 2.0;
}