const glfw = require("node-glfw")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./index.js') 

if (!glfw.Init()) {
	console.log("Failed to initialize GLFW");
	process.exit(-1);
}

// Open OpenGL window
glfw.DefaultWindowHints();
glfw.WindowHint(glfw.CONTEXT_VERSION_MAJOR, 4);
glfw.WindowHint(glfw.CONTEXT_VERSION_MINOR, 1);
glfw.WindowHint(glfw.OPENGL_FORWARD_COMPAT, 1);
glfw.WindowHint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE);
let window = glfw.CreateWindow(720, 480, "Test");
if (!window) {
	console.log("Failed to open GLFW window");
	glfw.Terminate();
	process.exit(-1);
}
glfw.MakeContextCurrent(window);

// testing events
glfw.events.on('keydown',function(evt) {
	//console.log("[keydown] ", (evt));
});

glfw.events.on('mousemove',function(evt) {
	//console.log("[mousemove] "+evt.x+", "+evt.y);
});

glfw.events.on('mousewheel',function(evt) {
	//console.log("[mousewheel] "+evt.position);
});

glfw.events.on('resize',function(evt){
	//console.log("[resize] "+evt.width+", "+evt.height);
});

// Enable vertical sync (on cards that support it)
glfw.SwapInterval(1); // 0 for vsync off

let vert = gl.createShader(gl.VERTEX_SHADER)
let frag = gl.createShader(gl.FRAGMENT_SHADER)
let program = gl.createProgram()

let vertcode = `#version 150
uniform mat3 u_matrix;
in vec2 a_position;
out vec4 v_color;

void main() {
  // Multiply the position by the matrix.
  gl_Position = vec4((u_matrix * vec3(a_position, 1)).xy, 0, 1);

  // Convert from clipspace to colorspace.
  // Clipspace goes -1.0 to +1.0
  // Colorspace goes from 0.0 to 1.0
  v_color = gl_Position * 0.5 + 0.5;
}
`
let fragcode = `#version 150
precision mediump float;

in vec4 v_color;
out vec4 outColor;

void main() {
  outColor = v_color;
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
let matrixLocation = gl.getUniformLocation(program, "u_matrix");

// Create a buffer.
let vertices = new Float32Array([
	0, -1,
	1,  1,
   -1,  1,
]);
let buffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.DYNAMIC_DRAW);

// Create set of attributes
let vao = gl.createVertexArray();
gl.bindVertexArray(vao);

// tell the position attribute how to pull data out of the current ARRAY_BUFFER
gl.enableVertexAttribArray(positionLocation);
let elementsPerVertex = 2; // for vec2
let normalize = false;
let stride = 0;
let offset = 0;
gl.vertexAttribPointer(positionLocation, elementsPerVertex, gl.FLOAT, normalize, stride, offset);

while(!glfw.WindowShouldClose(window) && !glfw.GetKey(window, glfw.KEY_ESCAPE)) {
	
	// Get window size (may be different than the requested size)
	let dim = glfw.GetFramebufferSize(window);
	//if(wsize) console.log("FB size: "+wsize.width+', '+wsize.height);

	gl.viewport(0, 0, dim.width, dim.height);
	gl.clearColor(0.2, 0.2, 0.2, 1);
	gl.clear(gl.COLOR_BUFFER_BIT);

	// Compute the matrix
	let angleInRadians = glfw.GetTime();
	let matrix = mat3.create();
	mat3.projection(matrix, 2, 2);
	mat3.translate(matrix, matrix, vec2.fromValues(1, 1));
	mat3.rotate(matrix, matrix, angleInRadians);
	mat3.scale(matrix, matrix, vec2.fromValues(1, 1));
	

    // Tell it to use our program (pair of shaders)
    gl.useProgram(program);

    // Bind the attribute/buffer set we want.
    gl.bindVertexArray(vao);

    // Set the matrix.
    gl.uniformMatrix3fv(matrixLocation, false, matrix);

    // Draw the geometry.
    let count = 3;
    gl.drawArrays(gl.TRIANGLES, 0, count);

	// Swap buffers
	glfw.SwapBuffers(window);
	glfw.PollEvents();

}

// Close OpenGL window and terminate GLFW
glfw.DestroyWindow(window);
glfw.Terminate();

process.exit(0);
