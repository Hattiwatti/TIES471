out varying vec2 UV;

void main()
{
  gl_Position = gl_Vertex;
  UV = (gl_Vertex.xy + vec2(1, 1)) / 2.0;
}