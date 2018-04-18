#version 330 core

in varying vec2 UV;
uniform sampler2D renderedTexture;

void main()
{
  gl_FragColor = vec4(texture(renderedTexture, UV).rgb, 1.0);
}