#version 440 core

uniform mat4 projection;
uniform mat4 model;
layout(location = 0) in vec3 position;

void main() {
	gl_Position = projection * model * vec4(position, 1.0f);
}
