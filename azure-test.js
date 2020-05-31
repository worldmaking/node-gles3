
const assert = require("assert")
const k4a = require("./k4a.js");

const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
const glfw = require('./glfw3.js')
const glutils = require('./glutils.js');

let numdevices = k4a.device_get_installed_count()
console.log("kinect devices:", numdevices)

let kinect = k4a.device_open()
assert(kinect, "failed to open kinect device")
console.log("kinect azure serial:", k4a.device_get_serialnum(kinect))
assert(!k4a.device_start_cameras(kinect) )   // returns true on error
assert(!k4a.device_start_imu(kinect) )   // returns true on error

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


let cloudprogram = glutils.makeProgram(gl, 
`#version 330
uniform mat4 u_modelmatrix;
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
uniform float u_pixelSize;
in vec3 a_position;
in vec2 a_texcoord;
out vec4 v_color;
out vec2 v_texcoord;

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

	v_texcoord = a_texcoord;

}
`,
`#version 330
precision mediump float;
uniform sampler2D u_tex;
in vec4 v_color;
in vec2 v_texcoord;
out vec4 outColor;

void main() {
	// get normalized -1..1 point coordinate
	vec2 pc = (gl_PointCoord - 0.5) * 2.0;
	// convert to distance:
	float dist = max(0., min(1., 0.1 + 1.5*(1.0 - length(pc))));
	// paint
	outColor = vec4(dist * dist) * v_color * texture(u_tex, v_texcoord).bgra;

}
`);

const NUM_POINTS = 640 * 576 * 3;
const points = [];
const texcoords = [];
for (let index = 0; index < NUM_POINTS; index++) {
	points.push((Math.random() - 0.5) * 2);
	points.push((Math.random() - 0.5) * 2);
	points.push((Math.random() - 0.5) * 2);

	texcoords.push((index % 640) / 640);
	texcoords.push((Math.floor(index / 640)) / 576);
}

// Create a buffer.
let vertices = new Float32Array(points);
let buffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.DYNAMIC_DRAW);

// Create set of attributes
let vao = gl.createVertexArray();
gl.bindVertexArray(vao);

// Create a buffer.
let vtexcoords = new Float32Array(texcoords);
let tbuffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, tbuffer);
gl.bufferData(gl.ARRAY_BUFFER, vtexcoords, gl.STATIC_DRAW);
{
	// tell the position attribute how to pull data out of the current ARRAY_BUFFER
	gl.enableVertexAttribArray(gl.getAttribLocation(cloudprogram.id, "a_position"));
	let elementsPerVertex = 3; // for vec2
	let normalize = false;
	let stride = 0;
	let offset = 0;
	gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
	gl.vertexAttribPointer(gl.getAttribLocation(cloudprogram.id, "a_position"), elementsPerVertex, gl.FLOAT, normalize, stride, offset);
}
{
	// tell the position attribute how to pull data out of the current ARRAY_BUFFER
	gl.enableVertexAttribArray(gl.getAttribLocation(cloudprogram.id, "a_texcoord"));
	let elementsPerVertex = 2; // for vec2
	let normalize = false;
	let stride = 0;
	let offset = 0;
	gl.bindBuffer(gl.ARRAY_BUFFER, tbuffer);
	gl.vertexAttribPointer(gl.getAttribLocation(cloudprogram.id, "a_texcoord"), elementsPerVertex, gl.FLOAT, normalize, stride, offset);
}

let colourTex = glutils.createPixelTexture(gl, 640, 576)
//fill with random values:
colourTex.data.forEach((e, i, a) => { a[i] = Math.random() * 255; });
colourTex.bind().submit().unbind()

// let depthTex = glutils.createPixelTexture(gl, 640, 576)
// depthTex.dataType = gl.UNSIGNED_SHORT
// depthTex.allocate().bind().submit().unbind()

// let depthTex = {
// 	id: gl.createTexture(),
// 	data: null,
// 	width: 640,
// 	height: 576,
// 	channels: 1,
// 	format: gl.RED_INTEGER,
// 	dataType: gl.UNSIGNED_SHORT,

// 	// allocate local data
// 	allocate() {
// 		if (!this.data) {
// 			let elements = this.width * this.height * this.channels;
// 			///////////////////////// how to detect?
// 			this.data = new Uint16Array(elements);
// 		}
// 		return this;
// 	},
// 	bind(unit = 0) {
// 		gl.activeTexture(gl.TEXTURE0 + unit);
// 		gl.bindTexture(gl.TEXTURE_2D, this.id);
// 		return this;
// 	},
// 	unbind(unit = 0) {
// 		gl.activeTexture(gl.TEXTURE0 + unit);
// 		gl.bindTexture(gl.TEXTURE_2D, null);
// 		return this;
// 	},
// 	// bind() first
// 	submit() {
// 		let mipLevel = 0;
// 		let internalFormat = gl.R16UI;   // format we want in the texture
// 		let border = 0;                 // must be 0
// 		gl.texImage2D(gl.TEXTURE_2D, mipLevel, internalFormat, this.width, this.height, border, this.format, this.dataType, this.data);
// 		gl.texImage2D(gl.TEXTURE_2D, 0, gl.R16UI, this.width, this.height, border, gl.RED_INTEGER, gl.UNSIGNED_SHORT, this.data);
// 		//gl.generateMipmap(gl.TEXTURE_2D);
// 		return this;
// 	},
// }

// depthTex.allocate()
// depthTex.data.forEach((e, i, a) => { 
// 	a[i] = Math.random() * 255 * 255; 
// });


// depthTex.bind().submit().unbind()


let t = glfw.getTime();
let fps = 60;
while(!glfw.windowShouldClose(window) && !glfw.getKey(window, glfw.KEY_ESCAPE)) {
	let t1 = glfw.getTime();
	let dt = t1-t;
	fps += 0.1*((1/dt)-fps);
	t = t1;
	// Get window size (may be different than the requested size)
	let dim = glfw.getFramebufferSize(window);
	glfw.setWindowTitle(window, `fps ${fps} dim ${dim[0]}c${dim[1]}`);

	

	if (k4a.device_capture(kinect, 0)) {
		let cloud2world = mat4.create();
		let correction = mat4.create();
		
		// get cloud data:
		vertices = k4a.device_get_cloud(kinect);
		//console.log(vertices.length)
		gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
		gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.DYNAMIC_DRAW);

		let colour = k4a.device_get_color(kinect);
		colourTex.data.set(colour, 0)
		colourTex.bind().submit().unbind()


		let acc = k4a.device_get_acc(kinect);
		// imu coordinate space: up is -Z, cam direction is -X, cam right is -Y
		vec3.set(acc, acc[1], acc[2], -acc[0]);
		//console.log("acc", acc)
		// normalize it
		vec3.normalize(acc, acc);
		let ref = [0, -1, 0];
		let rad = vec3.angle(acc, ref);
		// angle of rotation is angle between acc & ref
		// axis of rotation is perp to both acc and ref (assuming angle > 0)
		//console.log(rad)
		if (Math.abs(rad) > 0.) {
			let axis = vec3.cross(vec3.create(), acc, ref);
			vec3.normalize(axis, axis);
			//console.log("axis", axis);


			//correction = mat4.fromRotation(correction, -rad, axis);
			//mat4.invert(cloud2world, cloud2world);
			//mat4.rotate(cloud2world, cloud2world, t, [0, 1, 0]);
			//console.log(cloud2world)

			correction = mat4.fromRotation(correction, rad, axis);
		}
		//mat4.multiply(cloud2world, correction, cloud2world);
		//mat4.multiply(cloud2world, cloud2world, correction);

		

		mat4.multiply(cloud2world, correction, cloud2world);
		//mat4.translate(cloud2world, cloud2world, [0, 0, -1]);
		//mat4.rotate(cloud2world, cloud2world, t, [0, 1, 0]);
		mat4.translate(cloud2world, cloud2world, [0, 0, 1]);

		k4a.device_set_matrix(kinect, cloud2world);
	} 


	// // update scene:
	// for (let i=0; i<NUM_POINTS/10; i++) {
	// 	let idx = Math.floor(Math.random() * vertices.length);
	// 	vertices[idx] += (Math.random()-0.5) * 0.1;
	// }
	
	// Compute the matrix
	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	mat4.lookAt(viewmatrix, [0, 0, 1], [0, 0, 0], [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], 0.01, 10);
	let modelmatrix = mat4.create();
	let axis = vec3.fromValues(Math.sin(t), 1., 0.);
	vec3.normalize(axis, axis);
	//mat4.rotate(modelmatrix, modelmatrix, t, axis)

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clearColor(0., 0., 0., 1);
	gl.clear(gl.COLOR_BUFFER_BIT);

	colourTex.bind();

	// quadprogram.begin();
	// quadprogram.uniform("u_scale", 1, 1);
	// quad.bind().draw().unbind();
	// quadprogram.end();
	//gl.bindTexture(gl.TEXTURE_2D, null);

	// depthTex.bind()
	// depthquadprogram.begin();
	// depthquadprogram.uniform("u_scale", 1, 1);
	// depthquad.bind().draw().unbind();
	// depthquadprogram.end();
	// gl.bindTexture(gl.TEXTURE_2D, null);

	// particles
	// update GPU buffers:
	if (1) {
		gl.enable(gl.BLEND);
		gl.blendFunc(gl.SRC_ALPHA, gl.ONE);
		gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

		gl.enable(gl.PROGRAM_POINT_SIZE);  //not needed gles3?

		
		// Tell it to use our program (pair of shaders)
		cloudprogram.begin();
		cloudprogram.uniform("u_modelmatrix", modelmatrix);
		cloudprogram.uniform("u_viewmatrix", viewmatrix);
		cloudprogram.uniform("u_projmatrix", projmatrix);
		cloudprogram.uniform("u_pixelSize", dim[1]/2500);

		// Bind the attribute/buffer set we want.
		gl.bindVertexArray(vao);

		// Draw the geometry.
		gl.drawArrays(gl.POINTS, 0, vertices.length/3);
		
		gl.disable(gl.BLEND);
		gl.depthMask(true)
	}
	// Swap buffers
	glfw.swapBuffers(window);
	glfw.pollEvents();

}

// Close OpenGL window and terminate GLFW
glfw.destroyWindow(window);
glfw.terminate();


// k4a.device_stop_imu(kinect);
// k4a.device_stop_cameras(kinect)