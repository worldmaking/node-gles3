#version 330
precision mediump float;

uniform mat4 u_projviewmatrix_inverse;
uniform float u_time;
uniform vec3 u_light_pos;

in vec2 v_texCoord;

out vec4 outColor;

#include "common.glsl"


void main() {

	// ray direction of background:
	vec3 rd = normalize((u_projviewmatrix_inverse * vec4(v_texCoord, 1., 1.)).xyz);
	// rd could now be used to look up in an environment map
	// e.g. spheretex, equirectangular tex, etc., or a function
	outColor = vec4(hemisphereLight(rd, u_light_pos), 1.);
}