#version 330 core

uniform vec2 head_pos;
out vec4 FragColor;
in vec4 fs_color;
void main()
{
  vec2 h = head_pos / vec2(800.0, 600.0);
  vec2 f = vec2(gl_FragCoord.x/800.0, (600.0 - gl_FragCoord.y)/600.0);
  float distance = length(h - f);
  vec3 c = fs_color.xyz - distance * 5.0;
  FragColor = vec4(c, fs_color.w);
}