const assert = require("assert")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")

const gl = require("./gles3.js"),
	glfw = require("./glfw3.js"),
	vr = require("./openvr.js");
	
const glutils = require('./glutils.js');

/*
	
	Translate to a cylindrical space (height unchanged)
	XZ <-> radius, angle
	The idea here is that deltas are encoded in this format, so that the path looping end-on-end will orbit around the viewer
	Another is a kind of redirected walking, in which a rotation component is applied according to current location
	
	- angle is a cyclic property, requires modular arithmetic
	- tempting to also suggest making radius somehow a relative term, so that paths do not 'wander away'
		one way would be to invert radial change on each loop?

	

*/

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
glfw.setWindowPos(window, 25, 25)

//////////////////////////////////////////////////

let paths = []

const world_min = [-2, 0, -2]
const world_max = [ 2, 3,  2]

// hand state machine:
function handStateMachine() {
	let currentpath
	let state = none

	function none(event) {
		// handle a trigger down event
		if (event.pressed) {
			// start a new path:
			currentpath = {
				pos: vec3.clone(event.pos),
				hue: Math.random() * 360,
				deltas: []
			}
			paths.push(currentpath)
			state = drawing
		}
	}

	function drawing(event) {
		
		// exit conditions
		if (!event.pressed) {
			currentpath.isComplete = true
			currentpath = null
			state = none
			return;
		}
		
		// continue drawing:
		let delta = { 
			pos: vec3.clone(event.pos),
			dpos: vec3.clone(event.dpos)
		};
		currentpath.deltas.push(delta);
	}

	return function(event) {
		// dispatch to whatever function `state` currently points to:
		state(event)
	}
}

let LHSM = handStateMachine()
let RHSM = handStateMachine()

function updatePaths() {
	// animate each line:
	for (let path of paths) {
		// don't animate current path
		if (!path.isComplete) continue;
		if (path.deltas.length <= 1) continue;
		// mutate path:
		// for (let p of path.deltas) {
		//   p.dx += (Math.random() - 0.5);
		//   p.dy += (Math.random() - 0.5);
		// } 
		// shift path end-on-end
		let p = path.deltas.shift();
		let p0 = path.deltas[0]
		let p1 = path.deltas[path.deltas.length-1]
		path.deltas.push(p);
		// move it by the last segment too:
		vec3.add(p.pos, p1.pos, p1.dpos)
		// wrap at edges
		for (let i=0; i<3; i++) {
			if (p.pos[i] > world_max[i]) p.pos[i] -= (world_max[i]-world_min[i])
			if (p.pos[i] < world_min[i]) p.pos[i] += (world_max[i]-world_min[i])
		}

		// update location of entire strokedxe4
		vec3.copy(path.pos, p0.pos)
	}
}

//////////////////////////////////////////////////

const MAX_NUM_LINES = 100000

let lineprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;

// instance variables:
in vec4 i_color;
in vec3 i_pos0;
in vec3 i_pos1;

in float a_position; // not actually used...
in vec2 a_texCoord;

out vec4 v_color;
out float v_t;

void main() {
	float t = a_texCoord.x;
	vec3 dpos = i_pos1 - i_pos0;
	float len = length(dpos);
	vec4 vertex = vec4( mix( i_pos0, i_pos1, t), 1.);

	gl_Position = u_projmatrix * u_viewmatrix * vertex;
	float f = 0.1;
	v_color = i_color * clamp(f/(f + len), 0., 1.);
	v_t = t;
}
`,
`#version 330
precision mediump float;

in vec4 v_color;
in float v_t;
out vec4 outColor;

void main() {
	outColor = v_color;
}
`);
// create a VAO from a basic geometry and shader
let line = glutils.createVao(gl, glutils.makeLine({ min:0, max:1, div: 2 }), lineprogram.id);

// create a VBO & friendly interface for the instances:
// TODO: could perhaps derive the fields from the vertex shader GLSL?
let lines = glutils.createInstances(gl, [
	{ name:"i_color", components:4 },
	{ name:"i_pos0", components:3 },
	{ name:"i_pos1", components:3 },
], MAX_NUM_LINES)

// the .instances provides a convenient interface to the underlying arraybuffer
lines.instances.forEach((obj, i) => {
	let p = i/lines.count;
	let a = Math.PI * 2 * p;
	let x = Math.cos(a);
	let z = Math.sin(a);
	// pick a color:
	vec4.set(obj.i_color, 0.75, 1, 1, 0.75);
	if (i>0) vec3.copy(obj.i_pos0, lines.instances[i-1].i_pos1);
	vec3.set(obj.i_pos1, x, 1, z);
})
lines.bind().submit().unbind();

// attach these instances to an existing VAO:
lines.attachTo(line);
	
let pointsprogram = glutils.makeProgram(gl, 
`#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
uniform float u_pixelSize;
in vec3 a_position;
out vec4 v_color;

void main() {
	// Multiply the position by the matrix.
	gl_Position = u_projmatrix * u_viewmatrix * vec4(a_position.xyz, 1);
	if (gl_Position.w > 0.0) {
		gl_PointSize = u_pixelSize / gl_Position.w;
	} else {
		gl_PointSize = 0.0;
	}

	v_color = vec4(1.);
	//v_color = vec4(u_viewmatrix[3].xyz * 0.5 + 0.5, 0.5);
	//v_color = vec4(gl_Position.xyz * 0.5 + 0.5, 0.5);
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
	float dist = max(0., 1.0 - length(pc));
	// paint
		outColor = vec4(dist) * v_color;
}
`)
const NUM_POINTS = 10000;
let pointsgeom = {
	vertexComponents: 3,
	vertices: new Float32Array(NUM_POINTS*3),
	vec3s: [],
}
// a more convenient interface:
for (let i=0; i<NUM_POINTS; i++) {
	pointsgeom.vec3s[i] = pointsgeom.vertices.subarray(i*3, i*3+3)
}
let points = glutils.createVao(gl, pointsgeom, pointsprogram.id);

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
uniform float u_scale;
in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;
out vec4 v_color;

void main() {
	// Multiply the position by the matrix.
	vec3 vertex = a_position.xyz * u_scale;
	gl_Position = u_projmatrix * u_viewmatrix * u_modelmatrix * vec4(vertex, 1);

	v_color = vec4(a_texCoord*0.5, 0., 1.);
	v_color = vec4(a_normal*0.25+0.25, 1.);
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


assert(vr.connect(true), "vr failed to connect");
vr.update()
let models = vr.getModelNames()
console.log(models)
let vrdim = [vr.getTextureWidth(), vr.getTextureHeight()]
let fbo = glutils.makeFboWithDepth(gl, vrdim[0], vrdim[1])

let t = glfw.getTime();
let fps = 60;
let idx = 0

// initialize default VR event handling objects:
let left_hand_event = {
	trigger: 0, 
	pressed: 0,
	grip: 0,
	pad: 0,
	menu: 0,
	padx: 0,
	pady: 0,
	pos: vec3.create(),
	dpos: vec3.create(), // velocity
	mat: mat4.create()
}
let right_hand_event = left_hand_event
let hmd;

function makeHandEvent(input, old_event) {
	let pos = input.targetRaySpace.subarray(12, 15);
	let {buttons, axes} = input.gamepad;
	return {
		handedness: input.handedness,
		trigger: buttons[0].value, 
		pressed: buttons[0].pressed,
		grip: buttons[1].pressed,
		pad: buttons[2].pressed,
		menu: buttons[3].pressed,
		padx: axes[0],
		pady: axes[1],
		pos: vec3.clone(pos),
		dpos: vec3.sub(vec3.create(), pos, old_event.pos), // velocity
		mat: mat4.clone(input.targetRaySpace)
	}
}

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

	// update simulation
	updatePaths()

	let line_count = 0;
	let point_count = 0;

	// get the VR input events
	// pass controller events through to our state machines
	vr.update();
	let inputs = vr.inputSources()
	for (let input of inputs) {
		if (input.targetRayMode == "gaze") {
			hmd = input;
		} else if (input.handedness == "left" && input.targetRaySpace) {
			left_hand_event = makeHandEvent(input, left_hand_event)
			LHSM( left_hand_event )
			vec3.copy(points.geom.vec3s[point_count++], left_hand_event.pos);
		} else if (input.handedness == "right" && input.targetRaySpace) {
			right_hand_event = makeHandEvent(input, right_hand_event)
			RHSM( right_hand_event )
			vec3.copy(points.geom.vec3s[point_count++], right_hand_event.pos);
		}
	}

	// copy the active CPU paths into the GPU line instances:
	for (let j=0; j<paths.length && point_count < NUM_POINTS; j++) {
		// get each path in turn:
		let path = paths[j];
		let pt0 = path.pos;
		// loop over all points in path
		for (let i=0; i < path.deltas.length && line_count < lines.count; i++) {
			let p = path.deltas[i];
			let pt1 = p.pos;
			let line = lines.instances[line_count];
			// or: pt1 = vec3.add(vec3.create(), pt0, p.dpos)
			vec3.copy(line.i_pos0, pt0)
			vec3.copy(line.i_pos1, pt1)
			vec3.set(line.i_color, 1, 1, 1, 1);
			pt0 = pt1
			line_count++;
		}
		vec3.copy(points.geom.vec3s[point_count++], pt0);
	}
	
	// submit to GPU:
	lines.bind().submit().unbind()
	points.bind().submit().unbind()
	
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
			cubeprogram.uniform("u_scale", 1);
			cubeprogram.uniform("u_viewmatrix", viewmatrix);
			cubeprogram.uniform("u_projmatrix", projmatrix);
			cube.bind().draw().unbind();
			cubeprogram.end();

			if (left_hand_event) {
				cubeprogram.begin();
				cubeprogram.uniform("u_modelmatrix", left_hand_event.mat);
				cubeprogram.uniform("u_scale", 0.01);
				cubeprogram.uniform("u_viewmatrix", viewmatrix);
				cubeprogram.uniform("u_projmatrix", projmatrix);
				cube.bind().draw().unbind();
				cubeprogram.end();
			}

			if (right_hand_event) {
				cubeprogram.begin();
				cubeprogram.uniform("u_modelmatrix", right_hand_event.mat);
				cubeprogram.uniform("u_scale", 0.01);
				cubeprogram.uniform("u_viewmatrix", viewmatrix);
				cubeprogram.uniform("u_projmatrix", projmatrix);
				cube.bind().draw().unbind();
				cubeprogram.end();
			}

			gl.enable(gl.BLEND);
			gl.blendFunc(gl.SRC_ALPHA, gl.ONE);
			gl.depthMask(false)

			lineprogram.begin();
			lineprogram.uniform("u_viewmatrix", viewmatrix);
			lineprogram.uniform("u_projmatrix", projmatrix);
			// consider gl.LINE_STRIP with simpler geometry
			line.bind().drawInstanced(line_count, gl.LINES).unbind()
			lineprogram.end();

			
			pointsprogram.begin();
			pointsprogram.uniform("u_viewmatrix", viewmatrix);
			pointsprogram.uniform("u_projmatrix", projmatrix);
			pointsprogram.uniform("u_pixelSize", 10);
			points.bind().drawPoints(point_count).unbind()
			pointsprogram.end();

			gl.disable(gl.BLEND);
			gl.depthMask(true)
		}
	}
	gl.bindFramebuffer(gl.FRAMEBUFFER, null);

	// TODO: insert some nice bloom effect here

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