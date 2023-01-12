/* deeply inspired by the amazing work of @wyatt */
#define DIM iResolution.xy

#define PI 3.14159265359

#define A(uv) texture(iChannel0, uv/DIM)
#define B(uv) texture(iChannel1, uv/DIM)
#define C(uv) texture(iChannel2, uv/DIM)
#define D(uv) texture(iChannel3, uv/DIM)

#define norm(v) ((v)/(length(v)+1e-10))

// distance of pt to 2D line segment from start to end
float line2(vec2 pt, vec2 start, vec2 end) {
	vec2 g = end-start, h = pt-start;
	return length(h - g*clamp(dot(g,h)/dot(g,g), 0.0, 1.));
}

vec3 hsl2rgb( in vec3 c )
{
	vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );
	return c.z + c.y * (rgb-0.5)*(1.0-abs(2.0*c.z-1.0));
}
