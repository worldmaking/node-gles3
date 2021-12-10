#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;

// instanced variable:
in vec3 i_pos;     
in vec4 i_bounds;  // xyz is bounding box, w is scale factor
in vec4 i_quat;    

// normal in world space:
out mat4 v_viewprojmatrix;
out vec3 v_normal;
out vec4 v_world;
out vec2 v_texCoord;
out vec3 v_raydir;
out vec3 v_eyepos;
out vec3 v_raypos;
out vec4 v_bounds;
out vec4 v_quat;

#include "math.glsl"

void main() {
	vec4 vertex = vec4(a_position, 1.);

	// apply instance transform:
	vertex.xyz *= i_bounds.xyz;
	vertex.xyz *= i_bounds.w;
	vertex = quat_rotate(i_quat, vertex);
	vertex.xyz += i_pos.xyz;

	v_viewprojmatrix = u_projmatrix * u_viewmatrix;

	vec4 world = vertex;
	vec4 view = u_viewmatrix * world;
	gl_Position = u_projmatrix * view;

	// derive eyepos (worldspace)
	v_eyepos = -(u_viewmatrix[3].xyz)*mat3(u_viewmatrix);
	// derive ray (object space)
	v_raypos = a_position.xyz * i_bounds.xyz;
	v_raydir = (quat_unrotate(i_quat, world.xyz - v_eyepos));

	// if we needed precision, refine this in frag shader based on the surface function
	v_world = vec4(i_pos.xyz, length(view.xyz));
	v_quat = i_quat;
	
	v_texCoord = a_texCoord;
	// normal in world space:
	//v_normal = mat3(u_modelmatrix) * a_normal.xyz;
	v_normal = quat_rotate(i_quat, a_normal);
}