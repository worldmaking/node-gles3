#version 330
precision mediump float;

uniform sampler3D u_tex;
uniform float u_N;
in vec4 v_color;
in vec3 v_normal;
in vec3 v_tc;
in vec3 v_eyepos, v_raydir, v_rayexit;
in vec3 v_viewpos;
out vec4 outColor;

// assume box b:  0,0,0 to 1,1,1
float rayBoxExitDistance(vec3 ro, vec3 rd) {
	// pt = ro + rd*t => t = (pt-ro)/rd
	// assumes glsl handles case of rd component==0
    vec3 t1 = (0. - ro)/rd;
    vec3 t2 = (1. - ro)/rd;
	return min(min(max(t1.x, t2.x), max(t1.z, t2.z)), max(t1.y, t2.y));
}

// assume box b:  0,0,0 to 1,1,1
float rayBoxEntryDistance(vec3 ro, vec3 rd) {
	// pt = ro + rd*t => t = (pt-ro)/rd
	// assumes glsl handles case of rd component==0
    vec3 t1 = (0. - ro)/rd;
    vec3 t2 = (1. - ro)/rd;
	return max(max(min(t1.x, t2.x), min(t1.z, t2.z)), min(t1.y, t2.y));
}

vec3 normal4(in vec3 p, in sampler3D tex, float eps) {
	vec2 e = vec2(-eps, eps);
	// tetra points
	float t1 = texture(u_tex, p+e.yxx).x;
	float t2 = texture(u_tex, p+e.xxy).x;
	float t3 = texture(u_tex, p+e.xyx).x;
	float t4 = texture(u_tex, p+e.yyy).x;
	vec3 n = t1*e.yxx + t2*e.xxy + t3*e.xyx + t4*e.yyy;
	return normalize(n);
}

void main() {
	vec3 rd = normalize(v_raydir);
	// this assumes we rendered with front-face culling
	// for back-face culling, 
	// ro = v_rayentry, 
	// and tmax = rayBoxExitDistance(ro, rd)
	float tmax = rayBoxExitDistance(v_rayexit, -rd);
	vec3 ro = v_rayexit - tmax*rd;

	// need to clamp ro to near plane, if we are inside the volume already


	// ok now step from t=0 to t=tmax
	float a = 0.;
	float dt = 0.25 / u_N;
	//float t0 = fract(tmax/dt);

	float v = 0.;

	float t=0.;
	for (; t < tmax; t += dt) {
		float weight = min(1., tmax-t);
		//float weight = min(t, 1.);
		vec3 pt = ro + t*rd;
		float c = texture(u_tex, pt).x;
		v += c * weight;
		//a = max(a, c);
		// naive additive blending
		a += max(c, 0.)*dt * weight*16.; 
		// transmittance:
		float opacity = exp(-t * abs(c));
		//a += trans * dt * weight;
		//a = max(a, c*dt * weight*50.);
		// Cout(v) = Cin(v) * (1 - Opacity(x)) + Color(x) * Opacity(x)
		// float c1 = c*weight*8;
		// a = mix(c1, a, opacity);

		//a += 0.5*dt * weight;
		if (c*weight > 0.1) {
			//break;
		}
	}

	a = t < tmax ? 1. : 0. ;

	vec3 pt = ro + t*rd;
	vec3 n = normal4(pt, u_tex, dt);

	outColor = vec4(n*0.5+0.5, 1.);
	float ndotr = dot(n, rd);
	ndotr = pow(abs(ndotr), 0.5);
	outColor = vec4(1.-abs(ndotr) );

	//a = 1. - exp(-(a)/tmax);
	//a = 1.-exp(a/tmax);

	//outColor = vec4(v_tc, 0.2);
	// float v = texture(u_tex, v_tc).r;
	// outColor = vec4(rd, 0.5);
	//outColor *= vec4(tmax);
	outColor *= vec4(a);
	// outColor = vec4(0.1);
	// outColor = vec4(v_normal*0.5+0.5, 1.);
	//outColor = vec4(tmax);

	outColor = vec4(v * 0.1 );
	outColor = v < 1. ? vec4(v) : outColor;

	outColor = vec4(v_tc, 0.2);
	outColor = vec4(v_viewpos, 1);
}