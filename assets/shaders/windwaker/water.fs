#version 410

precision mediump float;

out vec4 FragColor;


// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform float time;
uniform vec3 camera;
uniform vec3 water_color;
uniform sampler2D texture0;

void main()
{
  vec2 dir = vec2(1.0f,1.0f);
  vec2 uv = vs_texcoord + vec2(time * dir);
  uv.x += 0.01  * sin(uv.x * 3.5 + time);
  uv.y -= 0.35  * sin(uv.y * 1.5 + time);

  vec4 sampler1 = texture(texture0, uv * 1.0);
  vec4 sampler2 = texture(texture0, uv * 1.2);

  vec3 final_color = vec3((sampler1 * 0.75) - (sampler2 * 0.25)).rgb;
  FragColor = vec4(final_color + water_color, 1.0);
}