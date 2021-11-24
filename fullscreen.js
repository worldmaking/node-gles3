const assert = require("assert")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
const glfw = require('./glfw3.js')
const glutils = require('./glutils.js');

const Window = require("./window.js")

let DISPLAY_SYNC = false
let START_IN_FULLSCREEN = false

let monitors = glfw.getMonitors()
monitors.forEach((monitor, i) => {
	let win = new Window({
		monitor: i,
		fullscreen: START_IN_FULLSCREEN,
		sync: DISPLAY_SYNC,

		onkey(key, scan, down, mod) {
			if (down==1) {
				if (key == 70) { // F
					// toggle fullscreen:
					this.setFullscreen(!this.fullscreen);
				}
				console.log(key, down, mod);
			}
		},
	})

	// some GL items, such as shaders, can be shared between windows
	// some GL items, such as VAOs, must be created uniquely per context
	// here we just do everything per context to keep it simple
	let [ first ] = Window.all
	console.log(Window.all.size, first)

	let program = first.program
	if (!program) program = glutils.makeProgram(gl,
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

	win.program = program
	win.vao = vao


	win.draw = function() {
		let {t, dt, fps, dim} = win;
		glfw.setWindowTitle(win.window, `fps ${fps}`);

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

		win.program.begin()
			.uniform("u_modelmatrix", modelmatrix)
			.uniform("u_viewmatrix", viewmatrix)
			.uniform("u_projmatrix", projmatrix)
			win.vao.bind().draw().unbind();
		win.program.end();
	}
})

Window.animate();

console.log("press the F key to toggle fullscreen")