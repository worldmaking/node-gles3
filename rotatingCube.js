//const glfw = require("node-glfw")
const EventEmitter = require('events');
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


// let emitter = new EventEmitter(); 
// emitter.on('keydown',function(evt) {
// 	console.log("[keydown] ", (evt));
// });
// emitter.on('mousemove',function(evt) {
// 	console.log("[mousemove] "+evt.x+", "+evt.y);
// });
// emitter.on('mousewheel',function(evt) {
// 	console.log("[mousewheel] "+evt.position);
// });
// emitter.on('resize',function(evt){
// 	console.log("[resize] "+evt.width+", "+evt.height);
// });

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

let vert = gl.createShader(gl.VERTEX_SHADER)
let frag = gl.createShader(gl.FRAGMENT_SHADER)
let program = gl.createProgram()

let vertcode = `#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
uniform float u_pixelSize;
in vec3 a_position;
out vec4 v_color;


void main() {
	// Multiply the position by the matrix.
	gl_Position = u_projmatrix * u_viewmatrix * vec4(a_position.xyz, 1);
	if (gl_Position.w > 0.0) {
		gl_PointSize = u_pixelSize / gl_Position.w;
	} else {
		gl_PointSize = 0.0;
	}

	v_color = vec4(1.);
	//v_color = vec4(u_viewmatrix[3].xyz * 0.5 + 0.5, 0.5);
	//v_color = vec4(gl_Position.xyz * 0.5 + 0.5, 0.5);
}
`
let fragcode = `#version 330
precision mediump float;

in vec4 v_color;
out vec4 outColor;

void main() {
	// get normalized -1..1 point coordinate
	vec2 pc = (gl_PointCoord - 0.5) * 2.0;
	// convert to distance:
	float dist = max(0., 1.0 - length(pc));
	// paint
  	outColor = vec4(dist) * v_color;
}
`
gl.shaderSource(vert, vertcode);
gl.compileShader(vert);
if (!gl.getShaderParameter(vert, gl.COMPILE_STATUS)) {
    console.log(`Error compiling vertex shader:`);
    console.log(gl.getShaderInfoLog(vert));
}

gl.shaderSource(frag, fragcode);
gl.compileShader(frag);
if (!gl.getShaderParameter(frag, gl.COMPILE_STATUS)) {
    console.log(`Error compiling fragment shader:`);
    console.log(gl.getShaderInfoLog(frag));
}

gl.attachShader(program, vert);
gl.attachShader(program, frag);
gl.linkProgram(program);
if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
    console.log("Error linking shader program:");
    console.log(gl.getProgramInfoLog(program));
}

let positionLocation = gl.getAttribLocation(program, "a_position");
let projmatrixLocation = gl.getUniformLocation(program, "u_projmatrix");
let viewmatrixLocation = gl.getUniformLocation(program, "u_viewmatrix");
let pixelSizeLocation = gl.getUniformLocation(program, "u_pixelSize");
console.log("locations", projmatrixLocation, viewmatrixLocation)

const NUM_POINTS = 1e3;
const points = [];
for (let index = 0; index < NUM_POINTS; index++) {
  points.push((Math.random() - 0.5) * 8);
  points.push((Math.random() - 0.5) * 8);
  points.push((Math.random() - 0.5) * 8);
}

// Create a buffer.
let vertices = new Float32Array(points);
let buffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.DYNAMIC_DRAW);

// Create set of attributes
let vao = gl.createVertexArray();
gl.bindVertexArray(vao);

// tell the position attribute how to pull data out of the current ARRAY_BUFFER
gl.enableVertexAttribArray(positionLocation);
let elementsPerVertex = 3; // for vec2
let normalize = false;
let stride = 0;
let offset = 0;
gl.vertexAttribPointer(positionLocation, elementsPerVertex, gl.FLOAT, normalize, stride, offset);


let cubeprogram = glutils.makeProgram(gl,
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
let cube = glutils.createVao(gl, glutils.makeCube(), cubeprogram.id);



let t = glfw.getTime();
let fps = 60;
while(!glfw.windowShouldClose(window) && !glfw.getKey(window, glfw.KEY_ESCAPE)){

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
	cube.bind().drawPoints().unbind();
	cubeprogram.end();

	gl.enable(gl.BLEND);
	gl.blendFunc(gl.SRC_ALPHA, gl.ONE);
	gl.depthMask(false)

	

    // Tell it to use our program (pair of shaders)
    gl.useProgram(program);

    // Bind the attribute/buffer set we want.
    gl.bindVertexArray(vao);

	// Set the matrix.
	gl.uniform1f(pixelSizeLocation, 10.);
    gl.uniformMatrix4fv(viewmatrixLocation, false, viewmatrix);
    gl.uniformMatrix4fv(projmatrixLocation, false, projmatrix);

    // Draw the geometry.
	let count = NUM_POINTS;
	gl.drawArrays(gl.POINTS, 0, count);

	gl.disable(gl.BLEND);
	gl.depthMask(true)

	// Swap buffers
	glfw.swapBuffers(window);
	glfw.pollEvents();
	
}

// Close OpenGL window and terminate GLFW
glfw.destroyWindow(window);
glfw.terminate();

process.exit(0);
