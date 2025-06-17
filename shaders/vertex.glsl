#version 330 core

layout (location = 0) in vec3 transform;

void main() {
  gl_Position = vec4(transform.x, transform.y, 0.0, 1.0);
  gl_PointSize = transform.z;
}
