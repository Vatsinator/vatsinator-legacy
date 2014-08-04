uniform highp mat4 matrix;

attribute highp vec2 vertex;
attribute mediump vec2 texcoord;
attribute highp vec2 position;

varying lowp vec2 v_texcoord;

void main()
{
  gl_Position = matrix /** vec4(position, 0.0, 1.0)*/ * vec4(vertex, 10.0, 1.0);

  v_texcoord = texcoord;
}
