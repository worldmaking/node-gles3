#version 330

in vec3 a_position;
in vec3 a_normal;

out vec2 v_texCoord;

void main() {
	gl_Position = vec4(a_position, 1.);

	v_texCoord = a_position.xy * 0.5 + 0.5;
}