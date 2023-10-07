#version 330 core

in vec2 TexCoords;
out vec4 FragColor;
uniform float time;
uniform sampler2D bitmap;
void main()
{
  float c = texture(bitmap, TexCoords).r;
  float w = 0.0f;
  if (c < 0.1f)
    discard;

  if (TexCoords.y < time)
    c = c * (1.0f - (time - TexCoords.y));
  else
    c = c * ((1.0f + time) - TexCoords.y);

  w = 2.0f * (c - 0.7f);
  FragColor = vec4(c, w, w, 0.8f);
}