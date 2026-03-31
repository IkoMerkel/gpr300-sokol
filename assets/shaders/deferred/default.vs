#version 410

// attributes
layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_texcoord;


// varyings
out vec2 vs_texcoord;

void main()
{
  vs_texcoord = in_texcoord;
  gl_Position = vec4(in_position, 0.0f, 1.0);
}