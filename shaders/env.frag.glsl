#version 330
precision mediump float;

// uniform sampler2D u_albedo_tex;
uniform vec3 u_light_pos;

in vec2 v_uv;
out vec4 outColor;

#include "common.glsl"



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