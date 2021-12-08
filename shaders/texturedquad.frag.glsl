#version 330
precision mediump float;

uniform sampler2D u_albedo_tex;
uniform float u_time;

in vec2 v_uv;
out vec4 outColor;

void main() {
	outColor = vec4(v_uv, 0., 1.);
	vec4 color = texture(u_albedo_tex, v_uv);
	float r = length(v_uv - 0.5);
	float lod = 5.0*abs(  mod( u_time - 2.*r, 2.) - 1. );
	color = texture(u_albedo_tex, v_uv, lod);
	outColor = color;
}