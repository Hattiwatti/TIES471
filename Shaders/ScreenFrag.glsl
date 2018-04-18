#version 450 core

layout (location = 0) out vec4 FragColor;

in vec2 texCoord;

layout(binding = 0)uniform sampler2D texture_position;
layout(binding = 1)uniform sampler2D texture_normal;
layout(binding = 2)uniform sampler2D texture_albedoMetal;
layout(binding = 3)uniform sampler2D texture_roughness;

void main()
{
  FragColor = vec4(texture(texture_normal, texCoord).rgb, 1.0);
  //FragColor = vec4(1,1,1,1);
}