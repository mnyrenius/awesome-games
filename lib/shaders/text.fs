#version 330 core

in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D bitmap;
void main()
{
  float c = texture(bitmap, TexCoords).r;
  if (c < 0.1f)
    discard;

  FragColor = vec4(c, c, c, 0.8f);
}