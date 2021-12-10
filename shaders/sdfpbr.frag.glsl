#version 430
precision mediump float;

uniform sampler2D u_albedo_tex;
uniform sampler2D u_normal_tex;
uniform sampler2D u_mra_tex;
uniform sampler2D u_env_tex;
uniform mat4 u_projviewmatrix_inverse;
uniform vec3 u_light_pos;
uniform vec3 u_camera_pos;

// normal in world space:
in mat4 v_viewprojmatrix;
in vec3 v_normal;
in vec4 v_world;
in vec2 v_texCoord;
in vec3 v_eyepos;
in vec3 v_raydir;
in vec3 v_raypos;
in vec4 v_bounds;
in vec4 v_quat;
out vec4 outColor;

#include "common.glsl"
#include "math.glsl"
#include "hg_sdf.glsl"
#include "pbr.glsl"

float scene(vec3 p) {
	float d0 = fSphere(p, 0.5);
	float d3 = fBox(p, vec3(0.1, 0.2, 0.4));
	float d1 = fCylinder(p, 0.25, 0.5);
	//return min(d0, d1);
	return d1;
}


vec2 texcoord(vec3 p) {
	// get a texture coordinate from the scene
	// a simple cheat is cylindrical mapping of p
	vec3 pn = normalize(p);
	// atan2(y,x)/2pi gives -0.5..0.5 range
	//return vec2(atan(pn.y, pn.x) * 0.159154943091895 + 0.5, pn.z);
	return vec2(atan(pn.z, pn.x) * 0.159154943091895 + 0.5, pn.y);
}

mat3 tbn4(in vec3 p, float eps) {
	vec2 e = vec2(-eps, eps);
	// get four nearby points (tetrahedral distribution):
	vec3 p1 = p + e.yxx, p2 = p + e.xxy, p3 = p + e.xyx, p4 = p + e.yyy;
	// get distances at these points:
	float t1 = scene(p + e.yxx), t2 = scene(p + e.xxy), t3 = scene(p + e.xyx), t4 = scene(p + e.yyy);
	vec3 N = normalize(e.yxx*t1 + e.xxy*t2 + e.xyx*t3 + e.yyy*t4);
	// get texcoords at these points:
	vec2 tc1 = texcoord(p1), tc2 = texcoord(p2), tc3 = texcoord(p3), tc4 = texcoord(p4); 
	vec3 T = normalize(e.yxx*tc1.y + e.xxy*tc2.y + e.xyx*tc3.y + e.yyy*tc4.y);
	// force it to be orthogonal:
	T = normalize(T - N*dot(N,T));
	// bitangent is orthogonal to both:
	vec3 B = cross(N, T);//normalize(e.yxx*tc1.y + e.xxy*tc2.y + e.xyx*tc3.y + e.yyy*tc4.y);
	return mat3(T, B, N);
}

// compute normal from a SDF gradient by sampling 4 tetrahedral points around a location p
// (cheaper than the usual technique of sampling 6 cardinal points)
// 'scene' should be the SDF evaluator 'float distance = scene(vec3 pos)''  
// 'eps' is the distance to compare points around the location 'p' 
// a smaller eps gives sharper edges, but it should be large enough to overcome sampling error
// in theory, the gradient magnitude of an SDF should everywhere = 1, 
// but in practice this isn’t always held, so need to normalize() the result
vec3 normal4(in vec3 p, float eps) {
	vec2 e = vec2(-eps, eps);
	// tetrahedral points
	float t1 = scene(p + e.yxx), t2 = scene(p + e.xxy), t3 = scene(p + e.xyx), t4 = scene(p + e.yyy); 
	vec3 n = (e.yxx*t1 + e.xxy*t2 + e.xyx*t3 + e.yyy*t4);
	// normalize for a consistent SDF:
	//return n / (4.*eps*eps);
	// otherwise:
	return normalize(n);
}


vec4 shade(vec3 p) {
	const float EPS = 0.003;
	vec4 outColor;

	// get a texcoord from the surface
	// ideally, the sdf itself would return a texcoord
	vec2 tc = texcoord(p);

	// vec3 N = normalize(v_normal); 
	// vec3 T = normalize(dFdx(v_texCoord.y)*dFdy(v_world.xyz)-dFdx(v_world.xyz)*dFdy(v_texCoord.y));
	// vec3 B = cross(N, T);
	// mat3 TBN = mat3(T, B, N);
	//N = TBN * (texture(u_normal_tex, tc).xyz*2.-1.);

	// for normal, we approximate it by testing the scene at nearby points
	// for tangent/bitangent, we do the same, using texcoords for the surface orientation
	mat3 TBN = tbn4(p, EPS);
	// normal map:
	TBN[2] = TBN * (texture(u_normal_tex, tc).xyz*2.-1.);

	// all of these are in object-space:
	// rotate to world space
	vec3 T = quat_rotate(v_quat, TBN[0]);
	vec3 B = quat_rotate(v_quat, TBN[1]);
	vec3 N = quat_rotate(v_quat, TBN[2]);
	//mat3 TBN = mat3(T, B, N);
	
	outColor = vec4(N*0.5+0.5, 1.);
	// outColor = vec4(T*0.5+0.5, 1.);
	// outColor = vec4(B*0.5+0.5, 1.);
	// outColor = vec4( abs(dot(N, T)) ); // verify that N, T are orthogonal; should be zero
	// outColor = vec4( abs(dot(N, B)) ); // verify that N, B are orthogonal; should be zero
	// outColor = vec4( abs(dot(T, B)) ); // verify that B, T are orthogonal; should be zero
	//outColor = vec4(tc, 0., 1.);
	// demo texture for debugging:
	vec2 chk = mod(tc*2., 1.)-0.5;
	float checker = sign(chk.x*chk.y)*0.5+0.5;
	//outColor = vec4(N*checker, 1.);

	float gamma = 2.2;
	vec3 albedo = pow(texture(u_albedo_tex, tc).rgb, vec3(gamma));
	vec3 mra = texture(u_mra_tex, tc).rgb;
	float metallic = mra.x;
	float roughness = mra.y;
	float ao = mra.z;

	return vec4(pbr(albedo, normalize(N), roughness, metallic, ao, v_world.xyz, u_camera_pos, u_light_pos, u_env_tex), 1.);

	// now go ahead and do the lighting & texturing of choice
	return outColor;
}


void main() {

	vec3 rd = normalize(v_raydir);
	vec3 ro = v_raypos;
	float scale = v_bounds.w;
	vec3 worldpos = v_world.xyz;
	float dist = v_world.w; // maybe used for fog etc.

	#define STEPS 64
	#define FAR 3.0
	const float EPS = 1./float(STEPS);
	vec3 p = ro;
	float t = 0.;
	int step = 0;
	float d = 0.;
	float d0 = 0.;
	int contact = 0;

	for (; step < STEPS && t < FAR; step++) {
		p = ro + t*rd;
		d = scene(p);
		if (sign(d)*sign(d0) == -1.) { // surface crossing
			contact++;
			// render at corrected surface position:
			p = ro + (t-abs(d))*rd;
			
			worldpos += quat_rotate(v_quat, p * scale);
			break;  // break here for solid shape
		}
		d0 = d;
		// always move forward:
		t += max(EPS,abs(d));
	}

	// for deadzone:
	if (contact == 0) {
		// outColor += vec4(0.25); // show bounding box
		discard;
	} else {
		outColor = shade(p);
	}
	gl_FragDepth = computeDepth(worldpos, v_viewprojmatrix);
}