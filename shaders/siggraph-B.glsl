
// Important details:
// - field here represents nearest particle (.xy = pos, .w = species, .y unused)
// - the way particles advect is from a blend between real particle position and pixel position; 
//   this affects rate of particle creation/destruction
// - there's a test for when the particles became stable, and an event to change species at this point;
//   this keeps the churn moving, since difference in the hormone field will cause fluid velocity
// - new species here is always determinstic: species A will always lead to species B

// a trick to do continuous float positions on a pixel grid:
// let each pixel refer to the nearest particle, storing the continuous position in .xy
// it works so long as the particles are never nearer than one pixel to each other
// (in that case a particle can die)

// effectively several pixels may compute results for one pixel

// field B represents the nearest particle to our pixel
// B.xy is the actual particle position

int id_from_pixel(vec2 P) {
    return int(mod(rand(P) + iTime*0.001, 1.)*IMG_GRID*IMG_GRID);
}

void swap (inout vec4 p, vec2 P, vec2 r) {
    // Q is our current estimated nearest particle
    // n is the particle at a pixel nearby
	vec4 n = B(P+r); 
    // if n is closer to our pixel coordinate, pick that one instead (via Q=n)
    if (length(P-n.xy) < length(P-p.xy)) p = n;
}

void mainImage( out vec4 OUT, in vec2 P) {

    float spd = 100.*iTimeDelta;

    // FIND NEAREST PARTICLE
    vec4 p = B(P);
    // in each axis consider a couple of steps:
    // if particle.xy there is actually closer to our pixel, use that particle instead
    if (true) {
        int d = 3;
        for (int y=-d; y<=d; y++) {
            for (int x=-d; x<d; x++) {
                vec4 n = B(P+vec2(x, y));
                if (length(P-n.xy) < length(P-p.xy)) p = n;
            }
        }
    } else {
        // cheaper but less accurate version:
        swap(p,P,vec2(1,0));
        swap(p,P,vec2(0,1));
        swap(p,P,vec2(-1,0));
        swap(p,P,vec2(0,-1));
        swap(p,P,vec2(2,2));
        swap(p,P,vec2(2,-2));
        swap(p,P,vec2(-2,2));
        swap(p,P,vec2(-2,-2));
    }
    // now we have our nearest particle `p`
    
 
    
    // advect the particle 
    vec2 vel = A(p.xy).xy; //p.zw;
    
    //vel = mix(vel, A(p.xy).xy, 0.001);
    
    //p.zw = vel;
    
    p.xy += vel * spd;
    
    //p.zw = rotate2(p.zw, 0.01);
    
       // is it too far?
    if (length(P-p.xy) > 100.) {
        p.xy = P;
       //p.zw = D(P).xy - 0.5;

       p.w = id_from_pixel(p.xy);
    }
    
    // BOUNDARY CONDITIONS
    if ((iMouse.z>0.&&length(iMouse.xy-P)<30.)) {
       p.xy = P;
       //p.zw = D(P).xy - 0.5;
       p.w = id_from_pixel(P.xy);
    }
    
    // BOUNDARY CONDITIONS
    if (int(iFrame) % 4800 == 0) {

        // quantize to grid:
        float grid = 40.;

       p.xy = floor(P/grid)*grid;
       //p.zw = D(P).xy-0.5;

       p.w = id_from_pixel(p.xy/grid);
    }
    
    OUT = p;
}
