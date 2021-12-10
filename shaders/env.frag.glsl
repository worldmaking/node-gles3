#version 330
precision mediump float;

// uniform sampler2D u_albedo_tex;
uniform vec3 u_light_pos;

in vec2 v_uv;
out vec4 outColor;

#include "common.glsl"
#include "math.glsl"


vec3 getSky(in vec3 rd)
{
	const vec3 sunColour = vec3(1., 1., .6);
    float horizon = pow(1.0-max(rd.y, 0.0), 2.2)*.06;
	vec3  sky = vec3(.0, .0, .07);
	sky = mix(sky, vec3(sunColour), horizon);
	return min(sky, 1.0);
}


vec3 fog(vec3 rd) {

	vec3 r3 = random3(rd);
	vec3 skyblue = vec3(.0, .0, .07);
	vec3 lightpos = vec3(0, 3.9, 0);
	vec3 ldir = normalize(lightpos);
	float ldotr = max(dot(rd, ldir), 0.);
	float sun = (pow(ldotr, 10.) + pow(ldotr, 1000.));
	float horizon = pow(max(rd.y, 0.), 0.03);
	float horizon2 = pow(1.0-max(rd.y, 0.0), 2.2)*.06;
	float atmos = pow(max(rd.y+0.5, 0.), 1.2);
	float haze =  (dot(rd, ldir + vec3(0, 1, 0))*0.25+0.5);

	return atmos * 0.1 + 
	 mix((atmos * haze) * skyblue, (sun * horizon) * vec3(0.7), 0.7) * vec3(0.8)
	 + r3*0.03;
	// //return vec3(horizon2);
	// return vec3(haze);
	return vec3(atmos);
	return r3;
}

void main() {
	outColor = vec4(v_uv, 0., 1.);
	vec2 muv = abs(mod(v_uv*2 + 0.5, 2.)-1.);
	outColor = vec4(muv, 0., 1.);

	vec3 R = equirectangularRay(v_uv);
	//vec2 uv2 = equirectangular(R);
	outColor = vec4(R, 1.);
	//outColor = vec4(mod(R * 8., 1.), 1.);
	// outColor = vec4(R, 1.);
	outColor = vec4(hemisphereLight(R, u_light_pos), 1.);
	// vec4 color = texture(u_albedo_tex, v_uv);
	// float r = length(v_uv - 0.5);
	// float lod = 5.0*abs(  mod( u_time - 2.*r, 2.) - 1. );
	// color = texture(u_albedo_tex, v_uv, lod);
	// outColor = color;
}