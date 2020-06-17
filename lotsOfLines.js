const assert = require("assert")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
const glfw = require('./glfw3.js')
const glutils = require('./glutils.js');

/*
	TODO: 
	- another example using screen-aligned ribbon strips instead
*/

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

glfw.windowHint(glfw.SAMPLES, 4)

let window = glfw.createWindow(720, 480, "Test");
if (!window) {
	console.log("Failed to open GLFW window");
	glfw.terminate();
	process.exit(-1);
}
glfw.setWindowPos(window, 32, 32)

glfw.makeContextCurrent(window);
console.log(gl.glewInit());

//can only be called after window creation!
console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Core Profile?: " + (glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE)==glfw.OPENGL_CORE_PROFILE));

// Enable vertical sync (on cards that support it)
glfw.swapInterval(0); // 0 for vsync off
gl.enable(gl.MULTISAMPLE)


let quadprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;

// instanced variable:
in vec4 i_pos;
in vec4 i_quat;

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;
out vec4 v_color;
out vec3 v_normal;

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
	gl_Position = u_projmatrix * u_viewmatrix * vertex;

	v_normal = quat_rotate(i_quat, a_normal);

	v_color = vec4(v_normal*0.25+0.25, 1.);
	v_color += vec4(a_texCoord*0.5, 0., 1.);
}
`,
`#version 330
precision mediump float;

in vec4 v_color;
in vec3 v_normal;
out vec4 outColor;

void main() {
	outColor = v_color;
}
`);
// create a VAO from a basic geometry and shader
let quad = glutils.createVao(gl, glutils.makeQuad({ min:-0.03, max:0.03, div: 8 }), quadprogram.id);

// create a VBO & friendly interface for the instances:
// TODO: could perhaps derive the fields from the vertex shader GLSL?
let quads = glutils.createInstances(gl, [
	{ name:"i_pos", components:4 },
	{ name:"i_quat", components:4 },
], 100)

// the .instances provides a convenient interface to the underlying arraybuffer
quads.instances.forEach(obj => {
	// each field is exposed as a corresponding typedarray view
	// making it easy to use other libraries such as gl-matrix
	// this is all writing into one contiguous block of binary memory for all instances (fast)
	vec4.set(obj.i_pos, 
		(Math.random()-0.5) * 2,
		(Math.random()-0.5) + 1,
		(Math.random()-0.5) * 2,
		1
	);
	quat.random(obj.i_quat);
})
quads.bind().submit().unbind();

// attach these instances to an existing VAO:
quads.attachTo(quad);

let lineprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
uniform float u_stiffness;

// instance variables:
in vec4 i_color;
in vec4 i_quat0;
in vec4 i_quat1;
in vec3 i_pos0;
in vec3 i_pos1;

in float a_position; // not actually used...
in vec2 a_texCoord;

out vec4 v_color;
out float v_t;

vec3 quat_rotate(vec4 q, vec3 v) {
	return v + 2.0*cross(q.xyz, cross(q.xyz, v) + q.w*v);
}
vec4 quat_rotate(vec4 q, vec4 v) {
	return vec4(v.xyz + 2.0*cross(q.xyz, cross(q.xyz, v.xyz) + q.w*v.xyz), v.w);
}

vec3 bezier(float t, vec3 v0, vec3 v1, vec3 v2, vec3 v3) {
	// interp the 3 line segments:
	vec3 v01 = mix(v0, v1, t);
	vec3 v12 = mix(v1, v2, t);
	vec3 v23 = mix(v2, v3, t);
	// interp those:
	vec3 v012 = mix(v01, v12, t);
	vec3 v123 = mix(v12, v23, t);
	// interp those:
	return mix(v012, v123, t);
}

float smootherstep(float edge0, float edge1, float x) {
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	return x*x*x*(x*(x*6 - 15) + 10);
}

void main() {
	// control points:
	float stiffness = u_stiffness;
	vec3 c0 = i_pos0 + quat_rotate(i_quat0, vec3(0., 0., stiffness));
	vec3 c1 = i_pos1 + quat_rotate(i_quat1, vec3(0., 0., stiffness));

	// bias t's distribution toward end points where curvature is greatest
	float t = smoothstep(0., 1., a_texCoord.x);

	// derive point from bezier:
	vec4 vertex = vec4(bezier(t, i_pos0, c0, c1, i_pos1), 1.);
	
	gl_Position = u_projmatrix * u_viewmatrix * vertex;

	// line intensity stronger near end points:
	v_color = i_color;
	v_t = t;

	// it might be nice to estimate the length of the bezier curve, for patterning purposes
	// however there is no analytic solution to this
	// we could estimate the local scaling factor (between t and object space) 
	// by computing two bezier points near the current point and getting the distance
}
`,
`#version 330
precision mediump float;

in vec4 v_color;
in float v_t;
out vec4 outColor;

void main() {
	outColor = v_color;

	// stippling:
	// float stipplerate = 1.; // 1.0
	// float stippleclamp = 0.; 
	// float stipple = 1. - 0.372*smoothstep(stippleclamp, 1.-stippleclamp, abs(sin(3.141592653589793 * v_t * stipplerate)));
	float stipple = smoothstep(0., 1., 0.5+abs(v_t - 0.5));
	outColor *= v_color * stipple;
}
`);
// create a VAO from a basic geometry and shader
let line = glutils.createVao(gl, glutils.makeLine({ min:0, max:1, div: 24 }), lineprogram.id);

// create a VBO & friendly interface for the instances:
// TODO: could perhaps derive the fields from the vertex shader GLSL?
let lines = glutils.createInstances(gl, [
	{ name:"i_color", components:4 },
	{ name:"i_quat0", components:4 },
	{ name:"i_quat1", components:4 },
	{ name:"i_pos0", components:3 },
	{ name:"i_pos1", components:3 },
], quads.count)

// the .instances provides a convenient interface to the underlying arraybuffer
lines.instances.forEach((obj, i) => {
	// pick a color:
	vec4.set(obj.i_color, 0.75, 1, 1, 0.75);
	// pick two quads to connect:
	obj.from = i;
	obj.to = i > 1 ? Math.floor(Math.random()*i) : quads.count-1;
	// the rest of the instance vars are set in the animate() loop
})
lines.bind().submit().unbind();

// attach these instances to an existing VAO:
lines.attachTo(line);


let t = glfw.getTime();
let fps = 60;

function animate() {
	if(glfw.windowShouldClose(window) || glfw.getKey(window, glfw.KEY_ESCAPE)) {
		shutdown();
	} else {
		//setImmediate(animate)
		setTimeout(animate, 10)
	}

	let t1 = glfw.getTime();
	let dt = t1-t;
	fps += 0.1*((1/dt)-fps);
	t = t1;
	glfw.setWindowTitle(window, `fps ${fps}`);
	// Get window size (may be different than the requested size)
	let dim = glfw.getFramebufferSize(window);

	// update scene:
	//quads.instances.forEach((obj, i) => 
	{
		let i = Math.floor(Math.random() * quads.count)
		let obj = quads.instances[i]
		quat.slerp(obj.i_quat, obj.i_quat, quat.random(quat.create()), 0.1);
		quat.normalize(obj.i_quat, obj.i_quat);
	}
	lines.instances.forEach((obj, i) => {
		let a = quads.instances[obj.from]
		let b = quads.instances[obj.to]
		quat.copy(obj.i_quat0, a.i_quat);
		quat.copy(obj.i_quat1, b.i_quat);
		vec3.copy(obj.i_pos0, a.i_pos);
		vec3.copy(obj.i_pos1, b.i_pos);
	}) 
	// submit to GPU:
	quads.bind().submit().unbind()
	lines.bind().submit().unbind()

	// Compute the matrix
	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	let modelmatrix = mat4.create();
	// lookat: out, eye, centre, up
	mat4.lookAt(viewmatrix, [2*Math.sin(t/9), 1, 2*Math.cos(t/9)], [0, 1, 0], [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI/4, dim[0]/dim[1], 0.01, 10);

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.enable(gl.DEPTH_TEST)

	quadprogram.begin();
	quadprogram.uniform("u_viewmatrix", viewmatrix);
	quadprogram.uniform("u_projmatrix", projmatrix);
	quad.bind().drawInstanced(quads.count).unbind()
	quadprogram.end();

	gl.enable(gl.BLEND);
	gl.blendFunc(gl.SRC_ALPHA, gl.ONE);
	gl.depthMask(false)

	lineprogram.begin();
	lineprogram.uniform("u_viewmatrix", viewmatrix);
	lineprogram.uniform("u_projmatrix", projmatrix);
	lineprogram.uniform("u_stiffness", 0.5)
	// consider gl.LINE_STRIP with simpler geometry
	line.bind().drawInstanced(lines.count, gl.LINES).unbind()
	lineprogram.end();

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

setTimeout(animate, 10)