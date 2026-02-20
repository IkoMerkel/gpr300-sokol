#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

uniform sampler2D screen;
uniform float strength;

void main()
{
  vec2 st = vs_texcoord - 0.5f;
  float uvA = atan(st.x, st.y);
  float uvD = dot(st,st);
  vec2 distort = 0.5f + vec2(sin(uvA), cos(uvA)) * sqrt(uvD) * (1.0 - strength * uvD);
  vec3 final_color = texture(screen, distort).rgb;
  FragColor = vec4(final_color, 1.0);
}