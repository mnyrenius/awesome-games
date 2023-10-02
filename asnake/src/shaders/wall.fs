#version 330 core

out vec4 FragColor;
in vec4 fs_color;

// Author @patriciogv - 2015
// http://patriciogonzalezvivo.com
float random (vec2 st)
{
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

void main()
{
  vec2 st = gl_FragCoord.xy/vec2(800.0, 600.0) * 30.0;
  float rnd = max(0.2, 0.5 * random(floor(st)));
  FragColor = vec4(vec3(rnd), fs_color.w);
}