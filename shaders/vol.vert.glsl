#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
uniform mat4 u_modelmatrix;
uniform mat4 u_modelmatrix_inverse;
uniform mat4 u_viewmatrix_inverse;
uniform mat4 u_projmatrix_inverse;
uniform float u_N;
uniform sampler3D u_tex;

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;
out vec4 v_color;
out vec3 v_normal;
out vec3 v_tc;
out vec3 v_eyepos, v_raydir, v_rayexit;
out vec3 v_viewpos;

// http://www.geeks3d.com/20141201/how-to-rotate-a-vertex-by-a-quaternion-in-glsl/
vec3 quat_rotate( vec4 q, vec3 v ){
	return v + 2.0 * cross( q.xyz, cross( q.xyz, v ) + q.w * v );
}
vec4 quat_rotate( vec4 q, vec4 v ){
	return vec4(v.xyz + 2.0 * cross( q.xyz, cross( q.xyz, v.xyz ) + q.w * v.xyz), v.w );
}

// equiv. quat_rotate(quat_conj(q), v):
// q must be a normalized quaternion
vec3 quat_unrotate(in vec4 q, in vec3 v) {
	// return quat_mul(quat_mul(quat_conj(q), vec4(v, 0)), q).xyz;
	// reduced:
	vec4 p = vec4(
		q.w*v.x - q.y*v.z + q.z*v.y,  // x
		q.w*v.y - q.z*v.x + q.x*v.z,  // y
		q.w*v.z - q.x*v.y + q.y*v.x,  // z
		q.x*v.x + q.y*v.y + q.z*v.z   // w
	);
	return vec3(
		p.w*q.x + p.x*q.w + p.y*q.z - p.z*q.y,  // x
		p.w*q.y + p.y*q.w + p.z*q.x - p.x*q.z,  // y
		p.w*q.z + p.z*q.w + p.x*q.y - p.y*q.x   // z
	);
}

vec3 eyePosFromViewMatrix(mat4 viewmatrix) {
	return -(viewmatrix[3].xyz)*mat3(viewmatrix);
}

void main() {
	// the shader expects a unit cube from 0,0,0 to 1,1,1
	// e.g. glutils.makeCube({ min:0, max:1 })
	// also expects to be rendered with cullface front, so that you can see even inside the cube

	// v_tc is unit 3D texture coordinate
	v_tc = a_position.xyz;
	
	// Multiply the position by the matrix.
	vec4 vertex = vec4(a_position, 1.);
	vec4 worldpos = u_modelmatrix * vertex;
	vec4 viewpos = u_viewmatrix * worldpos;
	gl_Position = u_projmatrix * viewpos;

	v_viewpos = viewpos.xyz;

	// derive eyepos (worldspace)
	v_eyepos = eyePosFromViewMatrix(u_viewmatrix);

	// derive ray (texture-space)
	// this assumes rendering with front-face culling:
	v_rayexit = v_tc;
	v_raydir = normalize(worldpos.xyz - v_eyepos);
	
	v_normal = a_normal;

	// v_color = vec4(v_normal*0.25+0.25, 1.);
	// v_color += vec4(a_texCoord*0.5, 0., 1.);
	v_color = vec4(v_tc, 1.);
	//v_color = vec4(texture(u_tex, tc));
}