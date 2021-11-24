vec3 hemisphereLight(vec3 rd, vec3 lightpos) {
	vec3 ldir = normalize(lightpos);
	float ldotr = max(dot(rd, ldir), 0.);
	float sun = (pow(ldotr, 10.) + pow(ldotr, 200.));
	float horizon = pow(max(rd.y, 0.), 0.03);
	float atmos = pow(max(rd.y, 0.), 0.4);
	float haze =  (dot(rd, ldir + vec3(0, 1, 0))*0.25+0.5);
	return atmos * 0.2 + mix((atmos * haze) * vec3(0.6, 0.6, 1.), (sun * horizon) * vec3(1., 0.9, 0.7), 0.7) * vec3(1.);
}

// convert a (normalized) ray direction into a texture coordinate for equirectangular mapping
vec2 equirectangular(vec3 rd) {
	return vec2(atan(rd.z, rd.x)*0.63661977, rd.y*0.5+0.5);
}
