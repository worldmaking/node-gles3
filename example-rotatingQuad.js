const assert = require("assert")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
const glfw = require('./glfw3.js')
const glutils = require('./glutils.js');
const Window = require('./window.js');

let win = new Window()


let shape_program = glutils.makeProgram(gl,
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
let shape_geom = glutils.makeQuad({
	min: [0,-1,-1],
	max: [1, 2, 2],
	div: [1, 3, 3]
})
let shape_vao = glutils.createVao(gl, shape_geom, shape_program.id);



// let t = glfw.getTime();
// let fps = 60;

// // function animate() {
// // 	if(glfw.windowShouldClose(window) || glfw.getKey(window, glfw.KEY_ESCAPE)) {
// // 		shutdown();
// // 	} else {
// // 		setImmediate(animate)
// // 	}

// // 	let t1 = glfw.getTime();
// // 	let dt = t1-t;
// // 	fps += 0.1*((1/dt)-fps);
// // 	t = t1;
// // 	glfw.setWindowTitle(window, `fps ${fps}`);
// // 	// Get window size (may be different than the requested size)
// // 	let dim = glfw.getFramebufferSize(window);

	
// // 	// Compute the matrix
// // 	let viewmatrix = mat4.create();
// // 	let projmatrix = mat4.create();
// // 	let modelmatrix = mat4.create();
// // 	mat4.lookAt(viewmatrix, [0, 0, 3], [0, 0, 0], [0, 1, 0]);
// // 	mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], 0.01, 30);

// // 	//mat4.identity(modelmatrix);
// // 	let axis = vec3.fromValues(Math.sin(t), 1., 0.);
// // 	vec3.normalize(axis, axis);
// // 	mat4.rotate(modelmatrix, modelmatrix, t, axis)

// // 	gl.viewport(0, 0, dim[0], dim[1]);
// // 	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

// // 	gl.enable(gl.DEPTH_TEST)

// // 	shape_program.begin();
// // 	shape_program.uniform("u_modelmatrix", modelmatrix);
// // 	shape_program.uniform("u_viewmatrix", viewmatrix);
// // 	shape_program.uniform("u_projmatrix", projmatrix);
// // 	//shape_.bind().drawPoints().unbind();
// // 	shape_.bind().draw().unbind();
// // 	shape_program.end();

// // 	// Swap buffers
// // 	glfw.swapBuffers(window);
// // 	glfw.pollEvents();
	
// // }

// // function shutdown() {
// // 	// Close OpenGL window and terminate GLFW
// // 	glfw.destroyWindow(window);
// // 	glfw.terminate();

// // 	process.exit(0);
// // }

// // animate();



win.draw = function() {

	let { t, dim } = this;

	let f = 0
	gl.clearColor(f, 1-f, 0, 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

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

	shape_program.begin();
		shape_program.uniform("u_modelmatrix", modelmatrix);
		shape_program.uniform("u_viewmatrix", viewmatrix);
		shape_program.uniform("u_projmatrix", projmatrix);
		//shape_vao.bind().drawPoints().unbind();
		shape_vao.bind().draw().unbind();
	shape_program.end();
}

Window.animate()