//const glfw = require("node-glfw")
const EventEmitter = require('events');
const glfw = require("glfw-raub")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./index.js') 
const glutils = require('./glutils.js');

if (!glfw.init()) {
	console.log("Failed to initialize GLFW");
	process.exit(-1);
}
let version = glfw.getVersion();
console.log('glfw ' + version.major + '.' + version.minor + '.' + version.rev);
console.log('glfw version-string: ' + glfw.getVersionString());
let monitors = glfw.getMonitors()

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
	// Open OpenGL window
	glfw.defaultWindowHints();
	glfw.windowHint(glfw.CONTEXT_VERSION_MAJOR, 3);
	glfw.windowHint(glfw.CONTEXT_VERSION_MINOR, 3);
	glfw.windowHint(glfw.OPENGL_FORWARD_COMPAT, 1);
	glfw.windowHint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE);

	let emitter = new EventEmitter(); 
	emitter.on('keydown',function(evt) {
		console.log("[keydown] ", (evt));
	});
	emitter.on('mousemove',function(evt) {
		console.log("[mousemove] "+evt.x+", "+evt.y);
	});
	emitter.on('mousewheel',function(evt) {
		console.log("[mousewheel] "+evt.position);
	});
	emitter.on('resize',function(evt){
		console.log("[resize] "+evt.width+", "+evt.height);
	});

	glfw.windowHint(glfw.RESIZABLE, 1);
	glfw.windowHint(glfw.VISIBLE, 1);
	glfw.windowHint(glfw.DECORATED, 1);
	glfw.windowHint(glfw.RED_BITS, 8);
	glfw.windowHint(glfw.GREEN_BITS, 8);
	glfw.windowHint(glfw.BLUE_BITS, 8);
	glfw.windowHint(glfw.DEPTH_BITS, 24);
	glfw.windowHint(glfw.REFRESH_RATE, 0);

	let window=glfw.createWindow(width, height, { emit: (t, e) => emitter.emit(t, e) }, title); //, monitors.length-1);
	if (!window) {
		console.log("Failed to open glfw window");
		glfw.terminate();
		process.exit(-1);
	}
	glfw.makeContextCurrent(window);
	console.log(gl.glewInit());
	
	glfw.setWindowTitle(window, title);
	glfw.setWindowPos(window, x, y);
	glfw.swapInterval(0); // Disable VSync (we want to get as high FPS as possible!)

	//can only be called after window creation!
	console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Profile: " + glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE));


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
	glfw.pollEvents();
	for (let win of windows) {
		if (glfw.windowShouldClose(win)) {
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

		glfw.makeContextCurrent(win);
		//let wsize = glfw.GetFramebufferSize(win);
		
		gl.clearColor(f, 1-f, 0, 1);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
		
		// Swap buffers
		glfw.swapBuffers(win);
	}
}

update();