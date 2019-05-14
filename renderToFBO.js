const glfw = require("node-glfw")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./index.js') 
const glutils = require('./glutils.js');

if (!glfw.Init()) {
	console.log("Failed to initialize GLFW");
	process.exit(-1);
}
let version = glfw.GetVersion();
console.log('glfw ' + version.major + '.' + version.minor + '.' + version.rev);
console.log('glfw version-string: ' + glfw.GetVersionString());

// Open OpenGL window
glfw.DefaultWindowHints();
glfw.WindowHint(glfw.CONTEXT_VERSION_MAJOR, 3);
glfw.WindowHint(glfw.CONTEXT_VERSION_MINOR, 3);
glfw.WindowHint(glfw.OPENGL_FORWARD_COMPAT, 1);
glfw.WindowHint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE);
let window = glfw.CreateWindow(720, 480, "Test");
if (!window) {
	console.log("Failed to open GLFW window");
	glfw.Terminate();
	process.exit(-1);
}
glfw.MakeContextCurrent(window);
console.log(gl.glewInit());

//can only be called after window creation!
console.log('GL ' + glfw.GetWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.GetWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.GetWindowAttrib(window, glfw.CONTEXT_REVISION) + " Profile: " + glfw.GetWindowAttrib(window, glfw.OPENGL_PROFILE));


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



let fbo = glutils.makeFboWithDepth(gl)


let quadprogram = glutils.makeProgram(gl,
`#version 330
in vec4 a_position;
in vec2 a_texCoord;
uniform vec2 u_scale;
out vec2 v_texCoord;
void main() {
    gl_Position = a_position;
    vec2 adj = vec2(1, -1);
    gl_Position.xy = (gl_Position.xy + adj)*u_scale.xy - adj;
    v_texCoord = a_texCoord;
}`,
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
let quad = glutils.createVao(gl, glutils.makeQuad(), quadprogram.id);

let cubeprogram = glutils.makeProgram(gl,
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
let cube = glutils.createVao(gl, glutils.makeCube(), cubeprogram.id);

let t = glfw.GetTime();
let fps = 60;
while(!glfw.WindowShouldClose(window) && !glfw.GetKey(window, glfw.KEY_ESCAPE)) {
	let t1 = glfw.GetTime();
	let dt = t1-t;
	fps += 0.1*((1/dt)-fps);
	t = t1;
	glfw.SetWindowTitle(window, `fps ${fps}`);
	// Get window size (may be different than the requested size)
	let dim = glfw.GetFramebufferSize(window);
	//if(wsize) console.log("FB size: "+wsize.width+', '+wsize.height);

	// Compute the matrix
	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	let modelmatrix = mat4.create();
	mat4.lookAt(viewmatrix, [0, 0, 3], [0, 0, 0], [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI/2, dim.width/dim.height, 0.01, 10);

	//mat4.identity(modelmatrix);
	let axis = vec3.fromValues(Math.sin(t), 1., 0.);
	vec3.normalize(axis, axis);
	mat4.rotate(modelmatrix, modelmatrix, t, axis)


	//fbo.begin()
	// render to our targetTexture by binding the framebuffer
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo.id);
	{
		gl.viewport(0, 0, fbo.width, fbo.height);
		gl.enable(gl.DEPTH_TEST)
		gl.depthMask(true)
		gl.clearColor(0, 0, 1, 1);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

		cubeprogram.begin();
		cubeprogram.uniform("u_modelmatrix", modelmatrix);
		cubeprogram.uniform("u_viewmatrix", viewmatrix);
		cubeprogram.uniform("u_projmatrix", projmatrix);
		cube.bind().draw().unbind();
		cubeprogram.end();
	}
	//fbo.end();
	gl.bindFramebuffer(gl.FRAMEBUFFER, null);

	gl.viewport(0, 0, dim.width, dim.height);
	gl.enable(gl.DEPTH_TEST)
	gl.depthMask(true)
	gl.clearColor(0.2, 0.2, 0.2, 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	// render the cube with the texture we just rendered to
    gl.bindTexture(gl.TEXTURE_2D, fbo.colorTexture);
	quadprogram.begin();
	quadprogram.uniform("u_scale", 1, 1);
	quad.bind().draw().unbind();
	quadprogram.end();

	// Swap buffers
	glfw.SwapBuffers(window);
	glfw.PollEvents();
}

// Close OpenGL window and terminate GLFW
glfw.DestroyWindow(window);
glfw.Terminate();

process.exit(0);
