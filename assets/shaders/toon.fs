#version 410

precision mediump float;

out vec4 FragColor;

struct Light{
  vec3 pos;
  vec3 color;
};

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform vec3 camera;
uniform Material material;
uniform Light light;
//uniform texture texture0;

vec3 toon(vec3 normal, vec3 frag_position, vec3 light_position) {
  vec3 view_dir = normalize(camera - frag_position);
  vec3 light_dir = normalize(light_position - frag_position);
  vec3 reflect_dir = reflect(light_dir, vs_normal);
  vec3 half_dir = normalize(light_dir + view_dir); 

  float NdotL = max(dot(normal, light_dir), 0.0f);
  float NdotH = max(dot(normal, half_dir), 0.0f);
  float PdotL = dot(vs_position, light_position.xyz);

  float diffuse = NdotL * material.diffuse;
  float specular = pow(NdotH, material.shiny) * material.specular;
  float lighting = diffuse + specular;
  return (lighting * light.color) + vec3(material.ambient);
}

void main()
{
  vec3 lighting = toon(vs_normal, vs_position,light.pos);
  vec3 object_color = vs_normal.rgb * 0.5f + 0.5f;
  //object_color = texture stuff (for when we build out the architecture for texturing)
  vec3 final_color = object_color * lighting;
  FragColor = vec4(final_color, 1.0);
}