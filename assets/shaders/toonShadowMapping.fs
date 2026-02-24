#version 410

precision mediump float;

out vec4 FragColor;

struct Light{
  vec3 pos;
  vec3 color;
};

struct Palette{
  vec3 color1;
  vec3 color2;
};

struct Material{
  float ambient;
  float diffuse;
  float specular;
  float shiny;
};

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;
in vec4 vs_light_view_proj_pos;

uniform vec3 camera;
uniform Light light;
uniform sampler2D texture0;
uniform sampler2D shadow_map;
uniform Palette pal;
uniform Material material;

float shadowCalc(vec4 frag_pos_light_space)
{
  vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
  float closet = texture(shadow, proj_coords.xy);
  float current = proj_coords.z;
  float shadow 0.0f;
  return shadow;
}

vec3 toon(vec3 normal, vec3 frag_position, vec3 light_position) {
  vec3 view_dir = normalize(camera - frag_position);
  vec3 light_dir = normalize(light_position - frag_position);
  vec3 reflect_dir = reflect(light_dir, vs_normal);
  vec3 half_dir = normalize(light_dir + view_dir); 

  float NdotL = (dot(normal, light_dir) + 1.0f) * 0.5f;
  float NdotH = pow(max(dot(normal, half_dir), 0.0f), material.shiny);
  float PdotL = dot(vs_position, light_position.xyz);

  vec3 gradient = texture(texture0,vec2(NdotL,NdotH)).rgb;  
  vec3 light_color = mix(pal.color1,pal.color2,gradient);

  float diffuse = NdotL;
  float specular = pow(NdotH,0.5f);
  float lighting = diffuse + specular;
  return light_color;
}

void main()
{
  float shadow = shadowCalc(vs_light_view_proj_pos);
  vec3 lighting = toon(vs_normal, vs_position,light.pos);
  lighting *= (1.0f - shadow);
  vec3 object_color = vec3(1.0f);
  vec3 final_color = object_color * lighting;
  FragColor = vec4(final_color, 1.0);
}