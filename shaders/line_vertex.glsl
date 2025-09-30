#version 330 core

layout (location = 0) in vec2 in_point1;
layout (location = 1) in vec2 in_point2;
layout (location = 2) in float in_thickness;
layout (location = 3) in vec3 in_color;

uniform float zoom;
uniform vec2 resolution;
uniform vec2 offset;

out VS_OUT {
  vec2 point1;
  vec2 point2;
  float thickness;
  vec3 color;
  float screen_ratio;
} vs_out;

void main() {
  float screen_ratio = resolution.x / resolution.y;

  vs_out.point1 = (in_point1 - offset) / zoom;
  vs_out.point2 = (in_point2 - offset) / zoom;
  vs_out.thickness = in_thickness / (zoom*screen_ratio);
  vs_out.screen_ratio = screen_ratio;
  vs_out.color = in_color;
}


