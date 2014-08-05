attribute highp vec2 vertex;

uniform highp mat4 matrix;
uniform lowp float offset;

void main(void)
{
  mat4 offsetMatrix = mat4(1.0);
  offsetMatrix[3] = vec4(offset, 0.0, 0.0, 1.0);
  gl_Position = matrix * offsetMatrix * vec4(vertex, 0.0, 1.0);
}
