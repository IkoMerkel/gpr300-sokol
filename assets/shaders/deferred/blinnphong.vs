#version 410

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

// uniforms
uniform mat4 view_proj;
uniform mat4 model;

out vec2 vs_texcoord;
out vec3 vs_normal;

void main()
{
    vs_texcoord = in_texcoord;
    vs_normal = in_normal;
    vec4 world_position =  model * vec4(in_position, 1.0);
    gl_Position = view_proj * world_position;
}