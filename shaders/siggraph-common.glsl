#define R iResolution.xy
#define DIM iResolution.xy
#define A(U) texture(iChannel0, (U)/R)
#define B(U) texture(iChannel1, (U)/R)
#define C(U) texture(iChannel2, (U)/R)
#define D(U) texture(iChannel3, (U)/R)

#define PI 3.14159265359

#define IMG_GRID 16

#define norm(v) ((v)/(length(v)+1e-10))

// distance of pt to 2D line segment from start to end
float line2_b(vec2 pt, vec2 start, vec2 end) {
    vec2 g = end-start, h = pt-start;
    return length(h - g*clamp(dot(g,h)/dot(g,g), 0.0, 1.));
}

// distance from point p to the nearest point on the line a->b
float line2(vec2 p, vec2 a, vec2 b) {
    // position on line a+tb
    float t = dot(p-a,b-a)/dot(b-a,b-a); 
    // clamp to line bounds:
    t = clamp(t,0.,1.);
    // nearest point:
    vec2 n = a + t*(b-a);
    // distance to that point:
	return length(p-n);
}

float circle2( vec2 p, float r )
{
    return length(p) - r;
}

float box2( in vec2 p, in vec2 b )
{
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

float orientedBox2( in vec2 p, in vec2 a, in vec2 b, float th )
{
    float l = length(b-a);
    vec2  d = (b-a)/l;
    vec2  q = (p-(a+b)*0.5);
          q = mat2(d.x,-d.y,d.y,d.x)*q;
          q = abs(q)-vec2(l,th)*0.5;
    return length(max(q,0.0)) + min(max(q.x,q.y),0.0);    
}

vec2 rotate2(vec2 p, float a) {
    float sine = sin(a), cosine = cos(a);
    return vec2(cosine * p.x + sine * p.y, cosine * p.y - sine * p.x);
}

vec3 hsl2rgb( in vec3 c )
{
    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );
    return c.z + c.y * (rgb-0.5)*(1.0-abs(2.0*c.z-1.0));
}

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

vec2 hash2( vec2 p ) // replace this by something better
{
	p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

// a deterministic random number generator
vec4 hash (float p) // Dave (Hash)kins
{
	vec4 p4 = fract(vec4(p) * vec4(.1031, .1030, .0973, .1099));
    p4 += dot(p4, p4.wzxy+19.19);
    return fract((p4.xxyz+p4.yzzw)*p4.zywx)*2.-1.;
    
}