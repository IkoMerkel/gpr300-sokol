#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec2 vs_texcoord;

uniform float time;
uniform sampler2D screen;
uniform float strength;

vec3 grain(vec3 fragColor)
{
  vec3 tempGrain = fragColor;
  float diff = (fract(sin(dot(vs_texcoord.xy, vec2(12.9898, 78.233))) * 43758.5453) - 0.0f) * strength;
  tempGrain.x += diff;
  tempGrain.y += diff;
  tempGrain.z += diff;
  return tempGrain;
}

void main()
{
  vec3 final_color = texture(screen, vs_texcoord).rgb;
  FragColor = vec4(grain(final_color), 1.0);
}
