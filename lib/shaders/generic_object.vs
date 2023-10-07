#version 330 core
layout (location = 0) in vec3 vertex;

uniform mat4 model;
uniform mat4 projection;
uniform vec4 color;
out vec4 fs_color;
out vec2 fs_pos;

void main()
{
  gl_Position = projection * model * vec4(vertex.xyz, 1.0f);
  fs_color = color;
  fs_pos = vertex.xy;
}
