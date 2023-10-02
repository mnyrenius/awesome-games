#version 330 core

out vec4 FragColor;
in vec4 fs_color;
in vec2 fs_pos;
void main()
{
  float l = length(fs_pos - vec2(0.5, 0.5));
  if (l < 0.5)
  {
    FragColor = vec4(fs_color.xyz * (0.8 - l), fs_color.w);
  }
  else
  {
    discard;
  }
}