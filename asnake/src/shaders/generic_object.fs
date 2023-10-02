#version 330 core

out vec4 FragColor;
in vec4 fs_color;
void main()
{
  FragColor = fs_color;
}