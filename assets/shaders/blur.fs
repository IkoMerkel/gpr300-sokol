#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

uniform sampler2D screen;
uniform float strength = 16.0f;

const float offset = 1.0/300.0;
const vec2 offsets[9] = vec2[](
  vec2(-offset,offset), //top left
  vec2(0.0, offset),
  vec2(offset, offset),

  vec2(-offset,0.0), //middle left
  vec2(0.0,0.0),
  vec2(offset,0.0),

  vec2(-offset,-offset), //bottom left
  vec2(0.0,-offset),
  vec2(offset,-offset)
);

const float kernal[9] = float[](
  1.0,2.0,1.0,
  2.0,4.0,2.0,
  1.0,2.0,1.0
);

void main()
{
  //vec3 final_color = texture(screen, vs_texcoord).rgb;
  vec3 final_color = vec3(0.0,0.0,0.0);

  for(int i = 0; i < 9; i++){
    vec3 local = vec3(texture(screen,vs_texcoord.xy + offsets[i]));
    final_color += local * (kernal[i]/strength); 
  }

  //float gray = 0.2126 * final_color.r + 0.7152 * final_color.g + 0.0722 * final_color.b;
  
  FragColor = vec4(final_color, 1.0);
}