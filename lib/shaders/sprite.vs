#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texpos;

out vec2 TexCoords;

uniform mat4 projection;

void main()
{
  TexCoords = texpos;
  gl_Position = projection * vec4(position, 0.0, 1.0);
}