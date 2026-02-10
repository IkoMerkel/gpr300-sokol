#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

uniform sampler2D screen;

void main()
{
  vec3 final_color = texture(screen, vs_texcoord).rgb;

  //float gray = (final_color.r + final_color.g + final_color.b)/3;
  float gray = 0.2126 * final_color.r + 0.7152 * final_color.g + 0.0722 * final_color.b;
  FragColor = vec4(vec3(gray), 1.0);
}