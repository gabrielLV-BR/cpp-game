#version 460

layout(location=0) in vec3 in_pos;

void main() {
    gl_Position = in_pos;
}