#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in float radius;
layout (location = 2) in vec3 color;

uniform vec2 offset;
uniform vec2 resolution;
uniform float zoom;
uniform bool isLinePrimitive;

out vec3 vColor;

void main() {
  float display_ratio = resolution.y/resolution.x;

  vec2 screen_space = vec2((position.x - offset.x) / zoom, ((position.y - offset.y) / zoom) * display_ratio);
  
  gl_Position = vec4(screen_space, 0.0, 1.0);

  if (!isLinePrimitive) {
    gl_PointSize = radius * resolution.x / zoom;
  }

  vColor = color;
}
