#version 150

precision highp float;

in  vec3 ex_Color;
in  vec2 ex_TexCoord;
out vec4 out_Color;

uniform sampler2D left;  //Linkes Bild
uniform sampler2D right; //Rechtes Bild

void main(void)
{
  //Horizontal Interlaced
  if (mod(trunc(gl_FragCoord.y), 2.0) < 0.5)
    out_Color = texture2D(left, vec2(ex_TexCoord));
  else
   discard;
   // out_Color = texture2D(right, vec2(ex_TexCoord));
}
