#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

uniform sampler2D screen;
uniform float intensity;
uniform float extent;

void main()
{
  vec3 final_color = texture(screen, vs_texcoord).rgb;
  vec2 uv = vs_texcoord * (1.0 - vs_texcoord.yx);
  float vig = uv.x*uv.y * intensity;
  vig = pow(vig, extent);
  FragColor = vec4(final_color, 1.0) * vec4(vig);
}