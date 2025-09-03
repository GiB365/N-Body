#version 330 core

in vec3 vColor;

out vec4 FragColor;

uniform bool isLinePrimitive;

void main()
{
  if (isLinePrimitive) {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    return;
  }

  vec2 centered = gl_PointCoord * 2.0 - 1.0;

  float dist_squared = dot(centered, centered);
  
  if (dist_squared > 1.0) {
    discard;
  }

  FragColor = vec4(vColor, 1.0);
} 
