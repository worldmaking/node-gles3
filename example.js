const glfw = require("node-glfw")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./index.js') 
const glutils = require('./glutils.js');
const assert = require("assert")
const fs = require("fs")




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



let geomsnake = glutils.geomFromOBJ(fs.readFileSync("snake_fat_adjust1.obj", "utf-8"))
let snakeprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;
in vec4 a_orientation;
in vec4 a_color;
in vec3 a_location;
in float a_phase;
in vec3 a_scale;
out vec4 v_color;

//	q must be a normalized quaternion
vec3 quat_rotate(vec4 q, vec3 v) {
	vec4 p = vec4(
				  q.w*v.x + q.y*v.z - q.z*v.y,	// x
				  q.w*v.y + q.z*v.x - q.x*v.z,	// y
				  q.w*v.z + q.x*v.y - q.y*v.x,	// z
				  -q.x*v.x - q.y*v.y - q.z*v.z	// w
				  );
	return vec3(
				p.x*q.w - p.w*q.x + p.z*q.y - p.y*q.z,	// x
				p.y*q.w - p.w*q.y + p.x*q.z - p.z*q.x,	// y
				p.z*q.w - p.w*q.z + p.y*q.x - p.x*q.y	// z
				);
}

void main() {
	// Multiply the position by the matrix.
	vec3 vertex = quat_rotate(a_orientation, (a_position.xyz * a_scale) + a_location.xyz);
	gl_Position = u_projmatrix * u_viewmatrix * vec4(vertex, 1);

	v_color = vec4(a_normal*0.5+0.5, 1.);
	v_color += vec4(a_texCoord, 0., 1.);
	v_color *= a_color;
	v_color *= vec4(a_phase);
	
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
let snake = glutils.createVao(gl, geomsnake, snakeprogram.id);


/*
snake instance data is 16 floats:
typedef struct SnakeSegment {
	glm::quat orientation;
	glm::vec4 color;
	glm::vec3 pos; float phase;
	glm::vec3 scale; float unused;
} SnakeSegment;
*/

const NUM_SNAKE_SEGMENTS = 136;
let snakeInstanceData = new Float32Array(NUM_SNAKE_SEGMENTS * 16)
for (let i=0; i<snakeInstanceData.length; i+=16) {
	// quat:
	let instance = snakeInstanceData.subarray(i, i+16);
	let orientation = instance.subarray(0,4);
	let color = instance.subarray(4,8);
	let pos = instance.subarray(8,11);
	let phase = instance.subarray(11, 12);
	let scale = instance.subarray(12,15);

	quat.set(orientation, Math.random()-0.5, Math.random()-0.5, Math.random()-0.5, Math.random()-0.5)
	quat.normalize(orientation, orientation)
	vec3.set(color, 0.5, 0.5, 0.5);
	pos[0] = (Math.random()-0.5)*8
	pos[1] = (Math.random()-0.5)*8
	pos[2] = (Math.random()-0.5)*8
	phase[0] = Math.random()
	vec3.set(scale, 1, 1, 1)
	
}

// TODO: derive this from struct header?
let bufferByteStride = 16*4
let bufferFields = [
	{ name:"a_orientation", components:4, type:gl.FLOAT, byteoffset:0*4 },
	{ name:"a_color", components:4, type:gl.FLOAT, byteoffset:4*4 },
	{ name:"a_location", components:3, type:gl.FLOAT, byteoffset:8*4 },
	{ name:"a_phase", components:1, type:gl.FLOAT, byteoffset:11*4 },
	{ name:"a_scale", components:3, type:gl.FLOAT, byteoffset:12*4 },
]
let snakeInstanceBuffer = gl.createBuffer()
gl.bindBuffer(gl.ARRAY_BUFFER, snakeInstanceBuffer);
gl.bufferData(gl.ARRAY_BUFFER, snakeInstanceData, gl.DYNAMIC_DRAW);

snake.bind()
	.setAttributes(snakeInstanceBuffer, bufferByteStride, bufferFields, true)
	.unbind();


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

	for (let i=0; i<snakeInstanceData.length; i+=16) {
		// quat:
		let instance = snakeInstanceData.subarray(i, i+16);
		let phase = instance.subarray(11, 12);
		phase[0] = (phase[0] + dt) % 1.; 
	}
	gl.bindBuffer(gl.ARRAY_BUFFER, snakeInstanceBuffer);
	gl.bufferData(gl.ARRAY_BUFFER, snakeInstanceData, gl.DYNAMIC_DRAW);

	// Compute the matrix
	let projmatrix = mat4.create();
	mat4.perspective(projmatrix, Math.PI*3/2, dim.width/dim.height, 0.01, 10);
	let viewmatrix = mat4.create();
	mat4.lookAt(viewmatrix, [0, 0, 3], [0, 0, 0], [0, 1, 0]);
	let axis = vec3.fromValues(Math.sin(t), 1., 0.);
	vec3.normalize(axis, axis);
	mat4.rotate(viewmatrix, viewmatrix, t, axis)


	//fbo.begin()
	// render to our targetTexture by binding the framebuffer
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo.id);
	{
		gl.viewport(0, 0, fbo.width, fbo.height);
		gl.enable(gl.DEPTH_TEST)
		gl.depthMask(true)
		gl.clearColor(0, 0, 1, 1);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

		snakeprogram.begin();
		snakeprogram.uniform("u_viewmatrix", viewmatrix);
		snakeprogram.uniform("u_projmatrix", projmatrix);
		snake.bind().drawInstanced(NUM_SNAKE_SEGMENTS).unbind();
		snakeprogram.end();
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
