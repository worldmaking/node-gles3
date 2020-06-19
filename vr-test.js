const assert = require("assert")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")

const gl = require("./gles3.js"),
	glfw = require("./glfw3.js"),
	vr = require("./openvr.js");
	
const glutils = require('./glutils.js');



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
glfw.makeContextCurrent(window);
console.log(gl.glewInit());

//can only be called after window creation!
console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Profile: " + glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE));

// Enable vertical sync (on cards that support it)
glfw.swapInterval(0); // 0 for vsync off



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
let geomcube = glutils.makeCube();
// push down 1 meter:
for (i=1; i<geomcube.vertices.length; i+=3) {
	geomcube.vertices[i] -= 1;
}
let cube = glutils.createVao(gl, geomcube, cubeprogram.id);

let cloudprogram = glutils.makeProgram(gl, 
`#version 330
uniform mat4 u_modelmatrix;
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
uniform float u_pixelSize;
in vec3 a_position;
out vec4 v_color;


void main() {
	// Multiply the position by the matrix.
	vec4 worldspace = u_modelmatrix * vec4(a_position.xyz, 1);
	vec4 viewspace = u_viewmatrix * worldspace;
	float viewdist = length(viewspace.xyz);
	gl_Position = u_projmatrix * viewspace;
	if (gl_Position.w > 0.0) {
		gl_PointSize = u_pixelSize / gl_Position.w;
	} else {
		gl_PointSize = 0.0;
	}

	v_color = vec4(worldspace.xyz * 0.5 + 0.5, 0.5);
	v_color = mix(v_color, vec4(1.), 0.95);

	// fade for near clip:
	float fade = min(max((viewdist-0.25)/0.25, 0.), 1.);
	// for distance:
	fade *= 1. - sqrt(viewdist) * 0.1 * 0.05;
	v_color.a *= fade;

}
`,
`#version 330
precision mediump float;

in vec4 v_color;
out vec4 outColor;

void main() {
	// get normalized -1..1 point coordinate
	vec2 pc = (gl_PointCoord - 0.5) * 2.0;
	// convert to distance:
	float dist = max(0., min(1., 0.1 + 1.5*(1.0 - length(pc))));
	// paint
	outColor = vec4(dist) * v_color;
}
`);

const NUM_POINTS = 1e6;
const points = [];
for (let index = 0; index < NUM_POINTS; index++) {
  points.push((Math.random() - 0.5) * 100);
  points.push((Math.random() - 0.5) * 100);
  points.push((Math.random() - 0.5) * 100);
}
let cloud = glutils.createVao(gl, { vertices: points }, cloudprogram.id);


// Create a buffer.
let vertices = new Float32Array(points);
let buffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.DYNAMIC_DRAW);

// Create set of attributes
let vao = gl.createVertexArray();
gl.bindVertexArray(vao);

// tell the position attribute how to pull data out of the current ARRAY_BUFFER
let positionLocation = gl.getAttribLocation(cloudprogram.id, "a_position");
gl.enableVertexAttribArray(positionLocation);
let elementsPerVertex = 3; // for vec2
let normalize = false;
let stride = 0;
let offset = 0;
gl.vertexAttribPointer(positionLocation, elementsPerVertex, gl.FLOAT, normalize, stride, offset);

assert(vr.connect(true), "vr failed to connect");
vr.update()
let vrdim = [vr.getTextureWidth(), vr.getTextureHeight()]
let fbo = glutils.makeFboWithDepth(gl, vrdim[0], vrdim[1])

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

	
	// update scene:
	for (let i=0; i<NUM_POINTS/10; i++) {
		let idx = Math.floor(Math.random() * vertices.length);
		vertices[idx] += (Math.random()-0.5) * 0.01;
	}
	// update GPU buffers:
	gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
	gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.DYNAMIC_DRAW);

	//if(wsize) console.log("FB size: "+wsize.width+', '+wsize.height);
	vr.update();

	console.log(vr.inputSources())
	
	
	// render to our targetTexture by binding the framebuffer
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo.id);
	{
		gl.viewport(0, 0, fbo.width, fbo.height);
		gl.enable(gl.DEPTH_TEST)
		gl.depthMask(true)
		gl.clearColor(0, 0, 0, 1);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

		for (let i=0; i<2; i++) {
			gl.viewport(i * fbo.width/2, 0, fbo.width/2, fbo.height);

			// Compute the matrix
			let viewmatrix = mat4.create();
			//mat4.lookAt(viewmatrix, [0, 0, 3], [0, 0, 0], [0, 1, 0]);
			vr.getView(i, viewmatrix);

			let projmatrix = mat4.create();
			//mat4.perspective(projmatrix, Math.PI/2, fbo.width/fbo.height, 0.01, 10);
			vr.getProjection(i, projmatrix);

			let modelmatrix = mat4.create();
			let axis = vec3.fromValues(Math.sin(t), 1., 0.);
			vec3.normalize(axis, axis);
			//mat4.rotate(modelmatrix, modelmatrix, t, axis)

			cubeprogram.begin();
			cubeprogram.uniform("u_modelmatrix", modelmatrix);
			cubeprogram.uniform("u_viewmatrix", viewmatrix);
			cubeprogram.uniform("u_projmatrix", projmatrix);
			cube.bind().draw().unbind();
			cubeprogram.end();

			gl.enable(gl.BLEND);
			gl.blendFunc(gl.SRC_ALPHA, gl.ONE);
			gl.depthMask(false)

			cloudprogram.begin();
			cloudprogram.uniform("u_modelmatrix", modelmatrix);
			cloudprogram.uniform("u_viewmatrix", viewmatrix);
			cloudprogram.uniform("u_projmatrix", projmatrix);
			cloudprogram.uniform("u_pixelSize", fbo.height/50);
			//cloud.bind().drawPoints().unbind();

			// Bind the attribute/buffer set we want.
			gl.bindVertexArray(vao);
			// Draw the geometry.
			let count = NUM_POINTS;
			gl.drawArrays(gl.POINTS, 0, count);
			gl.bindVertexArray(null);
			cloudprogram.end();


			gl.disable(gl.BLEND);
			gl.depthMask(true)
		}
	}
	gl.bindFramebuffer(gl.FRAMEBUFFER, null);

	vr.submit(fbo.colorTexture)

	// Get window size (may be different than the requested size)
	let dim = glfw.getFramebufferSize(window);
	gl.viewport(0, 0, dim[0], dim[1]);
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
	glfw.swapBuffers(window);
	glfw.pollEvents();
}

function shutdown() {
	vr.connect(false);
	// Close OpenGL window and terminate GLFW
	glfw.destroyWindow(window);
	glfw.terminate();

	process.exit(0);
}

animate();