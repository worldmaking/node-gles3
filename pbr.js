const assert = require("assert"),
fs = require("fs")
const glfw = require("./glfw3.js")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
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
glfw.makeContextCurrent(window);
console.log(gl.glewInit());

//can only be called after window creation!
console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Profile: " + glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE));

// Enable vertical sync (on cards that support it)
glfw.swapInterval(1); // 0 for vsync off
glfw.setWindowPos(window, 5, 25)


// Euclidean modulo. assumes n > 0
function wrap(a, n) { 
	const r = a % n;
	return r < 0 ? r + n : r; //a % n + (Math.sign(a) !== Math.sign(n) ? n : 0); 
}

function vec3_wrap(out, a, n) {
	out[0] = wrap(a[0], n)
	out[1] = wrap(a[1], n)
	out[2] = wrap(a[2], n)
	return out
}


function vec3_bound(out, a, min, max) {
	out[0] = min[0] + wrap(a[0]-min[0], max[0]-min[0])
	out[1] = min[1] + wrap(a[1]-min[1], max[1]-min[1])
	out[2] = min[2] + wrap(a[2]-min[2], max[2]-min[2])
	return out
}


function jpg2tex(gl, path) {
	const fs = require("fs");
	const jpeg = require('jpeg-js');

	let jpg = jpeg.decode(fs.readFileSync(path));
	let tex = glutils.createPixelTexture(gl, jpg.width, jpg.height)
	assert(tex.data.length == jpg.data.length);
	tex.data = jpg.data;
	tex.bind().submit()
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
	tex.unbind();

	return tex;
}

let world_min = [-4, 0, -4];
let world_max = [+4, 8, +4];
let camera_pos = [0, 1.7, 0.25];
let camera_at = [0, 1.7, 0];
let light_pos = [1, 3, 1];

let color_tex = png2tex(gl, 'textures/metal_albedo.png') 
let normal_tex = png2tex(gl, 'textures/metal_normal.png') 
let metalness_tex = png2tex(gl, 'textures/metal_metalness.png') 
let roughness_tex = png2tex(gl, 'textures/metal_roughness.png') 

let quat_shader_lib = `
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

let sdf_shader_lib = fs.readFileSync("hg_sdf.glsl")

let cubesprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_modelmatrix;
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;


// instanced variable:
in vec4 i_pos;
in vec4 i_bounds;  // xyz is bounding box, w is scale factor
in vec4 i_quat;

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;
out vec4 v_color;
//out vec4 v_normal;
out vec4 v_world;
out vec2 v_texCoord;
out vec3 v_raypos, v_raydir, v_eyepos;
out vec4 v_quat;
out vec4 v_pos;
out vec4 v_bounds;
out mat4 v_viewprojmatrix;


${quat_shader_lib}

void main() {
	vec4 vertex = vec4(a_position, 1.);

	// apply instance transform:
	vertex.xyz *= i_bounds.xyz;
	vertex.xyz *= i_bounds.w;
	vertex = quat_rotate(i_quat, vertex);
	vertex.xyz += i_pos.xyz;

	//vec4 v = u_modelmatrix * vec4(quat_rotate(i_quat, (a_position * i_bounds.w)) + i_pos.xyz, 1. );

	//gl_Position = u_projmatrix * u_viewmatrix * vertex;
	vec4 world = u_modelmatrix * vertex;
	vec4 view = u_viewmatrix * world;
	gl_Position = u_projmatrix * view;

	// derive eyepos (worldspace)
	v_eyepos = -(u_viewmatrix[3].xyz)*mat3(u_viewmatrix);
	// derive ray (object space)
	v_raypos = a_position.xyz * i_bounds.xyz;
	v_raydir = (quat_unrotate(i_quat, world.xyz - v_eyepos));
	
	// if we needed precision, compute this in frag shader based on the surface function
	v_world = vec4(world.xyz, length(view.xyz));
	//v_normal = vec4(mat3(u_modelmatrix) * quat_rotate(i_quat, a_normal), length(view.xyz));
	v_color = vec4(1);
	v_texCoord = a_texCoord;

	v_quat = i_quat ;
	v_pos = i_pos ;
	v_bounds = i_bounds ;
	v_viewprojmatrix = u_projmatrix * u_viewmatrix;
}
`,
`#version 330
precision mediump float;

uniform sampler2D u_color_tex;
uniform sampler2D u_normal_tex;
uniform sampler2D u_metalness_tex;
uniform sampler2D u_roughness_tex;
uniform vec3 u_camera_pos;
uniform vec3 u_light0_pos;

in vec4 v_quat;
in vec4 v_pos; // xyz, scale
in vec4 v_bounds;
in vec4 v_color;
//in vec4 v_normal;
in vec4 v_world;
in vec2 v_texCoord;
in vec3 v_eyepos, v_raypos, v_raydir;
in mat4 v_viewprojmatrix;
out vec4 outColor;

${quat_shader_lib}

${sdf_shader_lib}

const vec2 invAtan = vec2(0.1591, 0.3183);
const float MAX_REFLECTION_LOD = 12.0;

float distributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / denom;
}
float geometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
	//float a = roughness;
    //float k = (a * a) / 2.0;
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
} 

// normal assumed to be in world-space at this point:
vec4 render(vec4 albedo_opacity, vec4 normal_w, vec4 world_distance, vec4 metal_rough_ao_emissive) {
	vec3 albedo = albedo_opacity.rgb;
	float opacity = albedo_opacity.a;
	vec3 normal = normal_w.xyz;
	vec3 worldpos = world_distance.xyz;
	float distance = world_distance.w;
	float metalness = metal_rough_ao_emissive.r;
	float roughness = metal_rough_ao_emissive.g;
	float ao = metal_rough_ao_emissive.b;
	float emissive = metal_rough_ao_emissive.a;

	// outgoing vector from surface to eye, world space
	vec3 V = normalize(u_camera_pos - worldpos);
	vec3 N = normalize(normal);
	vec3 R = reflect(-V, N);

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
	vec3 F0 = mix(vec3(0.04), albedo, metalness);

	// reflectance equation
    vec3 direct_lighting = vec3(0.0);
	//int i=0;

	// for each light
	for(int i = 0; i < 8; ++i) {

		float a = PI * 2. * float(i)/8.;
	
		//vec3 light_pos = u_light0_pos; // in world space
		float ld = 3.;
		vec3 light_pos = vec3(ld * cos(a), 4., ld * sin(a));
        vec3 light_color = vec3(cos(a), sin(a), 0.)*0.3+0.5;
		float light_spotexponent = 1.0; //float(i)*4.;

		// incoming vector from light to surface, world space
        vec3 light_dir = normalize(light_pos - worldpos);
		// halfvector between incoming and outgoing rays
        vec3 H = normalize(V + light_dir);
		// similarity of light vector & normal vector
		// cosTheta of angle between them
        float NdotL = max(dot(N, light_dir), 0.0);   

		// calculate per-light radiance
        float light_distance = length(light_pos - worldpos);
        float attenuation = 1.0 / pow(light_distance, light_spotexponent);
        vec3 radiance = light_color * attenuation;

        // Cook-Torrance BRDF
        float NDF = distributionGGX(N, H, roughness);   
        float G   = geometrySmith(N, V, light_dir, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
        //vec3 F    = fresnelSchlick(abs(dot(H, V)), F0); // two-sided
           
        vec3 nominator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.001; // 0.001 to prevent divide by zero.
        vec3 spec = nominator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metalness;	  

        // scale light by NdotL     

        // add to outgoing radiance Lo
        // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
		direct_lighting += (kD * (albedo / PI) + spec) * radiance * NdotL;
	}

	vec3 color = direct_lighting;

	// indirect lighting:
	// ambient lighting from environment map:
	//vec3 ambient = equirectangular(irradianceMap, 
	//	gl_TextureMatrix[5], 
	///	normalize(N), 
	//	roughness * 8.).rgb;

	vec3 kAS = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 kAD = vec3(1.0) - kAS;
	kAD *= 1.0 - metalness;	
	
	// vec3 irradiance = equirectangular(irradianceMap, 
	// 	gl_TextureMatrix[5], 
	// 	normalize(N), 
	// 	MAX_REFLECTION_LOD).rgb;
	vec3 irradiance =  N*0.3+0.5; ;
	vec3 diffuse    = irradiance * albedo;

	// IBL specular:
	// vec3 prefilteredColor = equirectangular(irradianceMap, 
	// 	gl_TextureMatrix[5], 
	// 	normalize(R), 
	// 	roughness * MAX_REFLECTION_LOD).rgb;
	vec3 prefilteredColor = R*0.3+0.5; //vec3(0.5);
	// vec2 envBRDF  = texture2D(brdfLUTMap, vec2(max(dot(N, V), 0.0), roughness)).rg;
	// //vec3 specular = prefilteredColor * (kAS * envBRDF.x + envBRDF.y);
	vec3 specular = prefilteredColor * (kAS);

	color += kAD * diffuse * ao;
	color += specular * ao;

	// // HDR tonemapping
    // color = color / (color + vec3(1.0));
    // // gamma correct
    // color = pow(color, vec3(1.0/2.2)); 

	//return vec4(albedo, 1.0 );
	//return vec4(normal, 1.0 );
	// return vec4( worldpos, 1.0 );
	// return vec4( distance );
	// return vec4( metalness );
	// return vec4( roughness );
	// return vec4( ao );
	// return vec4( emissive );
	
	// return vec4(V, 1.0 );
	// return vec4(N, 1.0 );
	// return vec4(R, 1.0 );
	// return vec4(F0, 1.0 );
	// return vec4(direct_lighting, 1.0 );
	// return vec4(kAD, 1.0 );
	// return vec4(irradiance, 1.0 );
	//return vec4(diffuse, 1.0 );
	//return vec4(prefilteredColor, 1.0 );
	// return vec4(specular, 1.0 );
	//return vec4(ambient, 1.0 );

	return vec4(vec3(color), 1.);
}

// https://www.shadertoy.com/view/Ml3fWj
vec4 opElongate( in vec3 p, in vec3 h ) {
    //return vec4( p-clamp(p,-h,h), 0.0 ); // faster, but produces zero in the interior elongated box
    
    vec3 q = abs(p)-h;
    return vec4( max(q,0.0), min(max(q.x,max(q.y,q.z)),0.0) );
}

float DE(vec3 p) {
	//return fSphere(p, 1.);
	//return fCylinder(p.xzy, 0.25 , 1. );
	//return fTorus(p.xzy, 0.02, 0.25);
	
	vec3 q = p.xzy;
	//return fTorus( q, 0.02, 0.25 );
    vec4 w = opElongate( q, vec3(0.0, 1., 0.0) );
   	return w.w + fTorus( w.xyz, 0.003, 0.25 );

}

// compute normal from a SDF gradient by sampling 4 tetrahedral points around a location p
// (cheaper than the usual technique of sampling 6 cardinal points)
// 'fScene' should be the SDF evaluator 'float distance = fScene(vec3 pos)''  
// 'eps' is the distance to compare points around the location 'p' 
// a smaller eps gives sharper edges, but it should be large enough to overcome sampling error
// in theory, the gradient magnitude of an SDF should everywhere = 1, 
// but in practice this isn’t always held, so need to normalize() the result
vec3 normal4(in vec3 p, float eps) {
  vec2 e = vec2(-eps, eps);
  // tetrahedral points
  float t1 = DE(p + e.yxx), t2 = DE(p + e.xxy), t3 = DE(p + e.xyx), t4 = DE(p + e.yyy); 
 	vec3 n = (e.yxx*t1 + e.xxy*t2 + e.xyx*t3 + e.yyy*t4);
 	// normalize for a consistent SDF:
 	//return n / (4.*eps*eps);
 	// otherwise:
 	return normalize(n);
}

// p is the vec3 position of the surface at the fragment.
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

	outColor = vec4(1.);

	vec3 rd = normalize(v_raydir);
	vec3 ro = v_raypos;

	float scale = v_bounds.w;

	// TRACE:

	#define STEPS 64
	#define EPS 0.003
	#define FAR 3.0
	vec3 p = ro;
	float t = 0.;
	float stepsize = 1.;
	int step = 0;
	float d = 0.;
	int contact = 0;
	for (; step < STEPS; step++) {
		d = DE(p);
		if (abs(d) < EPS) {
			contact++;
			break;
		}
		t += d * stepsize;
		p = ro + t*rd;
		if (t >= FAR) break;

	}
	float glow = float(step)/float(STEPS);

	if (contact > 0) {

		// generate the tangent-space matrix TBN:
		//vec3 denormTangent = dFdx(v_texCoord.y)*dFdy(v_world.xyz)-dFdx(v_world.xyz)*dFdy(v_texCoord.y);
		vec3 denormTangent = rd;
		//vec3 N = normalize(v_normal.xyz);
		vec3 N = normal4(p, EPS );
		vec3 T = normalize(denormTangent-N*dot(N,denormTangent));
		vec3 B = cross(N,T);
		mat3 TBN = mat3(T, B, N);

		// get a texcoord from the surface
		// ideally, the sdf itself would return a texcoord
		// a lazy way is to use the object normal
		//vec2 tc = N.xy*0.5+0.5;
		// another is to normalize p
		vec2 tc = normalize(p).xy*0.5+0.5;
		//vec2 tc = v_texCoord;

		// project the normal to the cube surface?

		// this normal is in object-space
		
		vec3 normalmap = texture( u_normal_tex, tc ).xyz * 2.0 - 1.0 ;
		vec3 normal = normalize(TBN * normalmap);

		// make it world-space:
		normal = quat_rotate(v_quat, normal);
		
		vec3 albedo = v_color.rgb * pow(texture(u_color_tex, tc).xyz, vec3(2.2));
		float opacity = 1.;

		// this is the cube vertex, not the actual intersection point:
		vec3 worldpos = v_world.xyz;
		float distance = v_world.w;
		// this is more accurate, but probably unnecessary:
		// worldpos = v_pos.xyz + p * v_pos.w;
		// //distance += <some function of t


		float metalness = texture(u_metalness_tex, tc).r;
		float roughness = texture(u_roughness_tex, tc).r;
		float ao = 1.;
		float emissive = 0.;
		
		outColor = render(
			vec4(albedo, 1.), 
			vec4(normal, 1.), 
			vec4(worldpos, distance), 
			vec4(metalness, roughness, ao, emissive));

		//outColor.rgb = rd;

		// pick some vector orthogonal to 
		//outColor.rgb = vec3(albedo);

		// if you need accurate depth clipping -- there's a performance cost though
		//gl_FragDepth = computeDepth( worldpos.xyz, v_viewprojmatrix );
	} else {
		outColor = vec4(0.1);
		discard;
	}
}
`);
// create a VAO from a basic geometry and shader
let geom = glutils.makeCube({ min:-1, max:1, div: 8 })
let cube = glutils.createVao(gl, geom, cubesprogram.id);

// create a VBO & friendly interface for the instances:
// TODO: could perhaps derive the fields from the vertex shader GLSL?
let cubes = glutils.createInstances(gl, [
	// .xyz is the world space centre of the shape
	{ name:"i_pos", components:4 },
	{ name:"i_quat", components:4 },
	// .xyz is the bounding box size
	// default maxiumum is (1,1,1) (meaning -1..1 in each axis)
	// this is helpful to clip a shape if you don't need a full cube
	// .w is the uniform scale factor
	{ name:"i_bounds", components:4 }, 
], 500)

// the .instances provides a convenient interface to the underlying arraybuffer
cubes.instances.forEach(obj => {
	// each field is exposed as a corresponding typedarray view
	// making it easy to use other libraries such as gl-matrix
	// this is all writing into one contiguous block of binary memory for all instances (fast)
	vec4.set(obj.i_pos, 
		world_min[0] + Math.random()*(world_max[0]-world_min[0]),
		world_min[1] + Math.random()*(world_max[1]-world_min[1]),
		world_min[2] + Math.random()*(world_max[2]-world_min[2]),
		1
	);
	//quat.set(obj.i_quat, 0, 0, 0, 1);
	quat.random(obj.i_quat);

	vec4.set(obj.i_bounds, 1, 1, 1., 0.1)
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
	//if(wsize) console.log("FB size: "+wsize.width+', '+wsize.height);

	// Compute the matrix
	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	let modelmatrix = mat4.create();
	let ta = t*Math.PI*1/10;
	let r = 0.3;
	vec3.set(camera_pos, r*Math.cos(ta), camera_at[1], r*Math.sin(ta));

	mat4.lookAt(viewmatrix, camera_pos, camera_at, [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], 0.01, 10);


	// pick a random instance:
	cubes.instances.forEach((obj, i) => {
		if (i == 0) {
			vec3.copy(obj.i_pos, camera_at);
			let rot = quat.fromEuler(quat.create(), 30*Math.cos(t), 0, 0);
			//quat.mul(obj.i_quat, rot, obj.i_quat);
			quat.copy(obj.i_quat, rot);

			//quat.slerp(obj.i_quat, obj.i_quat, quat.random(quat.create()), 0.01);
		} else {
			// move:
			const vel = [0., 0, 0.01];
			quat.add(obj.i_pos, obj.i_pos, glutils.quat_rotate(vel, obj.i_quat, vel));
			// bound:
			//vec3.max(obj.i_pos, obj.i_pos, world_min);
			//vec3.min(obj.i_pos, obj.i_pos, world_max);
			vec3_bound(obj.i_pos, obj.i_pos, world_min, world_max)
			// change its orientation:
			quat.slerp(obj.i_quat, obj.i_quat, quat.random(quat.create()), Math.random()/25);
		}
	})
	
	// submit to GPU:
	cubes.bind().submit().unbind()

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.enable(gl.DEPTH_TEST)
	gl.depthMask(true)
	gl.clearColor(0., 0., 0., 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	roughness_tex.bind(3)
	metalness_tex.bind(2)
	normal_tex.bind(1)
	color_tex.bind(0)


	cubesprogram.begin();
	cubesprogram.uniform("u_modelmatrix", modelmatrix);
	cubesprogram.uniform("u_viewmatrix", viewmatrix);
	cubesprogram.uniform("u_projmatrix", projmatrix);
	cubesprogram.uniform("u_roughness_tex", 3);
	cubesprogram.uniform("u_metalness_tex", 2);
	cubesprogram.uniform("u_normal_tex", 1);
	cubesprogram.uniform("u_color_tex", 0);
	cubesprogram.uniform("u_camera_pos", camera_pos);
	cubesprogram.uniform("u_light0_pos", light_pos);
	cube.bind().drawInstanced(cubes.count).unbind()
	cubesprogram.end();


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