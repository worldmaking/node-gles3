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

glfw.setWindowPosCallback(window, function(w, x, y) {
	console.log("window moved", w, x, y)
	return 1;
})

glfw.setMouseButtonCallback(window, function(...args) {
	console.log("mouse button", args);
})

glfw.makeContextCurrent(window);
console.log(gl.glewInit());

//can only be called after window creation!
console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Core Profile?: " + (glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE)==glfw.OPENGL_CORE_PROFILE));

// Enable vertical sync (on cards that support it)
glfw.swapInterval(1); // 0 for vsync off

let cubeprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_modelmatrix;
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;

// instanced variable:
in vec3 i_pos;

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;
out vec4 v_color;


void main() {
	// Multiply the position by the matrix.
	vec4 vertex = u_modelmatrix * vec4(a_position, 1.);
	vertex.xyz += i_pos;
	gl_Position = u_projmatrix * u_viewmatrix * vertex;

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
let cube = glutils.createVao(gl, glutils.makeCube({ min:-0.1, max:0.1, div: 8 }), cubeprogram.id);

// instancing:
let cubeInstanceFields = [
	{ 
		name: "i_pos",
		components: 4,
		type: gl.FLOAT,
		byteoffset: 0*4 // *4 for float32
	},
]
let cubeInstanceByteStride = cubeInstanceFields[cubeInstanceFields.length-1].byteoffset + cubeInstanceFields[cubeInstanceFields.length-1].components*4 // *4 for float32
let cubeInstanceStride = cubeInstanceByteStride / 4; // 4 bytes per float
// create some instances:
let cubeInstanceTotal = 500;
let cubeInstanceData = new Float32Array(cubeInstanceStride * cubeInstanceTotal)

// a friendlier JS interface to the underlying data:
let cubeInstances = []
// iterate over each instance
for (let i=0; i<cubeInstanceTotal; i++) {
	let b = i*cubeInstanceByteStride;
	// make a  interface for this:
	let obj = {
		index: i,
		byteoffset: b,
	}
	for (let i in cubeInstanceFields) {
		let field = cubeInstanceFields[i];
		obj[field.name] = new Float32Array(cubeInstanceData.buffer, b + field.byteoffset, field.components)
	}
	cubeInstances = obj;

	obj.i_pos[0] = (Math.random() - 0.5) * 5;
	obj.i_pos[1] = (Math.random() - 0.5) * 5;
	obj.i_pos[2] = (Math.random() - 0.5) * 5;
}
let cubeInstanceBuffer = gl.createBuffer()
gl.bindBuffer(gl.ARRAY_BUFFER, cubeInstanceBuffer)
gl.bufferData(gl.ARRAY_BUFFER, cubeInstanceData, gl.DYNAMIC_DRAW)
cube.bind().setAttributes(cubeInstanceBuffer, cubeInstanceByteStride, cubeInstanceFields, true).unbind()

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

	
	
	// Compute the matrix
	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	let modelmatrix = mat4.create();
	mat4.lookAt(viewmatrix, [0, 0, 3], [0, 0, 0], [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], 0.01, 10);

	//mat4.identity(modelmatrix);
	let axis = vec3.fromValues(Math.sin(t), 1., 0.);
	vec3.normalize(axis, axis);
	mat4.rotate(modelmatrix, modelmatrix, t, axis)

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clearColor(0.2, 0.2, 0.2, 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.enable(gl.DEPTH_TEST)

	cubeprogram.begin();
	cubeprogram.uniform("u_modelmatrix", modelmatrix);
	cubeprogram.uniform("u_viewmatrix", viewmatrix);
	cubeprogram.uniform("u_projmatrix", projmatrix);
	//cube.bind().draw().unbind();
	cube.bind().drawInstanced(cubeInstanceTotal).unbind()
	cubeprogram.end();

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