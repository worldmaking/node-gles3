const glfw = require("node-glfw")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./index.js') 
const glutils = require('./glutils.js');

if (!glfw.Init()) {
	console.log("Failed to initialize glfw");
	process.exit(-1);
}
let version = glfw.GetVersion();
console.log('glfw ' + version.major + '.' + version.minor + '.' + version.rev);
console.log('glfw version-string: ' + glfw.GetVersionString());
let monitors = glfw.GetMonitors()

const requestAnimationFrame = function(callback, delay=1000/60) {
	let t0 = process.hrtime();
	let timer = ()=>{
		let dt = process.hrtime(t0)
		// dt (in ms):
		let ms = (dt[0]*1e9 + dt[1]) * 1e-6;
		if (ms > delay) {
			callback();
		} else {
			setImmediate(timer);
		}
	}
	timer();
}


function createWindow(title="", width=640, height=480, x=30, y=30) {

	// Open OpenGL window
	glfw.DefaultWindowHints();
	glfw.WindowHint(glfw.CONTEXT_VERSION_MAJOR, 3);
	glfw.WindowHint(glfw.CONTEXT_VERSION_MINOR, 3);
	glfw.WindowHint(glfw.OPENGL_FORWARD_COMPAT, 1);
	glfw.WindowHint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE);

	glfw.WindowHint(glfw.RESIZABLE, 1);
	glfw.WindowHint(glfw.VISIBLE, 1);
	glfw.WindowHint(glfw.DECORATED, 1);
	glfw.WindowHint(glfw.RED_BITS, 8);
	glfw.WindowHint(glfw.GREEN_BITS, 8);
	glfw.WindowHint(glfw.BLUE_BITS, 8);
	glfw.WindowHint(glfw.DEPTH_BITS, 24);
	glfw.WindowHint(glfw.REFRESH_RATE, 0);

	let window=glfw.CreateWindow(width, height, title); //, monitors.length-1);
	if (!window) {
		console.log("Failed to open glfw window");
		glfw.Terminate();
		process.exit(-1);
	}
	glfw.MakeContextCurrent(window);
	
	glfw.SetWindowTitle(window, title);
	glfw.SetWindowPos(window, x, y);
	glfw.SwapInterval(0); // Disable VSync (we want to get as high FPS as possible!)

	console.log('GL ' + glfw.GetWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.GetWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.GetWindowAttrib(window, glfw.CONTEXT_REVISION) + " Profile: " + glfw.GetWindowAttrib(window, glfw.OPENGL_PROFILE));

	return window;
}

let windows = [
	createWindow('win0', 720, 480, 30, 30),
	createWindow('win1', 720, 480, 750, 30),
	createWindow('win2', 720, 480, 30, 550),
	createWindow('win3', 720, 480, 750, 550),
];


let t0 = process.hrtime();
let framecount = 0;

function update() {
	glfw.PollEvents();
	for (let win of windows) {
		if (glfw.WindowShouldClose(win)) {
			return;
		}
	}

	let dt = process.hrtime(t0);
	framecount++;
	let elapsed_ms = (dt[0]*1e9 + dt[1]) * 1e-6;
	let fps = framecount*1000/(elapsed_ms);

	if (Math.random() < 0.01) console.log("fps", fps);

	requestAnimationFrame(update, 1);

	for (let i in windows) {
		let win = windows[i]
		let f = i/(windows.length-1)

		glfw.MakeContextCurrent(win);
		//let wsize = glfw.GetFramebufferSize(win);
		
		gl.clearColor(f, 1-f, 0, 1);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
		
		// Swap buffers
		glfw.SwapBuffers(win);
	}
}

update();