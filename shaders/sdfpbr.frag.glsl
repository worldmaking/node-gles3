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
in vec3 v_normal;
in vec3 v_world;
in vec2 v_texCoord;

out vec4 outColor;

#include "common.glsl"
#include "pbr.glsl"

// vec3 environment(vec3 dir) {
// 	return hemisphereLight(normalize(dir), u_light_pos) + 0.001;
// }

// // following functions are copies of UE4
// // for computing cook-torrance specular lighting terms
// float D_blinn(in float roughness, in float NdH){
//     float m = roughness * roughness;
//     float m2 = m * m;
//     float n = 2.0 / m2 - 2.0;
//     return (n + 2.0) / (2.0 * PI) * pow(NdH, n);
// }

// float D_beckmann(in float roughness, in float NdH){
//     float m = roughness * roughness;
//     float m2 = m * m;
//     float NdH2 = NdH * NdH;
//     return exp((NdH2 - 1.0) / (m2 * NdH2)) / (PI * m2 * NdH2 * NdH2);
// }

// float D_GGX(in float roughness, in float NdH){
//     float m = roughness * roughness;
//     float m2 = m * m;
//     float d = (NdH * m2 - NdH) * NdH + 1.0;
//     return m2 / (PI * d * d);
// }

// float G_schlick(in float roughness, in float NdV, in float NdL){
//     float k = roughness * roughness * 0.5;
//     float V = NdV * (1.0 - k) + k;
//     float L = NdL * (1.0 - k) + k;
//     return 0.25 / (V * L);
// }

// // cook-torrance specular calculation   

// // simple phong specular calculation with normalization
// vec3 phong_specular(in vec3 V, in vec3 L, in vec3 N, in vec3 specular, in float roughness) {
//     vec3 R = reflect(-L, N);
//     float spec = max(0.0, dot(V, R));

//     float k = 1.999 / (roughness * roughness);
//     return min(1.0, 3.0 * 0.0398 * k) * pow(spec, min(10000.0, k)) * specular;
// }

// // simple blinn specular calculation with normalization
// vec3 blinn_specular(in float NdH, in vec3 specular, in float roughness) {
//     float k = 1.999 / (roughness * roughness);
//     return min(1.0, 3.0 * 0.0398 * k) * pow(NdH, min(10000.0, k)) * specular;
// }

// vec3 cooktorrance_blinn(in float NdL, in float NdV, in float NdH, in vec3 specular, in float roughness) {
//  	float D = D_blinn(roughness, NdH);
//     float G = G_schlick(roughness, NdV, NdL);
//     float rim = mix(1.0 - roughness * 0.9, 1.0, NdV);
//     return (1.0 / rim) * specular * G * D;
// }

// vec3 cooktorrance_beckmann(in float NdL, in float NdV, in float NdH, in vec3 specular, in float roughness) {
// 	float D = D_beckmann(roughness, NdH);
//     float G = G_schlick(roughness, NdV, NdL);
//     float rim = mix(1.0 - roughness * 0.9, 1.0, NdV);
//     return (1.0 / rim) * specular * G * D;
// }

// vec3 cooktorrance_ggx(in float NdL, in float NdV, in float NdH, in vec3 specular, in float roughness) {
//     float D = D_GGX(roughness, NdH);
//     float G = G_schlick(roughness, NdV, NdL);
//     float rim = mix(1.0 - roughness * 0.9, 1.0, NdV);
//     return (1.0 / rim) * specular * G * D;
// }

// ///////////////////////////////



// // float pow5(in float x) {
// //     float x2 = x * x;
// //     return x2 * x2 * x;
// // }

// // vec3 schlick(const vec3 f0, float f90, float VoH) {
// //     float f = pow5(1.0 - VoH);
// //     return f + f0 * (f90 - f);
// // }

// // vec3 schlick(vec3 f0, vec3 f90, float VoH) {
// //     return f0 + (f90 - f0) * pow5(1.0 - VoH);
// // }

// // float schlick(float f0, float f90, float VoH) {
// //     return f0 + (f90 - f0) * pow5(1.0 - VoH);
// // }


// // // https://github.com/glslify/glsl-specular-beckmann
// // float beckmann(float _NoH, float roughness) {
// //     float NoH = max(_NoH, 0.0001);
// //     float cos2Alpha = NoH * NoH;
// //     float tan2Alpha = (cos2Alpha - 1.0) / cos2Alpha;
// //     float roughness2 = roughness * roughness;
// //     float denom = 3.141592653589793 * roughness2 * cos2Alpha * cos2Alpha;
// //     return exp(tan2Alpha / roughness2) / denom;
// // }

// // float diffuseOrenNayar(vec3 L, vec3 N, vec3 V, float NoV, float NoL, float roughness) {
// //     float LoV = dot(L, V);
// //     float s = LoV - NoL * NoV;
// //     float t = mix(1.0, max(NoL, NoV), step(0.0, s));
// //     float sigma2 = roughness * roughness;
// //     float A = 1.0 + sigma2 * (1.0 / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
// //     float B = 0.45 * sigma2 / (sigma2 + 0.09);
// //     return max(0.0, NoL) * (A + B * s / t);
// // }

// // float diffuseOrenNayar(vec3 L, vec3 N, vec3 V, float roughness) {
// //     float NoV = max(dot(N, V), 0.001);
// //     float NoL = max(dot(N, L), 0.001);
// //     return diffuseOrenNayar(L, N, V, NoV, NoL, roughness);
// // }

// // float diffuseBurley(float NoV, float NoL, float LoH, float linearRoughness) {
// //     // Burley 2012, "Physically-Based Shading at Disney"
// //     float f90 = 0.5 + 2.0 * linearRoughness * LoH * LoH;
// //     float lightScatter = schlick(1.0, f90, NoL);
// //     float viewScatter  = schlick(1.0, f90, NoV);
// //     return lightScatter * viewScatter;
// // }

// // float diffuseBurley(vec3 L, vec3 N, vec3 V, float NoV, float NoL, float roughness) {
// //     float LoH = max(dot(L, normalize(L + V)), 0.001);
// //     return diffuseBurley(NoV, NoL, LoH, roughness * roughness);
// // }

// // float diffuseBurley(vec3 L, vec3 N, vec3 V, float roughness) {
// //     vec3 H = normalize(V + L);
// //     float NoV = clamp(dot(N, V), 0.001, 1.0);
// //     float NoL = clamp(dot(N, L), 0.001, 1.0);
// //     float LoH = clamp(dot(L, H), 0.001, 1.0);

// //     return diffuseBurley(NoV, NoL, LoH, roughness * roughness);
// // }


// // // https://github.com/stackgl/glsl-specular-cook-torrance
// // float specularCookTorrance(vec3 _L, vec3 _N, vec3 _V, float _NoV, float _NoL, float _roughness, float _fresnel) {
// //     float NoV = max(_NoV, 0.0);
// //     float NoL = max(_NoL, 0.0);
// //     //Half angle vector
// //     vec3 H = normalize(_L + _V);
// //     //Geometric term
// //     float NoH = max(dot(_N, H), 0.0);
// //     float VoH = max(dot(_V, H), 0.000001);
// //     float LoH = max(dot(_L, H), 0.000001);
// //     float x = 2.0 * NoH / VoH;
// //     float G = min(1.0, min(x * NoV, x * NoL));
// //     //Distribution term
// //     float D = beckmann(NoH, _roughness);
// //     //Fresnel term
// //     float F = pow(1.0 - NoV, _fresnel);
// //     //Multiply terms and done
// //     return  G * F * D / max(3.14159265 * NoV * NoL, 0.000001);
// // }

// // // https://github.com/glslify/glsl-specular-cook-torrance
// // float specularCookTorrance(vec3 L, vec3 N, vec3 V, float roughness, float fresnel) {
// //     float NoV = max(dot(N, V), 0.0);
// //     float NoL = max(dot(N, L), 0.0);
// //     return specularCookTorrance(L, N, V, NoV, NoL, roughness, fresnel);
// // }

// // float specularCookTorrance(vec3 L, vec3 N, vec3 V, float roughness) {
// //     return specularCookTorrance(L, N, V, roughness, 0.04);
// // }

// // float toShininess(float roughness, float metallic) {
// //     float s = .95 - roughness * 0.5;
// //     return s*s*s * (80. + 160. * (1.0-metallic));
// // }

// // vec3 envMap(vec3 normal, float roughness, float metallic) {
// //     return pow(environment(normal), vec3(toShininess(roughness, metallic)));
// // }

// // vec3 fresnel(const vec3 f0, float LoH) {
// //     float f90 = clamp(dot(f0, vec3(50.0 * 0.33)), 0., 1.);
// //     return schlick(f0, f90, LoH);
// // }

// // vec3 fresnel(vec3 _R, vec3 _f0, float _NoV) {
// //     return fresnel(_f0, _NoV) * environment(_R);
// // }

// // vec3 pbr0(vec3 surface_pos, vec3 light_pos, vec3 camera_pos, vec3 albedo, vec3 normal, float metallic, float roughness, float shadow, vec3 lightcolor=vec3(1.)) {
	
// // 	float gamma = 2.2;
// // 	vec3 L = normalize(light_pos - surface_pos);
// //     vec3 N = normalize(normal);
// //     vec3 V = normalize(camera_pos - surface_pos);
// // 	vec3 R = reflect(-V, N);

// // 	float notMetal = 1. - metallic;
// //     float smoothe = 0.95 - roughness;
// // 	float diffuse = diffuseBurley(L, N, V, roughness);
// // 					//diffuseOrenNayar(L, N, V, roughness);
// //     float specular = specularCookTorrance(L, N, V, roughness);

// // 	return vec3(specular);

// // 	specular *= shadow;
// //     diffuse *= shadow;

// // 	vec3 color = albedo * diffuse;

// // 	color *= environment(N); // tonemapReinhard( sphericalHarmonics(N) );

// // 	float specIntensity =   (0.04 * notMetal + 2.0 * metallic) * 
// //                             clamp(1.1 + dot(N, V) + metallic, 0., 1.) * // Fresnel
// //                             (metallic + smoothe * 4.0); // make smaller highlights brighter
// // 	vec3 ambientSpecular = envMap(R, roughness, metallic) * specIntensity;
// // 	ambientSpecular += fresnel(R, vec3(0.04), dot(N,V)) * metallic;

// // 	color = color * notMetal + (ambientSpecular + lightcolor * 2.0 * specular) * (notMetal * smoothe + color * metallic);


// // 	//return pow(color, vec3(1. / gamma));
// // }

void main() {
	float gamma = 2.2;
	vec3 albedo = pow(texture(u_albedo_tex, v_texCoord).rgb, vec3(gamma));
	vec3 mra = texture(u_mra_tex, v_texCoord).rgb;
	float metallic = mra.x;
	float roughness = mra.y;
	float ao = mra.z;

	vec3 N = normalize(v_normal); 
	vec3 T = normalize(dFdx(v_texCoord.y)*dFdy(v_world.xyz)-dFdx(v_world.xyz)*dFdy(v_texCoord.y));
	vec3 B = cross(N, T);
	mat3 TBN = mat3(T, B, N);
	vec3 normal = TBN * (texture(u_normal_tex, v_texCoord).xyz*2.-1.);

	outColor = vec4(pbr(albedo, normalize(normal), roughness, metallic, ao, v_world.xyz, u_camera_pos, u_light_pos, u_env_tex), 1.);

	// ray from surface to eye
	// vec3 V = normalize(u_camera_pos - v_world);
	// vec3 R = reflect(-V, normal);
	// // ray from surface to light
	// vec3 L = normalize(u_light_pos - v_world);
	// vec3 H = normalize(L+V);

	//outColor = vec4(normal, 1.);


 	// // mix between metal and non-metal material, for non-metal
    // // constant base specular factor of 0.04 grey is used
    // vec3 specular_f0 = mix(vec3(0.04), albedo, metallic);

	// vec3 env_diffuse = environment(normal);
	// vec3 env_specular = environment(R);

	// // N or normal?
	// float NdotL = max(0., dot(normal, L));
	// float NdotV = max(0.001, dot(normal, V));
	// float NdotH = max(0.001, dot(normal, H));
	// float VdotH = max(0.001, dot(V, H));
	// float VdotL = max(0.001, dot(V, L));

	// vec3 spec_fresnel = mix(specular_f0, vec3(1.0), pow(1.01 - NdotV, 5.0));
	// vec3 specref = phong_specular(V, L, N, spec_fresnel, roughness);

	// specref *= NdotL;

	// // diffuse is common for any model
    // vec3 diffref = (vec3(1.0) - spec_fresnel) * NdotL / PI;


	//outColor = vec4(R*0.5+0.5, 1.);
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
	// outColor = vec4(NdotL);
	// outColor = vec4(diffref, 1.);
}