#version 330 core
layout(origin_upper_left) in vec4 gl_FragCoord;

uniform vec2 head_pos;
uniform vec2 screen_res;
out vec4 FragColor;
in vec4 fs_color;
void main()
{
  vec2 h = head_pos / screen_res;
  vec2 f = gl_FragCoord.xy/screen_res;
  float distance = length(h - f);
  vec2 c = fs_color.xy - distance * 4.0;
  FragColor = vec4(c.x, 1.0, c.y, fs_color.w);
}