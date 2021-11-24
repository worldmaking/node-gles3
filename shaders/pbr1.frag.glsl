#version 330
precision mediump float;

uniform sampler2D u_albedo_tex;
uniform sampler2D u_normal_tex;
uniform sampler2D u_mra_tex;
uniform mat4 u_projviewmatrix_inverse;
uniform vec3 u_light_pos;
uniform vec3 u_camera_pos;

// normal in world space:
in vec3 v_normal;
in vec3 v_world;
in vec2 v_texCoord;

out vec4 outColor;

#include "common.glsl"

vec3 environment(vec3 dir) {
	return hemisphereLight(normalize(dir), u_light_pos);
}

void main() {
	

	vec3 N = normalize(v_normal); 
	vec3 T = normalize(dFdx(v_texCoord.y)*dFdy(v_world.xyz)-dFdx(v_world.xyz)*dFdy(v_texCoord.y));
	vec3 B = cross(N, T);
	mat3 TBN = mat3(T, B, N);
	vec3 normal = TBN * (texture(u_normal_tex, v_texCoord).xyz*2.-1.);
	// ray from surface to eye
	vec3 V = normalize(u_camera_pos - v_world);
	vec3 R = reflect(-V, normal);
	// ray from surface to light
	vec3 L = normalize(u_light_pos - v_world);
	vec3 H = normalize(L+V);

	float gamma = 2.2;
	vec3 albedo = pow(texture(u_albedo_tex, v_texCoord).rgb, vec3(gamma));
	vec3 mra = texture(u_mra_tex, v_texCoord).rgb;
	float metallic = mra.x;
	float roughness = mra.y;
	float ao = mra.z;

 	// mix between metal and non-metal material, for non-metal
    // constant base specular factor of 0.04 grey is used
    vec3 specular = mix(vec3(0.04), albedo, metallic);


	outColor = vec4(R*0.5+0.5, 1.);
	//outColor = vec4(T*0.5+0.5, 1.);
	//outColor = mix( outColor, vec4(v_texCoord, 0., 1.), 0.2);
	//outColor = vec4(albedo, 1.);
	// // outColor = vec4(normal, 1.);
	// // //outColor = vec4(mra, 1.);
	// // // outColor = vec4(metallic);
	// // // outColor = vec4(roughness);
	// // //outColor = vec4(ao);
	// //outColor = vec4(normal, 1.);

	// outColor = vec4(v_texCoord, 0., 1.);

	//vec3 ambient = equirectangular(irradianceMap, 
	//	gl_TextureMatrix[5], 
	///	normal, 
	//	roughness * 8.).rgb;
	outColor = vec4(environment(normal), 1.);
}