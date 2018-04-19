#version 400 core

out vec4 FragColor;

in vec3 texCoord;
uniform samplerCube skyboxTexture;

void main()
{
  FragColor = texture(skyboxTexture, texCoord);
}