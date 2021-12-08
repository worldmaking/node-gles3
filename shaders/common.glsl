

const float PI = 3.1415926535897932384626433832795;

vec3 eyePosFromViewMatrix(mat4 viewmatrix) {
	return -(viewmatrix[3].xyz)*mat3(viewmatrix);
}

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
	return vec2(atan(rd.x, rd.z), asin(rd.y)) * vec2(0.159155, 0.3183099) + 0.5;
}
// get normalized (0..1) texture coordinate from (normalized) ray
vec3 equirectangularRay(vec2 uv) {
	vec2 a = (uv-0.5) * vec2(6.283185307, 3.14159265);
    vec2 c = cos(a), s = sin(a);
    return vec3(s.x*c.y, s.y, c.x*c.y); //vec2(s.x, c.x) * c.y, s.y);
}
