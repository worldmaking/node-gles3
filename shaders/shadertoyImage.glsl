void mainImage( out vec4 OUT, in vec2 COORD )
{
// OUT = vec4(0.0,0.0,1.0,1.0);
//
	// Normalized pixel coordinates (from 0 to 1)
//  vec2 uv = COORD/DIM;

	// Time varying pixel color
// vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));

	// Output to screen
// OUT = vec4(norm(col),1.0);
	
	// line test
// float d = line2(COORD, vec2(40,40), vec2(200,200));
	//OUT = vec4(exp(-d));
	
	vec4 a = A(COORD);
	
	OUT.rgb = a.www * hsl2rgb(vec3(0.25*dot(a.xy, vec2(1,0)), abs(a.z), 0.75));
}