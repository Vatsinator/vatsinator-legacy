uniform sampler2D texture;

varying lowp vec2 v_texcoord;

void main()
{
//   gl_FragColor = texture2D(texture, v_texcoord);
  gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
