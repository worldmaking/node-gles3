#version 430
precision mediump float;

uniform mat4 u_projviewmatrix_inverse;
uniform float u_time;
uniform vec3 u_light_pos;
uniform sampler2D u_env_tex;

in vec2 v_texCoord;

out vec4 outColor;

#include "common.glsl"

void main() {

	// ray direction of background:
	vec3 rd = normalize((u_projviewmatrix_inverse * vec4(v_texCoord*2.-1., 1., 1.)).xyz);
	vec2 uv = equirectangular(rd);	
	outColor = vec4(v_texCoord, 0, 1);
	outColor = vec4(rd, 1.);
	outColor = vec4(uv, 0, 1);
	// int levels = textureQueryLevels(u_env_tex);
	outColor = texture(u_env_tex, uv);

	outColor = vec4(hemisphereLight(rd, u_light_pos), 1.);
	
	
	
	//outColor = texture(u_env_tex, v_texCoord);
}