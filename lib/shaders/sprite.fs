#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec4 spriteColor;
uniform float time;
uniform int num_sprites;

void main()
{
  int idx = int(time * 20) % num_sprites;
  color = spriteColor * texture(image, vec2((TexCoords.x/num_sprites) + idx * (1.0 / num_sprites), (TexCoords.y)));
}