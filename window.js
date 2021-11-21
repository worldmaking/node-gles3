/*
	It would be nice to have a general abstraction over the common window needs, e.g.
	- multiwindow
	- fullscreen
	- UI (mouse, key)

	...as well as grouping the usual setup stuff

	There should be a shared window manager, 
	- init some things only on the first window
	- iterate windows in the animate()
*/

const gl = require('./gles3.js') 
const glfw = require('./glfw3.js')
const glutils = require('./glutils.js');

{
	// once only
	if (!glfw.init()) {
		console.log("Failed to initialize GLFW");
		process.exit(-1);
	}
	let version = glfw.getVersion();
	console.log('glfw ' + version.major + '.' + version.minor + '.' + version.rev);
	console.log('glfw version-string: ' + glfw.getVersionString());
}

let windows = []

function createWindow(options) {

	options = Object.assign({
		monitor: 0,
		fullscreen: false,
		sync: false,
	}, options)

	glfw.defaultWindowHints();
	glfw.windowHint(glfw.CONTEXT_VERSION_MAJOR, 3);
	glfw.windowHint(glfw.CONTEXT_VERSION_MINOR, 3);
	glfw.windowHint(glfw.OPENGL_FORWARD_COMPAT, 1);
	glfw.windowHint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE);

	let monitors = glfw.getMonitors()
	let monitor = typeof(options.monitor == "number") ? monitors[options.monitor % monitors.length] : options.monitor;
	let mode = glfw.getVideoMode(monitor)
	console.log("monitor", mode)

	if (options.fullscreen) {
		glfw.windowHint(glfw.DECORATED, 0);
	} else {
		glfw.windowHint(glfw.DECORATED, 1);
	}

	let window = glfw.createWindow(mode.width/2, mode.height/2, "Test");
	if (!window) {
		console.log("Failed to open GLFW window");
		glfw.terminate();
		process.exit(-1);
	}

	function fullscreen(bool, monitor = monitor) {
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

	fullscreen(options.fullscreen, monitor)

	// Enable vertical sync (on cards that support it)
	// NOTE: per issue https://github.com/glfw/glfw/issues/1267 this should happen *before* makeContextCurrent
	// but I also seem to need to do it *after* as well
	glfw.swapInterval(options.sync)
	glfw.makeContextCurrent(window)
	glfw.swapInterval(options.sync)

	console.log(gl.glewInit());
	//can only be called after window creation!
	console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Core Profile?: " + (glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE)==glfw.OPENGL_CORE_PROFILE));
	

	return {
		window,
	}
}

let win = createWindow()
console.log(win)

function animate() {
	glfw.pollEvents();
	for (let o of windows) {
		if (glfw.windowShouldClose(o.window) || glfw.getKey(o.window, glfw.KEY_ESCAPE)) {
			return;
		}
	}

	// insert simulate() here

	for (let o of windows) {
		glfw.makeContextCurrent(o.window);
		// Get window size (may be different than the requested size)
		let dim = glfw.getFramebufferSize(o.window);

		// insert submit() and draw() here

		// Swap buffers
		glfw.swapBuffers(o.window);
	}

	// at end, so that any errors prevent repeat:
	setImmediate(animate)
}

animate()