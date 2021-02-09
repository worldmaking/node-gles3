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

let world_min = [-2, -2, -2];
let world_max = [+2, +2, +2];

let color_tex = jpg2tex(gl, 'Metal007_1K_Color.jpg') 
let normal_tex = jpg2tex(gl, 'Metal007_1K_Normal.jpg') 
let metalness_tex = jpg2tex(gl, 'Metal007_1K_Metalness.jpg') 
let roughness_tex = jpg2tex(gl, 'Metal007_1K_Roughness.jpg') 

/*
	g-buffer layers:
	0: basecolor/albedo/diffuse RGB + opacity A
	1: normal XYZ + distance W
	2: worldpos XYZ + distance W


	Typical g-buffer layers:

	For PBR:
	metallic (float)
	roughness (float)
	AO (float)
	height (float)
	emissive (float / RGB?)

	Other:
	texcoords (ST / STU)?

*/

let quadprogram = glutils.makeProgram(gl,
`#version 330
in vec4 a_position;
in vec2 a_texCoord;
uniform vec2 u_scale;
out vec2 v_texCoord;
void main() {
    gl_Position = a_position;
    vec2 adj = vec2(1, -1);
    gl_Position.xy = (gl_Position.xy + adj)*u_scale.xy - adj;
	v_texCoord = a_texCoord;
}`,
`#version 330
precision mediump float;
uniform sampler2D u_tex0;
uniform sampler2D u_tex1;
uniform sampler2D u_tex2;
uniform sampler2D u_tex3;

uniform sampler2D u_depthtex;

uniform int u_whichtex;

in vec2 v_texCoord;
out vec4 outColor;

void main() {
	//outColor = vec4(v_texCoord, 0., 1.);
	//outColor.rgba = vec4(0.);
	//outColor.rgb = texture(u_tex1, v_texCoord).raa;
	// switch(u_whichtex) {
	// 	case 0: outColor = texture(u_tex0, v_texCoord); break;
	// 	case 1: outColor = texture(u_tex1, v_texCoord); break;
	// 	case 2: outColor = texture(u_tex2, v_texCoord); break;
	// 	//case 3: outColor = texture(u_tex3, v_texCoord); break;
	// 	default: outColor = texture(u_tex1, v_texCoord).aaaa/10.; break;

	// }

	outColor = texture(u_tex3, v_texCoord); break;
}
`);
let quad = glutils.createVao(gl, glutils.makeQuad(), quadprogram.id);


let fbo = glutils.makeGbuffer(gl, 1024, 1024, [
	{ float:false }, 	// basecolor
	{ float:true }, 	// normal
	{ float:true }, 	// worldpos
	{ float:false },	// pbr 
]);

console.log(fbo)
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

in vec4 v_color;
in vec4 v_normal;
in vec4 v_world;
in vec2 v_texCoord;
out vec4 outColor[4];

void main() {

	vec3 tangentNormal = texture(u_normal_tex, v_texCoord).xyz;
	vec3 q1 = dFdx(v_world.xyz);
	vec3 q2 = dFdy(v_world.xyz);
	vec2 st1 = dFdx(v_texCoord.xy);
	vec2 st2 = dFdy(v_texCoord.xy);
	vec3 N = normalize(v_normal.xyz);
	vec3 T = normalize(q1*st2.t - q2*st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	vec3 normal = normalize(TBN * tangentNormal);

	vec3 albedo = pow(texture(u_color_tex, v_texCoord).xyz, vec3(2.2));
	float metalness = texture(u_metalness_tex, v_texCoord).r;
	float roughness = texture(u_roughness_tex, v_texCoord).r;
	float ao = 1.;
	float emissive = 0.;

	outColor[0] = v_color * vec4(albedo, 1.);
	outColor[1] = vec4(normal, v_normal.w);
	outColor[2] = v_world;
	outColor[3] = vec4(metalness, roughness, ao, emissive);
}
`);
// create a VAO from a basic geometry and shader
let cuberadius = 0.05;
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
		(Math.random()-0.5) * 5,
		(Math.random()-0.5) * 5,
		(Math.random()-0.5) * 5,
		1
	);
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
	//if(wsize) console.log("FB size: "+wsize.width+', '+wsize.height);

	// Compute the matrix
	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	let modelmatrix = mat4.create();
	mat4.lookAt(viewmatrix, [0, 0, 0.25], [0, 0, 0], [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], 0.01, 10);

	//mat4.identity(modelmatrix);
	let cubemodelmatrix = mat4.create();
	let axis = vec3.fromValues(Math.sin(t), 1., 0.);
	vec3.normalize(axis, axis);
	mat4.rotate(cubemodelmatrix, cubemodelmatrix, t, axis)

	// pick a random instance:
	cubes.instances.forEach((obj, i) => {
		if (i == 0) {
			vec3.set(obj.i_pos, 0, 0, 0);
			quat.slerp(obj.i_quat, obj.i_quat, quat.random(quat.create()), 0.02);
		} else {
			// move:
			const vel = [0.02, 0, 0];
			quat.add(obj.i_pos, obj.i_pos, glutils.quat_rotate(vel, obj.i_quat, vel));
			// bound:
			vec3.max(obj.i_pos, obj.i_pos, world_min);
			vec3.min(obj.i_pos, obj.i_pos, world_max);
			// change its orientation:
			quat.slerp(obj.i_quat, obj.i_quat, quat.random(quat.create()), Math.random()*0.2);
		}
	})
	
	// submit to GPU:
	cubes.bind().submit().unbind()

	fbo.begin()
	{
		gl.viewport(0, 0, fbo.width, fbo.height);
		gl.enable(gl.DEPTH_TEST)
		gl.depthMask(true)
		gl.clearColor(0, 0, 0, 1);
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
		cube.bind().drawInstanced(cubes.count).unbind()
		cubesprogram.end();
	}
	fbo.end();

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.enable(gl.DEPTH_TEST)
	gl.depthMask(true)
	gl.clearColor(0., 0., 0., 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	// render the cube with the texture we just rendered to
	
	quadprogram.begin();
	for (let i=0; i<fbo.textures.length; i++) {
		gl.activeTexture(gl.TEXTURE0 + i);
		gl.bindTexture(gl.TEXTURE_2D, fbo.textures[i]);
		quadprogram.uniform("u_tex"+i, i);
	}
	quadprogram.uniform("u_scale", 1, 1);
	gl.activeTexture(gl.TEXTURE0 + fbo.textures.length);
	gl.bindTexture(gl.TEXTURE_2D, fbo.depthTexture);
	quadprogram.uniform("u_depthtex", fbo.textures.length)
	quadprogram.uniform("u_whichtex", Math.floor(t % 4));
	quad.bind().draw().unbind();
	quadprogram.end();

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