#version 330
uniform mat4 u_modelmatrix;
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;

// normal in world space:
out vec3 v_normal;
out vec3 v_world;
out vec2 v_texCoord;

void main() {
	vec4 vertex = vec4(a_position, 1.);
	vec4 world = u_modelmatrix * vertex;
	vec4 view = u_viewmatrix * world;
	gl_Position = u_projmatrix * view;

	v_world = world.xyz;
	v_texCoord = a_texCoord;
	// normal in world space:
	v_normal = mat3(u_modelmatrix) * a_normal.xyz;
}