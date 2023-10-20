#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform float time;
uniform int num_sprites;
uniform bool flip_x;

void main()
{
  int idx = int(time * 20) % num_sprites;
  float x = flip_x ? (1.0 - TexCoords.x) : TexCoords.x;
  color = texture(image, vec2((x/num_sprites) + idx * (1.0 / num_sprites), (TexCoords.y)));
}