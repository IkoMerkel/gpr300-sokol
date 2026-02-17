#version 410

precision mediump float;

out vec4 FragColor;


// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

float scale = 1.0f;

uniform float time;
uniform vec3 camera_pos;
uniform vec3 water_color;
uniform sampler2D wave_tex;
uniform sampler2D wave_warp;
uniform sampler2D wave_spec;

void main()
{
  vec2 dir = vec2(1.0f,1.0f);
  vec2 uv = vs_texcoord + vec2(time * dir);
  uv.x += 0.01  * sin(uv.x * 3.5 + time);
  uv.y -= 0.35  * sin(uv.y * 1.5 + time);

  vec4 sampler1 = texture(wave_tex, uv * 1.0);
  vec4 sampler2 = texture(wave_tex, uv * 1.2);

  vec2 warp_uv = vs_texcoord * scale;
  vec2 warp_scroll = vec2(0.5,0.5) * time * 0.2;
  vec2 warp = texture(wave_warp, warp_uv + warp_scroll).xy;

  vec2 albedo_uv = vs_texcoord * scale;
  vec4 albedo = texture(wave_tex, albedo_uv + warp).rgba;
  vec3 final_color = water_color + vec3(albedo.a);


  vec2 spec_uv = vs_texcoord * 0.5;
  vec2 spec_scroll = vec2(0.5,0.5) * time * 0.2f;
  //vec3 spec = texture(wave_spec, spec_uv + spec_scroll).rgb;

  vec3 spec_smp1 = texture(wave_spec, spec_uv + vec2(0.5,0.5) * time * 0.2).rgb;
  vec3 spec_smp2 = texture(wave_spec, spec_uv + vec2(-0.5,0.5) * time * 0.2).rgb;
  vec3 spec = spec_smp1 + spec_smp2;

  float fresnel = dot(normalize(camera_pos), vec3(0.0,1.0,0.0));


  const vec3 bright = vec3(0.299, 0.587,0.114);
  float k_bright =  dot(spec, bright);
  if(k_bright <= 0.5 || k_bright >= 0.95)
  {
    final_color = mix(final_color, final_color + spec,  fresnel);
  }

  FragColor = vec4(final_color, 1.0);
}