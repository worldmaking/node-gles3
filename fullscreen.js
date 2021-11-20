const assert = require("assert")
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



let windows = []
let scenes = []

let DISPLAY_SYNC = false
let START_IN_FULLSCREEN = false

// to get the size, we need to know the monitor dimensions:
let monitors = glfw.getMonitors()
monitors.forEach((monitor, i) => {

	glfw.defaultWindowHints();
	glfw.windowHint(glfw.CONTEXT_VERSION_MAJOR, 3);
	glfw.windowHint(glfw.CONTEXT_VERSION_MINOR, 3);
	glfw.windowHint(glfw.OPENGL_FORWARD_COMPAT, 1);
	glfw.windowHint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE);
	if (START_IN_FULLSCREEN) {
		glfw.windowHint(glfw.DECORATED, 0);
	}

	let mode = glfw.getVideoMode(monitor)
	console.log("monitor", i, mode)
	let window = glfw.createWindow(mode.width/2, mode.height/2, "Test");
	if (!window) {
		console.log("Failed to open GLFW window");
		glfw.terminate();
		process.exit(-1);
	}
	windows[i] = window

	function fullscreen(bool, monitor) {
		let pos = glfw.getMonitorPos(monitor)
		let mode = glfw.getVideoMode(monitor)
		if (bool) {
			// go fullscreen
			glfw.setWindowAttrib(window, glfw.DECORATED, 0)
			// enable this if you want the window to always be on top (no alt-tabbing)
			//glfw.setWindowAttrib(window, glfw.FLOATING , 1);
			glfw.setWindowSize(window, mode.width, mode.height)
			glfw.setWindowPos(window, pos[0], pos[1])
			// to hide the mouse:
			glfw.setInputMode(window, glfw.CURSOR, glfw.CURSOR_HIDDEN);

		} else {
			// exit fullscreen
			glfw.setWindowAttrib(window, glfw.DECORATED, 1)
			// enable this if you want the window to always be on top (no alt-tabbing)
			glfw.setWindowAttrib(window, glfw.FLOATING , 0);
			glfw.setWindowSize(window, mode.width/2, mode.height/2)
			glfw.setWindowPos(window, pos[0]+50, pos[1]+50)
			// to show the mouse:
			glfw.setInputMode(window, glfw.CURSOR, glfw.CURSOR_NORMAL);
		}
	}

	fullscreen(START_IN_FULLSCREEN, monitor)

	// Enable vertical sync (on cards that support it)
	// NOTE: per issue https://github.com/glfw/glfw/issues/1267 this should happen *before* makeContextCurrent
	// but I also seem to need to do it *after* as well
	glfw.swapInterval(DISPLAY_SYNC)
	glfw.makeContextCurrent(window);
	glfw.swapInterval(DISPLAY_SYNC)

	console.log(gl.glewInit());
	//can only be called after window creation!
	console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Core Profile?: " + (glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE)==glfw.OPENGL_CORE_PROFILE));
	
	glfw.setKeyCallback(window, (win, key, scan, down, mod) => {
		if (down==1) {
			if (key == 70) { // F
				// toggle fullscreen:
				fullscreen(glfw.getWindowAttrib(window, glfw.DECORATED), monitor);
			}
			console.log(key, down, mod);
		}
	})

	// some GL items, such as VAOs, must be created uniquely per context
	// here we just do everything per context to keep it simple

	let program = glutils.makeProgram(gl,
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
	
		// if using gl.POINTS:
		gl_PointSize = 10.;
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
	let vao = glutils.createVao(gl, glutils.makeCube({
		min: [0,-1,-1],
		max: [1, 2, 2],
		div: [1, 3, 3]
	}), program.id);

	scenes[i] = {
		program, vao
	}
})


let t = glfw.getTime();
let fps = 60;

function animate() {
	
	let t1 = glfw.getTime();
	let dt = t1-t;
	fps += 0.1*((1/dt)-fps);
	t = t1;

	glfw.pollEvents();
	for (let window of windows) {
		if (glfw.windowShouldClose(window) || glfw.getKey(window, glfw.KEY_ESCAPE)) {
			return;
		}
	}

	windows.forEach((window, i) => {
		glfw.makeContextCurrent(window);
		//let wsize = glfw.GetFramebufferSize(win);
		
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

		scenes[i].program.begin()
			.uniform("u_modelmatrix", modelmatrix)
			.uniform("u_viewmatrix", viewmatrix)
			.uniform("u_projmatrix", projmatrix)
		scenes[i].vao.bind().draw().unbind();
		scenes[i].program.end();

		// Swap buffers
		glfw.swapBuffers(window);
	})
	
	setImmediate(animate)
}

animate();

console.log("press the F key to toggle fullscreen")