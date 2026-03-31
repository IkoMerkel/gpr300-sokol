#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

uniform sampler2D screen;

void main()
{
  vec3 final_color = texture(screen, vs_texcoord).rgb;
  FragColor = vec4(final_color, 1.0);
}