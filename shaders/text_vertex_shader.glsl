#version 330 core

layout (location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform vec2 resolution;

void main() {
    float screen_ratio = resolution.x / resolution.y;

    vec2 screen_position = vec2(vertex.x, vertex.y) / (0.5*resolution) - vec2(1, 1);
    gl_Position = vec4(screen_position, 1.0, 1.0);
    TexCoords = vertex.zw;
}  
