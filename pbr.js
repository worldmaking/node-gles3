const assert = require("assert")
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

let window = glfw.createWindow(720, 480, "Test");
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
glfw.setWindowPos(window, 25, 25)

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

let color_tex = jpg2tex(gl, 'Metal007_1K_Color.jpg') 
let normal_tex = jpg2tex(gl, 'Metal007_1K_Normal.jpg') 
let metalness_tex = jpg2tex(gl, 'Metal007_1K_Metalness.jpg') 
let roughness_tex = jpg2tex(gl, 'Metal007_1K_Roughness.jpg') 

let cubesprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_modelmatrix;
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;


// instanced variable:
in vec4 i_pos;
in vec4 i_quat;

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;
out vec4 v_color;
out vec4 v_normal;
out vec4 v_world;
out vec2 v_texCoord;

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

void main() {
	// Multiply the position by the matrix.
	vec4 vertex = vec4(a_position, 1.);
	vertex = quat_rotate(i_quat, vertex);
	vertex.xyz += i_pos.xyz;

	//gl_Position = u_projmatrix * u_viewmatrix * vertex;
	vec4 world = u_modelmatrix * vertex;
	vec4 view = u_viewmatrix * world;
	gl_Position = u_projmatrix * view;
	
	v_world = vec4(world.xyz, length(view.xyz));
	v_normal = vec4(mat3(u_modelmatrix) * quat_rotate(i_quat, a_normal), length(view.xyz));
	v_color = vec4(1);
	v_texCoord = a_texCoord;
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

in vec4 v_color;
in vec4 v_normal;
in vec4 v_world;
in vec2 v_texCoord;
out vec4 outColor;


const float PI = 3.14159265359;
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
		vec3 light_pos = vec3(cos(a), 1.7, sin(a));
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

	vec3 ambient    = (kAD * diffuse + specular) * ao; 
	color += ambient;

	// // HDR tonemapping
    // color = color / (color + vec3(1.0));
    // // gamma correct
    // color = pow(color, vec3(1.0/2.2)); 

	//return vec4(albedo, 1.0 );
	// return vec4(normal, 1.0 );
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
	// return vec4(diffuse, 1.0 );
	// return vec4(prefilteredColor, 1.0 );
	// return vec4(specular, 1.0 );
	// return vec4(ambient, 1.0 );

	return vec4(vec3(color), 1.);
}

void main() {

	outColor = vec4(1.);

	// // generate the tangent-space matrix TBN:
	vec3 denormTangent = dFdx(v_texCoord.y)*dFdy(v_world.xyz)-dFdx(v_world.xyz)*dFdy(v_texCoord.y);
	vec3 N = normalize(v_normal.xyz);
	vec3 T = normalize(denormTangent-N*dot(N,denormTangent));
	vec3 B = cross(N,T);
	mat3 TBN = mat3(T, B, N);
	
	vec3 normalmap = texture( u_normal_tex, v_texCoord ).xyz * 2.0 - 1.0 ;
	
	vec3 albedo = v_color.rgb * pow(texture(u_color_tex, v_texCoord).xyz, vec3(2.2));
	float opacity = 1.;

	vec3 worldpos = v_world.xyz;
	float distance = v_world.w;

	vec3 normal = normalize(TBN * normalmap);

	float metalness = texture(u_metalness_tex, v_texCoord).r;
	float roughness = texture(u_roughness_tex, v_texCoord).r;
	float ao = 1.;
	float emissive = 0.;
	
	outColor = render(
		vec4(albedo, 1.), 
		vec4(normal, v_normal.w), 
		vec4(worldpos, distance), 
		vec4(metalness, roughness, ao, emissive));
}
`);
// create a VAO from a basic geometry and shader
let cuberadius = 0.1;
let cube = glutils.createVao(gl, glutils.makeCube({ min:-cuberadius, max:cuberadius, div: 8 }), cubesprogram.id);

// create a VBO & friendly interface for the instances:
// TODO: could perhaps derive the fields from the vertex shader GLSL?
let cubes = glutils.createInstances(gl, [
	{ name:"i_pos", components:4 },
	{ name:"i_quat", components:4 },
], 100)

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
	quat.set(obj.i_quat, 0, 0, 0, 1);
	//quat.random(obj.i_quat);
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
	let ta = t*Math.PI*2/10;
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
			const vel = [0.01, 0, 0];
			quat.add(obj.i_pos, obj.i_pos, glutils.quat_rotate(vel, obj.i_quat, vel));
			// bound:
			vec3.max(obj.i_pos, obj.i_pos, world_min);
			vec3.min(obj.i_pos, obj.i_pos, world_max);
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