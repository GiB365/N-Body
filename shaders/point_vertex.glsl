#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in float radius;
layout (location = 2) in vec3 color;

out vec3 vColor;

void main() {
  gl_Position = vec4(0, 0, 0.0, 1.0);
  gl_PointSize = 10;

  vColor = color;
}
