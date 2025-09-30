#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in VS_OUT {
  vec2 point1;
  vec2 point2;
  float thickness;
  vec3 color;
  float screen_ratio;
} gs_in[];

out vec3 fragColor;

void main() {
  vec2 p1 = gs_in[0].point1;
  vec2 p2 = gs_in[0].point2;
  float t = gs_in[0].thickness;
  float sr = gs_in[0].screen_ratio;
  fragColor = gs_in[0].color;

  vec2 dir = normalize(p2 - p1);
  vec2 normal = vec2(-dir.y, dir.x) * t * 0.5;

  p1 -= dir * t * 0.5;
  p2 += dir * t * 0.5;

  vec4 a = vec4(p1 + normal, 0.0, 1.0);
  vec4 b = vec4(p1 - normal, 0.0, 1.0);
  vec4 c = vec4(p2 + normal, 0.0, 1.0);
  vec4 d = vec4(p2 - normal, 0.0, 1.0);

  a.y *= sr;
  b.y *= sr;
  c.y *= sr;
  d.y *= sr;

  gl_Position = a; EmitVertex();
  gl_Position = b; EmitVertex();
  gl_Position = c; EmitVertex();
  gl_Position = d; EmitVertex();
  EndPrimitive();
}
