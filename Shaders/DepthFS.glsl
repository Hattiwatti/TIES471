#version 400 core

out float fragmentDepth;

void main()
{
  fragmentDepth = gl_FragCoord.z;
}