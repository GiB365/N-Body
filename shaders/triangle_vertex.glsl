#version 330 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

uniform vec2 resolution;

out vec3 vColor;

void main() {
  float screen_ratio = resolution.x / resolution.y;

  vec2 screen_position = vec2(position.x, position.y) / 50 - vec2(1, 1);

  gl_Position = vec4(screen_position, 1.0, 1.0);

  vColor = color;
}


