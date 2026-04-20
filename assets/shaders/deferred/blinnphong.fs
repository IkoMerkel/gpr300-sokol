#version 410

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_material;

struct Light{
  vec3 pos;
  vec3 color;
  float radius;
  float attenuation;
};

uniform vec3 camera;
uniform Light light;

out vec4 FragLighting;
in vec2 vs_texcoord;
in vec3 vs_normal;

float attenuateLinear(float distance, float radius)
{
  //return clamp((radius-distance)/radius,0.0,1.0);

  float i = clamp(1.0 - pow(distance/radius,4.0),0.0,1.0);
  return i*i;
}

vec3 blinnphong(vec3 normal, vec3 frag_position, vec3 light_position, vec4 material) {
  vec3 view_dir = normalize(camera - frag_position);
  vec3 diff = light_position - frag_position;
  vec3 light_dir = normalize(light_position - frag_position);
  vec3 reflect_dir = reflect(light_dir, normal);
  vec3 half_dir = normalize(light_dir + view_dir); 

  float NdotL = max(dot(normal, light_dir), 0.0f);
  float NdotH = max(dot(normal, half_dir), 0.0f);

  vec3 diffuse = NdotL * vec3(material.g);
  vec3 specular = pow(NdotH, material.a * 128) * vec3(material.b);
  vec3 lighting = diffuse + specular + vec3(material.r);
  return (lighting * (light.color * attenuateLinear(length(diff),light.radius)));
  //vec3(material.r);
}

void main()
{ 
  vec2 UV = gl_FragCoord.xy / vec2(800,600);
  vec3 pos = texture(g_position, UV).rgb;
  vec3 norm = texture(g_normal,  UV).rgb;
  vec3 alb = texture(g_albedo, UV).rgb;
  vec4 material = texture(g_material,  UV).rgba;
  vec3 finalLightColor = blinnphong(norm,pos,light.pos, material);
  FragLighting = vec4(finalLightColor * alb,1.0);
}