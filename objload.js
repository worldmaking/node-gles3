
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


let tetraOBJ = `
v -0.500000 -0.707107 -0.866025
v -0.500000 -0.707107 0.866025
v 1.000000 -0.707107 -0.000000
v 0.000000 0.707107 0.000000
vt 0.000000 0.000000
vt 0.500000 0.000000
vt 1.000000 0.000000
vt 0.250000 0.375000
vt 0.750000 0.375000
vt 0.500000 0.750000
vn 0.942809 -0.333333 0.000000
vn -0.471405 -0.333333 0.816497
vn -0.471405 -0.333333 -0.816497
vn 0.000000 1.000000 0.000000
usemtl initialShadingGroup
f 3/4/1 2/2/2 1/5/3
f 3/1/1 4/2/4 2/4/2
f 4/4/4 3/5/1 1/6/3
f 2/5/2 4/2/4 1/3/3
`;

let geomprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_modelmatrix;
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;
out vec4 v_color;

void main() {
	// Multiply the position by the matrix.
	vec3 vertex = a_position.xyz;
	gl_Position = u_projmatrix * u_viewmatrix * u_modelmatrix * vec4(vertex, 1);

	v_color = vec4(a_normal*0.25+0.25, 1.);
	v_color += vec4(a_texCoord*0.5, 0., 1.);
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
let geom = glutils.createVao(gl, glutils.geomFromOBJ(tetraOBJ), geomprogram.id);

let t = glfw.getTime();
let fps = 60;
while(!glfw.windowShouldClose(window) && !glfw.getKey(window, glfw.KEY_ESCAPE)) {
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
	mat4.lookAt(viewmatrix, [0, 0, 3], [0, 0, 0], [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI*3/2, dim[0]/dim[1], 0.01, 10);

	//mat4.identity(modelmatrix);
	let axis = vec3.fromValues(Math.sin(t), 1., 0.);
	vec3.normalize(axis, axis);
	mat4.rotate(modelmatrix, modelmatrix, t, axis)
	let s = 1
	mat4.scale(modelmatrix, modelmatrix, vec3.fromValues(s,s,s));



	gl.viewport(0, 0, dim[0], dim[1]);
	gl.enable(gl.DEPTH_TEST)
	gl.depthMask(true)
	gl.clearColor(0.2, 0.2, 0.2, 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	
	geomprogram.begin();
	geomprogram.uniform("u_modelmatrix", modelmatrix);
	geomprogram.uniform("u_viewmatrix", viewmatrix);
	geomprogram.uniform("u_projmatrix", projmatrix);
	geom.bind().draw().unbind();
	geomprogram.end();

	// Swap buffers
	glfw.swapBuffers(window);
	glfw.pollEvents();
}

// Close OpenGL window and terminate GLFW
glfw.destroyWindow(window);
glfw.terminate();

process.exit(0);
