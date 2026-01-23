#version 410

precision mediump float;

out vec4 FragColor;

struct Light{
  vec3 pos;
  vec3 color;
};

struct Material{
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shiny;
};

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform vec3 camera;
uniform float alpha;
uniform Material material;
uniform Light light;

vec3 blinnphong(vec3 normal, vec3 frag_position, vec3 light_position) {
  vec3 view_dir = normalize(camera - frag_position);
  vec3 light_dir = normalize(light.pos - frag_position);
  vec3 reflect_dir = reflect(light_dir, vs_normal);
  vec3 half_dir = normalize(light_dir + view_dir); 

  float NdotL = max(dot(normal, light_dir), 0.0f);
  float NdotH = max(dot(normal, half_dir), 0.0f);
  float PdotL = dot(vs_position, light.pos.xyz);

  vec3 diffuse = NdotL * material.diffuse;
  vec3 specular = NdotH * material.specular;
  vec3 lighting = vec3(diffuse) + vec3(pow(specular,alpha));
  return lighting * light.color;
}

void main()
{
  vec3 lighting = blinnphong(vs_normal, vs_position,light.pos);
  vec3 object_color = vs_normal.rgb * 0.5f + 0.5f;
  vec3 ambient = vec3(0.2f);
  vec3 final_color = object_color * light.color + ambient;
  FragColor = vec4(final_color, 1.0);
}