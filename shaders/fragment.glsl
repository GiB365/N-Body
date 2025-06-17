#version 330 core
out vec4 FragColor;


void main()
{
  vec2 centered = gl_PointCoord * 2.0 - 1.0;

  float dist_squared = dot(centered, centered);
  
  if (dist_squared > 1.0) {
    discard;
  }


  FragColor = vec4(0.9f, 0.5f, 0.2f, 1.0f);
} 
