attribute highp vec2 vertex;
attribute mediump vec2 texcoord;

uniform highp vec2 position;
uniform highp mat4 matrix;

varying mediump vec2 v_texcoord;

void main()
{
  v_texcoord = texcoord;
  mat4 positionMatrix = mat4(1.0);
  positionMatrix[3] = vec4(position, 0.0, 1.0);
  gl_Position = matrix * positionMatrix * vec4(vertex, 0.0, 1.0);
}
