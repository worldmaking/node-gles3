
// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(vec3 F0, float cosTheta) {
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float ndfGGX(float cosLh, float roughness) {
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;
	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}


// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k){
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness){
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// albedo is the base colour from material
// N is the normalized normal (world space), including material map
// roughness, metalness, shadow (==ao) also from material
// surface_pos, eye_pos, light_pos are in world space
// env_tex is an equirectangular map
vec3 pbr(vec3 albedo, vec3 N, float roughness, float metalness, float shadow, vec3 surface_pos, vec3 eye_pos, vec3 light_pos, sampler2D env_tex) {
	int levels = textureQueryLevels(env_tex);

	vec3 V = normalize(eye_pos - surface_pos);
	// Angle between surface normal and outgoing light direction.
	float NdotV = max(0.0, dot(N, V));
	// Specular reflection vector.
	vec3 R = 2.0 * NdotV * N - V;
	// Fresnel reflectance at normal incidence (for metals use albedo color).
	vec3 F0 = mix(vec3(0.04), albedo, metalness);
	// Direct lighting calculation for analytical lights.
	vec3 directLighting = vec3(0);
	
	float radiance = 1.;
	const float Epsilon = 0.00001;
	vec3 L = normalize(light_pos - surface_pos);
	// half-vector between ray from light and ray to eye
	vec3 H = normalize(V+L);
	// Calculate angles between surface normal and various light vectors.
	float NdotL = max(0.0, dot(N, L));
	float NdotH = max(0.0, dot(N, H));
	// Calculate Fresnel term for direct lighting. 
	vec3 F  = fresnelSchlick(F0, max(0.0, dot(H, V)));
	// Calculate normal distribution for specular BRDF.
	float D = ndfGGX(NdotH, roughness);
	// Calculate geometric attenuation for specular BRDF.
	float G = gaSchlickGGX(NdotL, NdotV, roughness);
	// Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
	// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
	// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
	vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metalness);
	// Lambert diffuse BRDF.
	// We don't scale by 1/PI for lighting & material units to be more convenient.
	// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
	vec3 diffuseBRDF = kd * albedo;
	// Cook-Torrance specular microfacet BRDF.
	vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * NdotL * NdotV);
	vec3 direct = (diffuseBRDF + specularBRDF) * radiance * NdotL;
	
	vec3 ambientLighting;
	// Sample diffuse irradiance at normal direction.
	//vec3 irradiance = texture(env_tex, equirectangular(N), roughness * levels).rgb;
	vec3 irradiance = texture(env_tex, equirectangular(N)).rgb;
	// Calculate Fresnel term for ambient lighting.
	// Since we use pre-filtered cubemap(s) and irradiance is coming from many directions
	// use NdotV instead of angle with light's half-vector (LdotH).
	// See: https://seblagarde.wordpress.com/2011/08/17/hello-world/
	vec3 Fa = fresnelSchlick(F0, NdotV);
	// Get diffuse contribution factor (as with direct lighting).
	vec3 kda = mix(vec3(1.0) - Fa, vec3(0.0), metalness);
	// Irradiance map contains exitant radiance assuming Lambertian BRDF, no need to scale by 1/PI here either.
	vec3 diffuse = shadow * kda * albedo * irradiance;
	// Sample pre-filtered specular reflection environment at correct mipmap level.
	vec3 specularIrradiance = texture(env_tex, equirectangular(R), roughness * levels).rgb;
	float specIntensity =   (0.04 * (1.-metalness) + 2.0 * metalness) * 
                            clamp(1.1 + NdotV + metalness, 0., 1.) * // Fresnel
                            (metalness + (0.95-roughness) * 4.0); // make smaller highlights brighter
	vec3 specular = F0 * texture(env_tex, equirectangular(R)).rgb*
		specIntensity*specularIrradiance; 
	// Split-sum approximation factors for Cook-Torrance specular BRDF.
	//vec2 specularBRDF = texture(specularBRDF_LUT, vec2(cosLo, roughness)).rg;

	return pow(direct + diffuse + specular, vec3(1./2.2));
}
