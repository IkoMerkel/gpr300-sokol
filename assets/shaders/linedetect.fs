#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

uniform sampler2D screen;

void main()
{
  vec3 final_color = texture(screen, vs_texcoord).rgb;
  float luminance = dot(final_color,vec3(0.2126, 0.7152,0.0622));
  float gradient = fwidth(luminance);
  float isEdge;
  if(gradient > 0.2f)
  {
    isEdge = 1.0f;
  }
  else
  {
    isEdge = 0.0f;
  }
  FragColor = vec4(vec3(isEdge), 1.0);
}