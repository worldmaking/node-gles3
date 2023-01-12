// back project along the vector field to guess where we were previously:
vec4 prev(vec2 coord) {
	coord -= A(coord).xy*0.5;
	coord -= A(coord).xy*0.5;
	return A(coord);
}

void mainImage( out vec4 OUT, in vec2 COORD ){
	vec2 uv = COORD / iResolution.xy;

	OUT = vec4(0);
	
	// past neighborhood states (per flow)
	vec4 p = prev(COORD),
		n = prev(COORD + vec2( 0, 1)),
		s = prev(COORD + vec2( 0,-1)),
		e = prev(COORD + vec2( 1, 0)),
		w = prev(COORD + vec2(-1, 0));
	// diffused past:
	vec4 avg = (n+s+e+w)*0.25;
	// ordered difference in the pressure/convergence/disorder (A.z) 
	// creates velocity in me (OUT.xy)
	vec2 force = -0.25*vec2(e.z-w.z, n.z-s.z);
	// new velocity derived from neighbourhood average
	// should this be p.xy rather than avg.xy?
	// either the velocity or the pressure should be diffused, but not both
	float blend = 0.;  // I like blend=0 more, it gives more turbulence; 1 is more smoky
	//OUT.xy = avg.xy + force;
	OUT.xy = mix(p.xy, avg.xy, blend) + force; 
	
	// variance in the velocity (A.xy) near me creates pressure/convergence/disorder in me
	float press = -0.25*(e.x + n.y - w.x - s.y);
	// should this be avg.z rather than p.z  ?
	//OUT.z = p.z + press;
	OUT.z = mix(avg.z, p.z, blend) + press;
	
	/*
		This whole thing about bouncing energy between the velocity and pressure reminds me of scatter junctions in physical models!
	*/
	
	// mass transport
	float transport = -0.25*(e.x*e.w - w.x*w.w + n.y*n.w - s.y*s.w);
	// can mix between p.w and avg.w here to allow general diffusion of mass
	// slightly unrealistic in that this can result in negative mass
	OUT.w = mix(p.w, avg.w, 0.) + transport;
	
	// optional add forces
	float d = line2(COORD, DIM/2. - DIM.y*0.2* vec2(sin(iTime*0.2),cos(iTime*0.2)), DIM/2. + DIM.y*0.4* vec2(sin(iTime*.1618),cos(iTime*.1618)));
	if (d < 1.) {
		OUT = vec4(cos(iTime*PI), sin(iTime*PI), 0, 1);
	}
	if (iMouse.z > 0. && length(iMouse.xy - COORD) < 4.) {
		OUT = vec4(COORD/DIM - 0.5, 0., 1.);
	}
	
	// optional decays
	// xy or z, don't need to do both
	// OUT.xy *= 0.99;
	//OUT.z *= 0.9999;
	OUT.w *= 0.9999;
	
	// boundary:
	float b = 4.;
	if (COORD.x < b || COORD.y < b || DIM.x-COORD.x < b || DIM.y-COORD.y < b) {
		OUT = vec4(0);
	}

	// init:
	if (iFrame == 0) {
		OUT = texture(iChannel1, uv);
	}
}