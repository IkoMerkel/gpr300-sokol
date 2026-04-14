#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

uniform sampler2D blinnPhong;
uniform sampler2D screen;

void main()
{
  vec3 final_color = texture(screen, vs_texcoord).rgb;
  vec3 lighting = texture(blinnPhong, vs_texcoord).rgb;
  FragColor = vec4(final_color * lighting, 1.0);
}