
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


let window = glfw.createWindow(1024, 1024, "Test");
if (!window) {
	console.log("Failed to open GLFW window");
	glfw.terminate();
	process.exit(-1);
}
glfw.makeContextCurrent(window);
console.log(gl.glewInit());
glfw.setWindowPos(window, 40, 40)

//can only be called after window creation!
console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Profile: " + glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE));

// Enable vertical sync (on cards that support it)
glfw.swapInterval(1); // 0 for vsync off

let cloudprogram = glutils.makeProgram(gl, `#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
uniform float u_pixelsize;
in vec3 a_position;
in vec2 a_texCoord;
out vec4 v_color;

void main() {
	// Multiply the position by the matrix.
	gl_Position = u_projmatrix * u_viewmatrix * vec4(a_position.xyz, 1);
	if (gl_Position.w > 0.0) {
		gl_PointSize = u_pixelsize / gl_Position.w;
	} else {
		gl_PointSize = 0.0;
	}

	v_color = vec4(0.5);
	//v_color = vec4(a_texCoord, 0., 1.);
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
pipe.modelmatrix = mat4.create();
mat4.translate(pipe.modelmatrix, pipe.modelmatrix, [0, 2.65, 0])
mat4.rotateX(pipe.modelmatrix, pipe.modelmatrix, 0.5*Math.PI)
pipe.maxarea = 0.0001
pipe.miny = 0.1
pipe.grab(true) // true means wait for a result


const NUM_POINTS = 640 * 480 // = 307200

let points_geom = {
	vertices: pipe.vertices,
	texCoords: new Float32Array(NUM_POINTS*2)
}

let points = glutils.createVao(gl, points_geom, cloudprogram.id);

for (let i=0; i<NUM_POINTS; i++) {
	let r = Math.random()*Math.random()*2
	let a = Math.random()*2*Math.PI
	vec3.set(points.geom.vertices.subarray(i*3), 
		r*Math.cos(a),
		Math.random() * 2, 
		r*Math.sin(a)
	);

	let col = i % 640, row = Math.floor(i/640)
	let u = (col+0.5) / 640
	let v = (row+0.5) / 480
	points.geom.texCoords[i*2+0] = u
	points.geom.texCoords[i*2+1] = v
}

glutils.ok(gl, "after vao")

let meshprogram = glutils.makeProgram(gl, `#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
uniform float u_pixelsize;
in vec3 a_position;
in vec3 a_normal;
out vec4 v_color;

void main() {
	// Multiply the position by the matrix.
	gl_Position = u_projmatrix * u_viewmatrix * vec4(a_position.xyz, 1);
	if (gl_Position.w > 0.0) {
		gl_PointSize = u_pixelsize / gl_Position.w;
	} else {
		gl_PointSize = 0.0;
	}

	//v_color = vec4(0.4);
	v_color = vec4(a_normal*0.5+0.5, 1.);
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

	outColor = v_color;
}
`);

let mesh_geom = {
	vertices: pipe.vertices,
	normals: pipe.normals,
	indices: pipe.indices
}
let mesh = glutils.createVao(gl, mesh_geom, meshprogram.id);

console.log(pipe.indices[pipe.count-1])

// // let's also create a float texture from this data:
// let htex = glutils.createTexture(gl, {
// 	float: true,
// 	channels: 1,
// 	width: 640,
// 	height: 480,
// 	//data: pipe.depth
// })
// console.log(htex)

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
	let dim = glfw.getFramebufferSize(window);

	
	


	if (pipe.grab(false, 0.0001)) {
		points.bind().submit()
		mesh.bind().submit()
	}
	// Compute the matrix
	let h = 1
	let a = t/60
	let r = 2
	let z = r*Math.cos(a*Math.PI*2), x = r*Math.sin(a*Math.PI*2)
	let viewmatrix = mat4.create();
	mat4.lookAt(viewmatrix, [x, h, z], [0, h, 0], [0, 1, 0]);

	let projmatrix = mat4.create();
	mat4.perspective(projmatrix, Math.PI * 0.7, dim[0]/dim[1], 0.05, 50);

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clearColor(0.1, 0.1, 0.1, 1);
	gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT);

	gl.enable(gl.DEPTH_TEST)
	gl.depthMask(true);

	meshprogram.begin()
		.uniform("u_viewmatrix", viewmatrix)
		.uniform("u_projmatrix", projmatrix)
		.uniform("u_pixelsize", dim[1] / 500)
	mesh.bind().draw(pipe.count).unbind()
	meshprogram.end()

	gl.disable(gl.DEPTH_TEST)
	gl.depthMask(false);

	gl.enable(gl.BLEND);
	//gl.blendFunc(gl.SRC_ALPHA, gl.ONE);
	gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA )

	cloudprogram.begin()
		.uniform("u_viewmatrix", viewmatrix)
		.uniform("u_projmatrix", projmatrix)
		.uniform("u_pixelsize", dim[1] / 2000)
	points.bind().drawPoints().unbind()
	cloudprogram.end()

	gl.disable(gl.BLEND);

	gl.enable(gl.DEPTH_TEST)
	gl.depthMask(true);

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
