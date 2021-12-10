const assert = require("assert"),
	fs = require("fs")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
const glfw = require('./glfw3.js')
const glutils = require('./glutils.js');

const jpeg = require('jpeg-js');
const pnglib = require("pngjs").PNG

function png2tex(gl, pngpath) {
	let img = pnglib.sync.read(fs.readFileSync(pngpath))
	let tex = glutils.createPixelTexture(gl, img.width, img.height)
	tex.data = img.data
	tex.bind().submit()
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
	gl.generateMipmap(gl.TEXTURE_2D);
	tex.unbind();
	return tex
}

function png2tex3(gl, pngpath0, pngpath1, pngpath2) {
	let img0 = pnglib.sync.read(fs.readFileSync(pngpath0))
	let img1 = fs.existsSync(pngpath1) ? pnglib.sync.read(fs.readFileSync(pngpath1)) : null
	let img2 = fs.existsSync(pngpath2) ? pnglib.sync.read(fs.readFileSync(pngpath2)) : null
	let tex = glutils.createPixelTexture(gl, img0.width, img1.height)
	for (let i=0; i<tex.width * tex.height; i++) {
		tex.data[i*4+0] = img0.data[i*4]
		tex.data[i*4+1] = img1 ? img1.data[i*4] : 0
		tex.data[i*4+2] = img2 ? img2.data[i*4] : 0
	}
	tex.bind().submit()
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
	gl.generateMipmap(gl.TEXTURE_2D);
	tex.unbind();
	return tex
}



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

// glfw.setWindowPosCallback(window, function(w, x, y) {
// 	console.log("window moved", w, x, y)
// 	return 1;
// })

// glfw.setMouseButtonCallback(window, function(...args) {
// 	console.log("mouse button", args);
// })

glfw.makeContextCurrent(window);
console.log(gl.glewInit());

//can only be called after window creation!
console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Core Profile?: " + (glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE)==glfw.OPENGL_CORE_PROFILE));

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

let colortex = jpg2tex(gl, 'Metal007_1K_Color.jpg') 


let cubeprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_modelmatrix;
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;
out vec2 v_texCoord;

void main() {
	// Multiply the position by the matrix.
	vec3 vertex = a_position.xyz;
	gl_Position = u_projmatrix * u_viewmatrix * u_modelmatrix * vec4(vertex, 1);
	v_texCoord = a_texCoord;

	// if using gl.POINTS:
	gl_PointSize = 10.;
}
`,
`#version 330
precision mediump float;
uniform sampler2D u_tex;

in vec2 v_texCoord;
out vec4 outColor;

void main() {
	outColor = vec4(v_texCoord, 0., 1.);
	outColor = texture(u_tex, v_texCoord);
}
`);
let cube_geom = glutils.makeCube({
	min: [0,-1,-1],
	max: [1, 2, 2],
	div: [1, 3, 3]
})
let cube = glutils.createVao(gl, cube_geom, cubeprogram.id);



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
	mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], 0.01, 30);

	//mat4.identity(modelmatrix);
	let axis = vec3.fromValues(Math.sin(t), 1., 0.);
	vec3.normalize(axis, axis);
	mat4.rotate(modelmatrix, modelmatrix, t, axis)

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.enable(gl.DEPTH_TEST)

	colortex.bind()
	cubeprogram.begin();
	cubeprogram.uniform("u_modelmatrix", modelmatrix);
	cubeprogram.uniform("u_viewmatrix", viewmatrix);
	cubeprogram.uniform("u_projmatrix", projmatrix);
	cubeprogram.uniform("u_tex", 0);
	//cube.bind().drawPoints().unbind();
	cube.bind().draw().unbind();
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