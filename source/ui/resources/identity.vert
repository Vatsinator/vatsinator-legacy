attribute highp vec2 vertex;
uniform highp mat4 matrix;

void main(void)
{
  gl_Position = matrix * vec4(vertex, 0.0, 1.0);
}
