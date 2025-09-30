#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in float radius;
layout (location = 2) in vec3 color;

uniform float zoom;
uniform vec2 resolution;
uniform vec2 offset;

out vec3 vColor;

void main() {
  float screen_ratio = resolution.x/resolution.y;

  vec2 screen_position = vec2(position.x - offset.x, screen_ratio*(position.y - offset.y)) / zoom;

  gl_Position = vec4(screen_position, 0.0, 1.0);
  gl_PointSize = radius / zoom * resolution.y;

  vColor = color;
}
