#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

uniform sampler2D screen;
uniform vec3 offset;

void main()
{
  vec3 final_color;
  final_color.x = texture(screen, vs_texcoord + offset.x).r;
  final_color.y = texture(screen, vs_texcoord + offset.y).g;
  final_color.z = texture(screen, vs_texcoord + offset.z).b;
  FragColor = vec4(final_color, 1.0);
}