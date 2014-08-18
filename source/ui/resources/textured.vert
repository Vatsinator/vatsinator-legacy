attribute highp vec2 vertex;
attribute mediump vec2 texcoord;

uniform highp vec2 position;
uniform lowp float rotation;
uniform lowp float z;
uniform highp mat4 matrix;

varying mediump vec2 v_texcoord;

void main()
{
  v_texcoord = texcoord;
  
  mat4 positionMatrix = mat4(1.0);
  positionMatrix[3] = vec4(position, z, 1.0);
  
  mat4 rotationMatrix = mat4(1.0);
  rotationMatrix[0] = vec4(cos(rotation), sin(rotation), 0.0, 0.0);
  rotationMatrix[1] = vec4(-sin(rotation), cos(rotation), 0.0, 0.0);
  
  gl_Position = matrix * positionMatrix * rotationMatrix * vec4(vertex, 0.0, 1.0);
}
