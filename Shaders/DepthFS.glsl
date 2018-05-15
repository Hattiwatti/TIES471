#version 400 core

out float fragmentDepth;

// This shader is for generating shadowmaps (obsolete)

void main()
{
  fragmentDepth = gl_FragCoord.z;
}