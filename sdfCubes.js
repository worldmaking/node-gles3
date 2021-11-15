const assert = require("assert")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
const glfw = require('./glfw3.js')
const vr = require('./openvr.js')
const glutils = require('./glutils.js');

if (!glfw.init()) {
	console.log("Failed to initialize GLFW");
	process.exit(-1);
}
let version = glfw.getVersion();
console.log('glfw ' + version.major + '.' + version.minor + '.' + version.rev);
console.log('glfw version-string: ' + glfw.getVersionString());

// Open OpenGL window
glfw.defaultWindowHints();
glfw.windowHint(glfw.CONTEXT_VERSION_MAJOR, 3);
glfw.windowHint(glfw.CONTEXT_VERSION_MINOR, 3);
glfw.windowHint(glfw.OPENGL_FORWARD_COMPAT, 1);
glfw.windowHint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE);

let window = glfw.createWindow(1024, 1024, "Test");
if (!window) {
	console.log("Failed to open GLFW window");
	glfw.terminate();
	process.exit(-1);
}
glfw.setWindowPos(window, 32, 32)
glfw.makeContextCurrent(window);
console.log(gl.glewInit());
console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Core Profile?: " + (glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE)==glfw.OPENGL_CORE_PROFILE));

// Enable vertical sync (on cards that support it)
glfw.swapInterval(1); // 0 for vsync off

let vertex_shader_lib = `
// http://www.geeks3d.com/20141201/how-to-rotate-a-vertex-by-a-quaternion-in-glsl/
vec3 quat_rotate( vec4 q, vec3 v ){
	return v + 2.0 * cross( q.xyz, cross( q.xyz, v ) + q.w * v );
}
vec4 quat_rotate( vec4 q, vec4 v ){
	return vec4(v.xyz + 2.0 * cross( q.xyz, cross( q.xyz, v.xyz ) + q.w * v.xyz), v.w );
}

// equiv. quat_rotate(quat_conj(q), v):
// q must be a normalized quaternion
vec3 quat_unrotate(in vec4 q, in vec3 v) {
	// return quat_mul(quat_mul(quat_conj(q), vec4(v, w)), q).xyz;
	// reduced:
	vec4 p = vec4(
				  q.w*v.x - q.y*v.z + q.z*v.y,  // x
				  q.w*v.y - q.z*v.x + q.x*v.z,  // y
				  q.w*v.z - q.x*v.y + q.y*v.x,  // z
				  q.x*v.x + q.y*v.y + q.z*v.z   // w
				  );
	return vec3(
				p.w*q.x + p.x*q.w + p.y*q.z - p.z*q.y,  // x
				p.w*q.y + p.y*q.w + p.z*q.x - p.x*q.z,  // y
				p.w*q.z + p.z*q.w + p.x*q.y - p.y*q.x   // z
				);
}

// equiv. quat_rotate(quat_conj(q), v):
// q must be a normalized quaternion
vec4 quat_unrotate(in vec4 q, in vec4 v) {
	// return quat_mul(quat_mul(quat_conj(q), vec4(v, w)), q).xyz;
	// reduced:
	vec4 p = vec4(
				  q.w*v.x - q.y*v.z + q.z*v.y,  // x
				  q.w*v.y - q.z*v.x + q.x*v.z,  // y
				  q.w*v.z - q.x*v.y + q.y*v.x,  // z
				  q.x*v.x + q.y*v.y + q.z*v.z   // w
				  );
	return vec4(
				p.w*q.x + p.x*q.w + p.y*q.z - p.z*q.y,  // x
				p.w*q.y + p.y*q.w + p.z*q.x - p.x*q.z,  // y
				p.w*q.z + p.z*q.w + p.x*q.y - p.y*q.x,  // z
				v.w
				);
}
`

let fragment_shader_lib = `
  
////////////////////////////////
// Math
////////////////////////////////
  
#define PI 3.141592653589793
#define TWOPI 3.141592653589793*2.0

// Quantize a number
float quantize(float v, float s) {
	return floor(v/s)*s;
}

// Maximum/minumum elements of a vector
float vmax(vec2 v) {
	return max(v.x, v.y);
}

float vmax(vec3 v) {
	return max(max(v.x, v.y), v.z);
}

float vmax(vec4 v) {
	return max(max(v.x, v.y), max(v.z, v.w));
}

float vmin(vec2 v) {
	return min(v.x, v.y);
}

float vmin(vec3 v) {
	return min(min(v.x, v.y), v.z);
}

float vmin(vec4 v) {
	return min(min(v.x, v.y), min(v.z, v.w));
}
  
vec3 closest_point_on_line_segment(vec3 P, vec3 A, vec3 B) {
	vec3 AB = B-A;
	float l2 = dot(AB, AB);	// length squared
	
	if (l2 < 0.000001) {
		// line is too short, just use an endpoint
		return A;
	}
	
	// Consider the line extending the segment,
	// parameterized as A + t (AB).
	// We find projection of point p onto the line.
	// It falls where t = [(AP) . (AB)] / |AB|^2
	
	vec3 AP = P-A;
	float t = dot(AP, AB) / l2;
	
	if (t < 0.0) {
		return A; 	// off A end
	} else if (t > 1.0) {
		return B; 	// off B end
	} else {
		return A + t * AB; // on segment
	}
}
  
////////////////////////////////
// Transformations
////////////////////////////////

vec3 pTranslate(vec3 p, vec3 t) {
	return p + t;
}
  
// Rotate around a coordinate axis (i.e. in a plane perpendicular to that axis) by angle <a>.
// Read like this: R(p.xz, a) rotates "x towards z".
// This is fast if <a> is a compile-time constant and slower (but still practical) if not.
void pR(inout vec2 p, float a) {
	p = cos(a)*p + sin(a)*vec2(p.y, -p.x);
}

vec2 pRot(in vec2 p, float a) {
	p = cos(a)*p + sin(a)*vec2(p.y, -p.x);
	return p;
}

vec3 pRotYZ(in vec3 p, float a) {
	p.yz = cos(a)*p.yz + sin(a)*vec2(p.z, -p.y);
	return p;
}

vec3 pRotXZ(in vec3 p, float a) {
	p.xz = cos(a)*p.xz + sin(a)*vec2(p.z, -p.x);
	return p;
}

vec3 pRotXY(in vec3 p, float a) {
	p.xy = cos(a)*p.xy + sin(a)*vec2(p.y, -p.x);
	return p;
}

//Rotate function by:
// http://jamie-wong.com/2016/07/15/ray-marching-signed-distance-functions/
mat4 rotateY(float theta) {
    float c = cos(theta);
    float s = sin(theta);

    return mat4(
        vec4(c, 0, s, 0),
        vec4(0, 1, 0, 0),
        vec4(-s, 0, c, 0),
        vec4(0, 0, 0, 1)
    );
}

// Shortcut for 45-degrees rotation
void pR45(inout vec2 p) {
	p = (p + vec2(p.y, -p.x))*sqrt(0.5);
}
  

//	q must be a normalized quaternion
vec3 quat_rotate(vec4 q, vec3 v) {
	vec4 p = vec4(
		q.w*v.x + q.y*v.z - q.z*v.y,	// x
		q.w*v.y + q.z*v.x - q.x*v.z,	// y
		q.w*v.z + q.x*v.y - q.y*v.x,	// z
		-q.x*v.x - q.y*v.y - q.z*v.z	// w
	);
	return vec3(
		p.x*q.w - p.w*q.x + p.z*q.y - p.y*q.z,	// x
		p.y*q.w - p.w*q.y + p.x*q.z - p.z*q.x,	// y
		p.z*q.w - p.w*q.z + p.y*q.x - p.x*q.y	// z
	);
}

// equiv. quat_rotate(quat_conj(q), v):
// q must be a normalized quaternion
vec3 quat_unrotate(in vec4 q, in vec3 v) {
	// return quat_mul(quat_mul(quat_conj(q), vec4(v, 0)), q).xyz;
	// reduced:
	vec4 p = vec4(
				  q.w*v.x - q.y*v.z + q.z*v.y,  // x
				  q.w*v.y - q.z*v.x + q.x*v.z,  // y
				  q.w*v.z - q.x*v.y + q.y*v.x,  // z
				  q.x*v.x + q.y*v.y + q.z*v.z   // w
				  );
	return vec3(
				p.w*q.x + p.x*q.w + p.y*q.z - p.z*q.y,  // x
				p.w*q.y + p.y*q.w + p.z*q.x - p.x*q.z,  // y
				p.w*q.z + p.z*q.w + p.x*q.y - p.y*q.x   // z
				);
}
  

  
// Repeat around the origin by a fixed angle.
// For easier use, num of repetitions is use to specify the angle.
float pModPolar(inout vec2 p, float repetitions) {
	float angle = 2.*PI/repetitions;
	float a = atan(p.y, p.x) + angle/2.;
	float r = length(p);
	float c = floor(a/angle);
	a = mod(a,angle) - angle/2.;
	p = vec2(cos(a), sin(a))*r;
	// For an odd number of repetitions, fix cell index of the cell in -x direction
	// (cell index would be e.g. -5 and 5 in the two halves of the cell):
	if (abs(c) >= (repetitions/2.)) c = abs(c);
	return c;
}

float pModPolarShift(inout vec2 p, float repetitions, float shift) {
	float angle = 2.*PI/repetitions;
	float a = atan(p.y, p.x) + angle/2.;
	float r = length(p);
	float c = floor(a/angle);

	a = mod(a,angle) - angle/2.;
	p = vec2(cos(a), sin(a))*r;
	// For an odd number of repetitions, fix cell index of the cell in -x direction
	// (cell index would be e.g. -5 and 5 in the two halves of the cell):
	if (abs(c) >= (repetitions/2.)) c = abs(c);
	return c;
}

//https://www.shadertoy.com/view/Xds3zN
vec3 opRep( vec3 p, vec3 c )
{
    return mod(p,c)-0.5*c;
}
  

  
////////////////////////////////
// Combinations
////////////////////////////////
  
// min(a, b) : union
// max(a, b) : intersection
// max(a, -b) : subtraction (exclusion)

// polynomial smooth min (k = 0.1);
float smin( float a, float b, float k ) {
	float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
	return mix( b, a, h ) - k*h*(1.0-h);
}

float smax( float a, float b, float k ) {
	float k1 = k*k;
	float k2 = 1./k1;
	return log( exp(k2*a) + exp(k2*b) )*k1;
}
  
float ssub(in float A, in float B, float k) {
	return smax(A, -B, k);
}
  
////////////////////////////////
// Shapes
////////////////////////////////

float fSphere(vec3 p, float r) {
	return length(p) - r;
}
  
// Cylinder standing upright on the xz plane
float fCylinder(vec3 p, float r, float height) {
	float d = length(p.xz) - r;
	d = max(d, abs(p.y) - height);
	return d;
}

// Capsule: A Cylinder with round caps on both sides
float fCapsule(vec3 p, float r, float c) {
	return mix(length(p.xz) - r, length(vec3(p.x, abs(p.y) - c, p.z)) - r, step(c, abs(p.y)));
}
  
// i.e. distance to line segment, with smoothness r
float sdCapsule1(vec3 p, vec3 a, vec3 b, float r) {
	vec3 p1 = closest_point_on_line_segment(p, a, b);
	return distance(p, p1) - r;
}
  
/*p = position of ray
* a and b = endpoints of the line (capsule)
* ra = radius of a
* rb = radius of b
*/
float sdCapsule2(vec3 p, vec3 a, vec3 b, float ra, float rb) {
	vec3 pa = p - a, ba = b - a;
	float t = dot(pa,ba)/dot(ba,ba);	// phase on line from a to b
	float h = clamp( t, 0.0, 1.0 );
	
	// basic distance:
	vec3 rel = pa - ba*h;
	float d = length(rel);
	
	d = d - mix(ra, rb, h);
	
	return d;
}

// Box: correct distance to corners
float fBox(vec3 p, vec3 b) {
	vec3 d = abs(p) - b;
	return length(max(d, vec3(0))) + vmax(min(d, vec3(0)));
}
  
// Signed distance function for a cube centered at the origin
// http://jamie-wong.com/2016/07/15/ray-marching-signed-distance-functions/
float sdCube(in vec3 p, in vec3 r){
	vec3 d = abs(p) - r;
	// Assuming p is inside the cube, how far is it from the surface?
    // Result will be negative or zero.
	float inDist = min(max(d.x, max(d.y, d.z)), 0.0);
	// Assuming p is outside the cube, how far is it from the surface?
    // Result will be positive or zero.
	float outDist = length(max(d, 0.0));
	return inDist + outDist;
}

// iq has this version, which seems a lot simpler?
float sdEllipsoid1( in vec3 p, in vec3 r ) {
	return (length( p/r ) - 1.0) * min(min(r.x,r.y),r.z);
}
  
//https://www.shadertoy.com/view/Xds3zN
float sdCone( in vec3 p, in vec3 c )
{
    vec2 q = vec2( length(p.xz), p.y );
    float d1 = -q.y-c.z;
    float d2 = max( dot(q,c.xy), q.y);
    return length(max(vec2(d1,d2),0.0)) + min(max(d1,d2), 0.);
}
`

let cubeprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;

// instanced variable:
in vec4 i_pos;     
in vec4 i_bounds;  // xyz is bounding box, w is scale factor
in vec4 i_quat;    

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;

out vec3 v_normal;
out vec3 v_eyepos;
out vec3 v_raypos;
out vec3 v_raydir;

out vec4 v_quat;
out vec4 v_bounds;
out mat4 v_viewprojmatrix;
out vec4 v_world; // xyz is position, w is dist to camera

${vertex_shader_lib}

void main() {
	vec4 vertex = vec4(a_position, 1.);
	// apply instance transform:
	vertex.xyz *= i_bounds.xyz;
	vertex.xyz *= i_bounds.w;
	vertex = quat_rotate(i_quat, vertex);
	vertex.xyz += i_pos.xyz;

	vec4 world = /*u_modelmatrix * */ vertex;
	vec4 view = u_viewmatrix * world;
	gl_Position = u_projmatrix * view;

	// derive eyepos (worldspace)
	v_eyepos = -(u_viewmatrix[3].xyz)*mat3(u_viewmatrix);
	// derive ray (object space)
	v_raypos = a_position.xyz * i_bounds.xyz;
	v_raydir = (quat_unrotate(i_quat, world.xyz - v_eyepos));

	// if we needed precision, refine this in frag shader based on the surface function
	v_world = vec4(i_pos.xyz, length(view.xyz));
	v_normal = vec3(/* mat3(u_modelmatrix) * */ quat_rotate(i_quat, a_normal));

	v_quat = i_quat;
	v_bounds = i_bounds;
	v_viewprojmatrix = u_projmatrix * u_viewmatrix;

	// // q: can we orient the boxes to always face the camera?

	// mat3 viewmat = mat3(u_viewmatrix);

	// mat3 unviewmat = transpose(viewmat);

	// // vec3 viewX = vec3(viewmat[0][0], viewmat[1][0], viewmat[2][0]);
	// // vec3 viewY = vec3(viewmat[0][1], viewmat[1][1], viewmat[2][1]);
	// // vec3 viewZ = vec3(viewmat[0][2], viewmat[1][2], viewmat[2][2]);

	// v_world_vertex = a_position * i_scale;
	// v_world_vertex = unviewmat * v_world_vertex;
	// //v_world_vertex = quat_rotate(i_quat, v_world_vertex);
	// v_world_vertex = v_world_vertex + i_pos.xyz;
	

	// // rotate to face camera:
  	// vec4 camera_position = u_viewmatrix * vec4(v_world_vertex, 1);

	// gl_Position = u_projmatrix * camera_position;

	// v_normal = quat_rotate(i_quat, a_normal); // world space

	// // derive eye location in world space from current view matrix:
	// // (could pass this in as a uniform instead...)
	// v_eyepos = -(u_viewmatrix[3].xyz)*mat3(u_viewmatrix);
	// v_viewprojmatrix = u_projmatrix * u_viewmatrix;
  	// // we want the raymarching to operate in object-local space:
	// v_ray_origin = a_position;
	// v_ray_direction = v_world_vertex - v_eyepos; 
	// //v_ray_direction = quat_unrotate(i_quat, v_ray_direction); 

	// v_world_orientation = vec4(0, 0, 0, 1);//i_quat;
}
`,
`#version 330
precision mediump float;

uniform vec2 u_nearfar;

in vec4 v_quat;
in vec4 v_world; 
in vec4 v_bounds; // xyz, scale
//in vec4 v_color;
in vec3 v_normal;
in vec2 v_texCoord;
in vec3 v_eyepos, v_raypos, v_raydir;
in mat4 v_viewprojmatrix;

in vec4 v_debug;

out vec4 outColor;

${fragment_shader_lib}

float scene(vec3 p) {
	float d0 = fSphere(p, 0.3);
	float d1 = fCylinder(p, 0.4, 0.5);
	float d2 = sdCapsule2(p, vec3(0., 0, -0.4), vec3(0., 0., -.4), 0.2, 0.3);
	float d3 = sdCube(p, vec3(0.1, 0.2, 0.4));
	//return d0;
	return min(d0, d3);
}

vec2 texcoord(vec3 p) {
	// get a texture coordinate from the scene
	// a simple cheat is cylindrical mapping of p
	vec3 pn = normalize(p);
	// atan2(y,x)/2pi gives -0.5..0.5 range
	return vec2(atan(pn.y, pn.x) * 0.159154943091895 + 0.5, pn.z);
}

mat3 tbn4(in vec3 p, float eps) {
	vec2 e = vec2(-eps, eps);
	// get four nearby points (tetrahedral distribution):
	vec3 p1 = p + e.yxx, p2 = p + e.xxy, p3 = p + e.xyx, p4 = p + e.yyy;
	// get distances at these points:
	float t1 = scene(p + e.yxx), t2 = scene(p + e.xxy), t3 = scene(p + e.xyx), t4 = scene(p + e.yyy);
	vec3 N = normalize(e.yxx*t1 + e.xxy*t2 + e.xyx*t3 + e.yyy*t4);
	// get texcoords at these points:
	vec2 tc1 = texcoord(p1), tc2 = texcoord(p2), tc3 = texcoord(p3), tc4 = texcoord(p4); 
	vec3 T = normalize(e.yxx*tc1.y + e.xxy*tc2.y + e.xyx*tc3.y + e.yyy*tc4.y);
	// force it to be orthogonal:
	T = normalize(T - N*dot(N,T));
	// bitangent is orthogonal to both:
	vec3 B = cross(N, T);//normalize(e.yxx*tc1.y + e.xxy*tc2.y + e.xyx*tc3.y + e.yyy*tc4.y);
	return mat3(T, B, N);
}

// compute normal from a SDF gradient by sampling 4 tetrahedral points around a location p
// (cheaper than the usual technique of sampling 6 cardinal points)
// 'scene' should be the SDF evaluator 'float distance = scene(vec3 pos)''  
// 'eps' is the distance to compare points around the location 'p' 
// a smaller eps gives sharper edges, but it should be large enough to overcome sampling error
// in theory, the gradient magnitude of an SDF should everywhere = 1, 
// but in practice this isn’t always held, so need to normalize() the result
vec3 normal4(in vec3 p, float eps) {
	vec2 e = vec2(-eps, eps);
	// tetrahedral points
	float t1 = scene(p + e.yxx), t2 = scene(p + e.xxy), t3 = scene(p + e.xyx), t4 = scene(p + e.yyy); 
	vec3 n = (e.yxx*t1 + e.xxy*t2 + e.xyx*t3 + e.yyy*t4);
	// normalize for a consistent SDF:
	//return n / (4.*eps*eps);
	// otherwise:
	return normalize(n);
}

vec4 shade(vec3 p) {
	const float EPS = 0.003;
	vec4 outColor;

	// get a texcoord from the surface
	// ideally, the sdf itself would return a texcoord
	vec2 tc = texcoord(p);

	// for normal, we approximate it by testing the scene at nearby points
	// for tangent/bitangent, we do the same, using texcoords for the surface orientation
	mat3 TBN = tbn4(p, EPS);

	// all of these are in object-space:
	// rotate to world space
	vec3 T = quat_rotate(v_quat, TBN[0]);
	vec3 B = quat_rotate(v_quat, TBN[1]);
	vec3 N = quat_rotate(v_quat, TBN[2]);
	
	outColor = vec4(N*0.5+0.5, 1.);
	outColor = vec4(T*0.5+0.5, 1.);
	outColor = vec4(B*0.5+0.5, 1.);
	// outColor = vec4( abs(dot(N, T)) ); // verify that N, T are orthogonal; should be zero
	// outColor = vec4( abs(dot(N, B)) ); // verify that N, B are orthogonal; should be zero
	// outColor = vec4( abs(dot(T, B)) ); // verify that B, T are orthogonal; should be zero
	outColor = vec4(tc, 0., 1.);
	// demo texture for debugging:
	vec2 chk = mod(tc*2., 1.)-0.5;
	float checker = sign(chk.x*chk.y)*0.5+0.5;
	outColor = vec4(N*checker, 1.);

	// now go ahead and do the lighting & texturing of choice
	return outColor;
}

// p is the vec3 position of the surface at the fragment
// p should be in world-space
// viewProjectionMatrix would be typically passed in as a uniform
// assign result to gl_FragDepth:
float computeDepth(vec3 p, mat4 viewProjectionMatrix) {
	float dfar = 1.;//gl_DepthRange.far;
	float dnear = 0.;//gl_DepthRange.near;
	vec4 clip_space_pos = viewProjectionMatrix * vec4(p, 1.);
	float ndc_depth = clip_space_pos.z / clip_space_pos.w;	
	// standard perspective:
	return (((dfar-dnear) * ndc_depth) + dnear + dfar) / 2.0;
}

void main() {

	vec3 rd = normalize(v_raydir);
	vec3 ro = v_raypos;
	float scale = v_bounds.w;
	vec3 worldpos = v_world.xyz;
	float dist = v_world.w; // maybe used for fog etc.

	#define STEPS 64
	#define FAR 3.0
	const float EPS = 1./float(STEPS);
	vec3 p = ro;
	float t = 0.;
	int step = 0;
	float d = 0.;
	float d0 = 0.;
	int contact = 0;
	for (; step < STEPS && t < FAR; step++) {
		p = ro + t*rd;
		d = scene(p);
		if (sign(d)*sign(d0) == -1.) { // surface crossing
			contact++;
			// render at corrected surface position:
			p = ro + (t-abs(d))*rd;
			// += for additive blending
			// max() for max blending
			// mix() for something in between
			//outColor += shade(p) / float(contact);
			//outColor = mix(outColor, shade(p), 1. / float(contact));
			//outColor = max(outColor, shade(p)/ float(contact));
			if (contact == 1) {
				// first contact defines actual world position:
				worldpos += quat_rotate(v_quat, p * scale);
			}
			//break;  // break here for solid shape
		}
		d0 = d;
		// always move forward:
		t += max(EPS,abs(d));
	}

	float glow = float(step)/float(STEPS);
	//outColor += vec4(glow*glow); // show halo
	outColor = mix(outColor, vec4(glow), glow*glow);
	
	// for deadzone:
	if (contact == 0) {
		// outColor += vec4(0.25); // show bounding box
		// discard;
	}

	gl_FragDepth = computeDepth(worldpos, v_viewprojmatrix);
	
	//outColor = vec4(dist);
	// outColor = vec4(v_pos);
	// outColor = vec4(v_bounds);
	// outColor = vec4(v_normal*0.5+0.5, 1.);
	// //outColor = vec4(v_world);
	// outColor = vec4(v_texCoord, 0., 1.);
	// outColor = vec4(v_eyepos, 1.);
	// outColor = vec4(v_raypos, 1.);
	// outColor = vec4(v_raydir, 1.);
	// outColor = vec4(rd, 1.);
	// outColor = vec4(d);
	// outColor = vec4(contact);
	// outColor = vec4(t * float(contact) + glow);
	//outColor = vec4(worldpos, 1.);
}

`);
// create a VAO from a basic geometry and shader
let cube = glutils.createVao(gl, glutils.makeCube({ min:-0.5, max:0.5, div: 8 }), cubeprogram.id);

// create a VBO & friendly interface for the instances:
// TODO: could perhaps derive the fields from the vertex shader GLSL?
let cubes = glutils.createInstances(gl, [
	{ name:"i_pos", components:4 },
	{ name:"i_quat", components:4 },
	{ name:"i_bounds", components:4 },
], 100)

// the .instances provides a convenient interface to the underlying arraybuffer
cubes.instances.forEach(obj => {
	// each field is exposed as a corresponding typedarray view
	// making it easy to use other libraries such as gl-matrix
	// this is all writing into one contiguous block of binary memory for all instances (fast)
	vec4.set(obj.i_pos, 
		(Math.random()-0.5) * 20,
		(Math.random()-0.5) * 20,
		(Math.random()-0.9) * 20,
		1
	);
	// xyz is bounding box, w is scale factor
	let s = 1
	vec4.set(obj.i_bounds, s, s, s, 1);
	quat.random(obj.i_quat);
})
cubes.bind().submit().unbind();

// attach these instances to an existing VAO:
cubes.attachTo(cube);


let t = glfw.getTime();
let fps = 60;

function animate() {
	if(glfw.windowShouldClose(window) || glfw.getKey(window, glfw.KEY_ESCAPE)) {
		shutdown();
	} else {
		setImmediate(animate)
	}

	let t1 = glfw.getTime();
	let dt = t1-t;
	fps += 0.1*((1/dt)-fps);
	t = t1;
	glfw.setWindowTitle(window, `fps ${fps}`);
	// Get window size (may be different than the requested size)
	let dim = glfw.getFramebufferSize(window);

	// update scene:

	// pick a random instance:
	let q = [0, 0, 0, 1]
	for (let obj of cubes.instances) {
		//let obj = cubes.instances[Math.floor(Math.random() * cubes.count)];
		// change its orientation:
		quat.random(q);
		quat.slerp(obj.i_quat, obj.i_quat, q, 0.01);
		// submit to GPU:
		cubes.bind().submit().unbind()
	}

	// Compute the matrix
	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	let near = 0.01, far = 30;
	let angle = t*0.3;
	let dist = 4;
	mat4.lookAt(viewmatrix, [dist*Math.sin(angle), 0, dist*Math.cos(angle)], [-dist*Math.sin(angle), 0, -dist*Math.cos(angle)], [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], near, far);

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clearColor(0.1, 0.1, 0.1, 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.enable(gl.DEPTH_TEST)

	gl.enable(gl.BLEND);
	gl.blendEquation(gl.FUNC_ADD)
	gl.blendFunc(gl.SRC_ALPHA, gl.ONE);
	gl.blendFuncSeparate(gl.ONE, gl.ONE, gl.ZERO, gl.ONE_MINUS_SRC_ALPHA);

	gl.depthMask(false)
	gl.enable(gl.CULL_FACE)

	cubeprogram.begin();
	cubeprogram.uniform("u_viewmatrix", viewmatrix);
	cubeprogram.uniform("u_projmatrix", projmatrix);
	cubeprogram.uniform("u_nearfar", near, far);
	cube.bind().drawInstanced(cubes.count).unbind()
	cubeprogram.end();

	gl.disable(gl.CULL_FACE)
	gl.disable(gl.BLEND);
	gl.depthMask(true)

	// Swap buffers
	glfw.swapBuffers(window);
	glfw.pollEvents();
	
}

function shutdown() {
	// Close OpenGL window and terminate GLFW
	glfw.destroyWindow(window);
	glfw.terminate();

	process.exit(0);
}

animate();