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
glfw.swapInterval(1); // 0 for vsync off


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
let quad = glutils.createVao(gl, glutils.makeQuad({ min:-0.05, max:0.05, div: 8 }), quadprogram.id);

// create a VBO & friendly interface for the instances:
// TODO: could perhaps derive the fields from the vertex shader GLSL?
let quads = glutils.createInstances(gl, [
	{ name:"i_pos", components:4 },
	{ name:"i_quat", components:4 },
], 10)

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

// instanced variable:
in vec4 i_color;
in vec3 i_pos;
in vec3 i_pos1;
in vec4 i_quat;
in vec4 i_quat1;

in float a_position;
in vec3 a_normal;
in vec2 a_texCoord;

out vec4 v_color;

vec3 quat_rotate(vec4 q, vec3 v) {
	return v + 2.0*cross(q.xyz, cross(q.xyz, v) + q.w*v);
}
vec4 quat_rotate(vec4 q, vec4 v) {
	return vec4(v.xyz + 2.0*cross(q.xyz, cross(q.xyz, v.xyz) + q.w*v.xyz), v.w);
}

vec3 bezier(float t, vec3 v0, vec3 v1, vec3 v2, vec3 v3) {
    float t2 = t * t;
    float rt = 1.0 - t;
    float rt2 = rt * rt;
    return (v0*rt2*rt + v1*3.0*t*rt2 + v2*3.0*t2*rt + v3*t2*t);
}

float smootherstep(float edge0, float edge1, float x) {
	x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
	return x*x*x*(x*(x*6 - 15) + 10);
}

void main() {
	// control points:
	float stiffness = u_stiffness;
	vec3 c0 = i_pos + quat_rotate(i_quat, vec3(0., 0., stiffness));
	vec3 c1 = i_pos1 + quat_rotate(i_quat1, vec3(0., 0., stiffness));

	// bias t's distribution toward end points where curvature is greatest
	float t = smoothstep(0., 1., a_texCoord.x);

	// derive point from bezier:
	vec4 vertex = vec4(bezier(t, i_pos, c0, c1, i_pos1), 1.);
	
	gl_Position = u_projmatrix * u_viewmatrix * vertex;

	// line intensity stronger near end points:
	float alpha = clamp(1. - 0.414*sin(3.141592653589793 * t), 0., 1.);
	v_color = i_color * alpha;
}
`,
`#version 330
precision mediump float;

in vec4 v_color;
out vec4 outColor;

void main() {
	outColor = v_color;
}
`);
// create a VAO from a basic geometry and shader
let line = glutils.createVao(gl, glutils.makeLine({ min:0, max:1, div: 24 }), lineprogram.id);

// create a VBO & friendly interface for the instances:
// TODO: could perhaps derive the fields from the vertex shader GLSL?
let lines = glutils.createInstances(gl, [
	{ name:"i_color", components:4 },
	{ name:"i_pos", components:4 },
	{ name:"i_pos1", components:4 },
	{ name:"i_quat", components:4 },
	{ name:"i_quat1", components:4 },
], quads.count)

// the .instances provides a convenient interface to the underlying arraybuffer
lines.instances.forEach((obj, i) => {
	let quad = quads.instances[i]
	let quad1 = quads.instances[(i+1) % quads.count]

	vec4.set(obj.i_color, 1, 1, 1, 1);

	vec3.copy(obj.i_pos, quad.i_pos);
	quat.copy(obj.i_quat, quad.i_quat);

	//vec3.set(obj.i_pos1, 0, 0, 0)
	vec3.copy(obj.i_pos1, quad1.i_pos);
	quat.copy(obj.i_quat1, quad1.i_quat);
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
		setImmediate(animate)
	}

	let t1 = glfw.getTime();
	let dt = t1-t;
	fps += 0.1*((1/dt)-fps);
	t = t1;
	glfw.setWindowTitle(window, `fps ${fps}`);
	// Get window size (may be different than the requested size)
	let dim = glfw.getFramebufferSize(window);

	// update scene:s
	quads.instances.forEach((obj, i) => {
		quat.slerp(obj.i_quat, obj.i_quat, quat.random(quat.create()), 0.01);
		quat.copy(lines.instances[i].i_quat, obj.i_quat);
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

animate();