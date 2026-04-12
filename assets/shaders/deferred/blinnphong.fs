#version 410

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_material;

struct Light{
  vec3 pos;
  vec3 color;
};

uniform vec3 camera;
uniform Light light;

out vec4 FragLighting;
in vec2 vs_texcoord;

void main()
{ 
  FragLighting = vec4(light.color,1.0);
}