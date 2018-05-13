#version 400 core

uniform mat4 LightMVP;
uniform mat4 Model;

layout(location = 0)in vec3 position;
layout(location = 1)in vec3 normal;
layout(location = 2)in vec3 tangent;
layout(location = 3)in vec2 texCoord;

void main()
{
  gl_Position = LightMVP * Model * vec4(position, 1.0);
}