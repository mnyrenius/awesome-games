#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 projection;
uniform vec4 color;
out vec4 fs_color;

void main()
{
  gl_Position = projection * model * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
  fs_color = color;
}
