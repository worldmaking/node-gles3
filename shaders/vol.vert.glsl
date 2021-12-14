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

out vec3 v_ro, v_rd, v_re;

out vec3 v_debug;

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
	
	// exit point of the cube, in model space (and texture space)
	vec4 vertex = vec4(a_position, 1.);
	// this is the exit-point of the cube, in world space
	vec4 worldpos = u_modelmatrix * vertex;
	// exit point in camera-dependent view-space
	vec4 viewpos = u_viewmatrix * worldpos;
	gl_Position = u_projmatrix * viewpos;

	// clamp this at the near:
	//viewpos.z = min(viewpos.z, 0.);

	// we know where eyepos is
	// make it relative to cube-space (inv modelmatrix)
	// that tells you which face

	// derive eyepos (worldspace)
	v_eyepos = eyePosFromViewMatrix(u_viewmatrix);
	v_viewpos = viewpos.xyz;
	v_raydir = normalize(worldpos.xyz - v_eyepos);

	// // in model space:
	//vec3 rd = normalize(mat3(u_modelmatrix_inverse) * v_raydir);
	vec3 eye_tc = (u_modelmatrix_inverse * vec4(v_eyepos, 1.)).xyz;
	// clamp to box:
	eye_tc = clamp(eye_tc, 0., 1.);
	vec3 rd_tc = normalize(v_tc - eye_tc);

	v_rd = rd_tc;
	v_ro = eye_tc;
	v_re = v_tc;


	// // solve ray p = ro + t*rd for exit "t"
	// // t = (p-ro)/rd
	// // where p depends on ray direction
	// // e.g., if rd.x > 0, then exit point is 1, if rd.x < 0, then exit point is 0
	// // step(0, a_position) gives us that
	// vec3 p = step(0., a_position.xyz);
	// vec3 t3 = ((p-a_position.xyz)/rd);
	// // stop at first plane:
	// float t = max(max(t3.x, t3.y), t3.z);

	// // the two intersection sets for the 6 bounding planes:
	// vec3 t0 = (0.-a_position)/rd;
	// vec3 t1 = (1.-a_position)/rd;

	// float t0m = min(t0.x, min(t0.y, t0.z));
	// float t1m = min(t1.x, min(t1.y, t1.z));
	// vec3 t2 = max(t0, t1);
	// t = min(t2.x, min(t2.y, t2.z));

	v_debug = eye_tc;

	// // derive ray (texture-space)
	// this assumes rendering with front-face culling:
	v_rayexit = v_tc;
	
	v_normal = a_normal;

	// v_color = vec4(v_normal*0.25+0.25, 1.);
	// v_color += vec4(a_texCoord*0.5, 0., 1.);
	v_color = vec4(rd_tc, 1.);
	//v_color = vec4(texture(u_tex, tc));
}