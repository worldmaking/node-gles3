
const rs2 = require('bindings')('realsense.node');

const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
const glfw = require('./glfw3.js')
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

let cloudprogram = glutils.makeProgram(gl, `#version 330
uniform mat4 u_modelmatrix;
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
uniform float u_pixelsize;
in vec3 a_position;
out vec4 v_color;

void main() {
	// Multiply the position by the matrix.
	gl_Position = u_projmatrix * u_viewmatrix * u_modelmatrix * vec4(a_position.xyz, 1);
	if (gl_Position.w > 0.0) {
		gl_PointSize = u_pixelsize / gl_Position.w;
	} else {
		gl_PointSize = 0.0;
	}

	v_color = vec4(1.);
	//v_color = vec4(u_viewmatrix[3].xyz * 0.5 + 0.5, 0.5);
	//v_color = vec4(gl_Position.xyz * 0.5 + 0.5, 0.5);
}
`, 
`#version 330
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
`);


let pipe = new rs2.Pipeline().start()
pipe.grab(true) // true means wait for a result


const NUM_POINTS = 640 * 480 // = 307200
let vertices = pipe.vertices; //new Float32Array(NUM_POINTS * 3)

let points = glutils.createVao(gl, {
	vertices: vertices,
}, cloudprogram.id);

for (let i=0; i<vertices.length/3; i++) {
	// vec3_random_bound(points.geom.vertices.subarray(i*3), world_min, world_max)
	// points.geom.vertices[i*3+1] = 0
	let r = Math.random()*Math.random()*2
	let a = Math.random()*2*Math.PI
	vec3.set(points.geom.vertices.subarray(i*3), 
		r*Math.cos(a),
		Math.random() * 2, 
		r*Math.sin(a)
	);
}

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

	if (pipe.grab()) points.bind().submit()

	// // update scene:
	// for (let i=0; i<NUM_POINTS/10; i++) {
	// 	let idx = Math.floor(Math.random() * vertices.length);
	// 	vertices[idx] += (Math.random()-0.5) * 0.1;
	// }
	// // update GPU buffers:
	// gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
	// gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.DYNAMIC_DRAW);

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clearColor(0., 0., 0., 1);
	gl.clear(gl.COLOR_BUFFER_BIT);

	gl.enable(gl.BLEND);
	gl.blendFunc(gl.SRC_ALPHA, gl.ONE);

	// //gl.enable(gl.PROGRAM_POINT_SIZE);  not needed gles3?

	// Compute the matrix
	// let viewmatrix = mat4.create();
	// let projmatrix = mat4.create();
	// mat4.lookAt(viewmatrix, [0, 0, 1], [0, 0, 0], [0, 1, 0]);
	// mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], 0.01, 10);

	// Compute the matrix
	let h = 1
	let a = t/60
	let r = 2
	let z = r*Math.cos(a*Math.PI*2), x = r*Math.sin(a*Math.PI*2)
	let viewmatrix = mat4.create();
	mat4.lookAt(viewmatrix, [x, h, z], [0, h, 0], [0, 1, 0]);

	let projmatrix = mat4.create();
	mat4.perspective(projmatrix, Math.PI * 0.7, dim[0]/dim[1], 0.05, 50);

	let modelmatrix = mat4.create();
	mat4.translate(modelmatrix, modelmatrix, [0, 2.65, 0])
	mat4.rotateX(modelmatrix, modelmatrix, 0.5*Math.PI)


	cloudprogram.begin()
	cloudprogram.uniform("u_modelmatrix", modelmatrix);
	cloudprogram.uniform("u_viewmatrix", viewmatrix);
	cloudprogram.uniform("u_projmatrix", projmatrix);
	cloudprogram.uniform("u_pixelsize", dim[1] / 500)
	points.bind().drawPoints(NUM_POINTS).unbind()
	cloudprogram.end()

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
