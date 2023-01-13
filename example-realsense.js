
const rs2 = require('bindings')('realsense.node');

const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
const glfw = require('./glfw3.js')
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


let window = glfw.createWindow(1024, 1024, "Test");
if (!window) {
	console.log("Failed to open GLFW window");
	glfw.terminate();
	process.exit(-1);
}
glfw.makeContextCurrent(window);
console.log(gl.glewInit());
glfw.setWindowPos(window, 40, 40)

//can only be called after window creation!
console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Profile: " + glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE));

// Enable vertical sync (on cards that support it)
glfw.swapInterval(1); // 0 for vsync off

let cloudprogram = glutils.makeProgram(gl, `#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
uniform float u_pixelsize;
uniform vec4 u_color;
in vec4 a_color;
in vec3 a_position;
in vec2 a_texCoord;
out vec4 v_color;

void main() {
	// Multiply the position by the matrix.
	gl_Position = u_projmatrix * u_viewmatrix * vec4(a_position.xyz, 1);
	if (gl_Position.w > 0.0) {
		gl_PointSize = u_pixelsize / gl_Position.w;
	} else {
		gl_PointSize = 0.0;
	}

	v_color = vec4(u_color.rgb, a_color.a);
	//v_color = vec4(a_texCoord, 0., 1.);
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
`);



let pipe = new rs2.Pipeline().start()
pipe.modelmatrix = mat4.create();
// mat4.translate(pipe.modelmatrix, pipe.modelmatrix, [0, 2.65, 0])
// mat4.rotateX(pipe.modelmatrix, pipe.modelmatrix, 0.5*Math.PI)
pipe.maxarea = 0.0001
pipe.min = [-1.1, 0.2, -1.2]
pipe.max = [1.2, 2, 1.]
pipe.grab(true) // true means wait for a result

const NUM_SPORES = 1e4
let spores_geom = {
	vertices: new Float32Array(NUM_SPORES*3),
	colors: new Float32Array(NUM_SPORES*4),
	texCoords: new Float32Array(NUM_SPORES*2)
}
let spores = glutils.createVao(gl, spores_geom, cloudprogram.id);


const NUM_POINTS = 640 * 480 // = 307200
let points_geom = {
	vertices: pipe.vertices,
	texCoords: new Float32Array(NUM_POINTS*2)
}
let points = glutils.createVao(gl, points_geom, cloudprogram.id);

for (let i=0; i<NUM_POINTS; i++) {
	let r = Math.random()*Math.random()*2
	let a = Math.random()*2*Math.PI
	vec3.set(points.geom.vertices.subarray(i*3), 
		r*Math.cos(a),
		Math.random() * 2, 
		r*Math.sin(a)
	);

	let col = i % 640, row = Math.floor(i/640)
	let u = (col+0.5) / 640
	let v = (row+0.5) / 480
	points.geom.texCoords[i*2+0] = u
	points.geom.texCoords[i*2+1] = v
}

glutils.ok(gl, "after vao")

let meshprogram = glutils.makeProgram(gl, `#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
uniform float u_pixelsize;
in vec3 a_position;
in vec3 a_normal;
out vec4 v_color;

void main() {
	// Multiply the position by the matrix.
	gl_Position = u_projmatrix * u_viewmatrix * vec4(a_position.xyz, 1);
	if (gl_Position.w > 0.0) {
		gl_PointSize = u_pixelsize / gl_Position.w;
	} else {
		gl_PointSize = 0.0;
	}

	//v_color = vec4(0.4);
	v_color = vec4(a_normal*0.5+0.5, 1.);
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

	outColor = v_color;
}
`);

let mesh_geom = {
	vertices: pipe.vertices,
	normals: pipe.normals,
	indices: pipe.indices
}
let mesh = glutils.createVao(gl, mesh_geom, meshprogram.id);
const MAX_INDICES = 639*479*12

console.log(pipe.indices[pipe.count-1])

let voxshader = glutils.makeProgram(gl, `
#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
uniform mat4 u_modelmatrix;
uniform vec3 u_dim;
uniform sampler3D u_tex;

// instanced variable:
in vec3 i_pos;

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;
out vec4 v_color;
out vec3 v_normal;

void main() {
	v_color = texture(u_tex, i_pos).rrrr;

	float s = min(1., abs(v_color.x));
	
	// Multiply the position by the matrix.
	vec4 vertex = vec4(a_position * s, 1.);
	vertex.xyz += i_pos.xyz;
	gl_Position = u_projmatrix * u_viewmatrix * u_modelmatrix * vertex;
	v_normal = a_normal;
}`,`
#version 330
precision mediump float;

in vec4 v_color;
in vec3 v_normal;
out vec4 outColor;

void main() {
	outColor = v_color;
}`)

let vdim = vec3.fromValues(64, 48, 48)
let vtex = glutils.createTexture3D(gl, { 
	float:true, 
	channels:1, 
	width:vdim[0], 
	height:vdim[1], 
	depth:vdim[2]
})
vtex.bind().submit()

let s = 0.5
vvao = glutils.createVao(gl, glutils.makeCube({ 
	min:[-s/vdim[0], -s/vdim[1], -s/vdim[2] ], 
	max:[ s/vdim[0],  s/vdim[1],  s/vdim[2] ], 
	div: 2 
}), voxshader.id);
let vcount = vdim[0]*vdim[1]*vdim[2]
vvbo = glutils.createInstances(gl, [
	{ name:"i_pos", components:3 },
], vcount)

let idx = 0
for (let z=0; z<vdim[2]; z++) {
	for (let y=0; y<vdim[1]; y++) {
		for (let x=0; x<vdim[0]; x++) {
			let obj = vvbo.instances[idx]
			vec3.set(obj.i_pos, 
				x / vdim[0],
				y / vdim[1],
				z / vdim[2]
			);
			idx++
		}
	}
}
vvbo.bind().submit().unbind();
// attach these instances to an existing VAO:
vvbo.attachTo(vvao);

// vmat maps a 0..1 volume to 3D space
let vmat = mat4.create()
// makes sense to make this fit the bounding box of the lidar points
mat4.translate(vmat, vmat, pipe.min)
mat4.scale(vmat, vmat, vec3.sub([0, 0, 0], pipe.max, pipe.min))
// inverse of this matrix needed to map world-space points into (normalized) voxel space:
let vmat_inverse = mat4.create()
mat4.invert(vmat_inverse, vmat)


// // let's also create a float texture from this data:
// let htex = glutils.createTexture(gl, {
// 	float: true,
// 	channels: 1,
// 	width: 640,
// 	height: 480,
// 	//data: pipe.depth
// })
// console.log(htex)

vec3.normalize(pipe.accel, pipe.accel)
let axisy = vec3.fromValues(0, 0, 1)
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
	let dim = glfw.getFramebufferSize(window);

	let sporechance = dt  * pipe.count / MAX_INDICES
	for (let i=0; i<NUM_SPORES; i++) {
		let j = i*3
		let k = pipe.indices[Math.floor(Math.random() * pipe.count)] * 3
		if (Math.random() < sporechance && mesh.geom.normals[k+1] > 0.5) {
			spores.geom.vertices[j+0] = mesh.geom.vertices[k+0]
			spores.geom.vertices[j+1] = mesh.geom.vertices[k+1]
			spores.geom.vertices[j+2] = mesh.geom.vertices[k+2]
			spores.geom.colors[i*4+3] = 1.;
		
		} else {
			spores.geom.vertices[j+0] += dt * 0.1 * (Math.random()-0.5)
			spores.geom.vertices[j+1] += dt * 0.1 * (Math.random()-0.5)
			spores.geom.vertices[j+2] += dt * 0.1 * (Math.random()-0.5)
			spores.geom.colors[i*4+3] *= 0.99;
		}
	}

	function lidarRotate(lidar, config={ degreesY: 0, pos: [0, 2.65, 0]}) {

	}


	if (pipe.grab(false, 0.0001)) {
		// pipe.accel is a vec3 that represents gravity
		// we should apply a rotation to the point cloud such that this vector becomes -Y
		// by default, let us assume that +X remains the same; then we rotate around X
		// after that, only one other angle defines the coordinate frame

		let a = vec3.clone(pipe.accel)
		vec3.normalize(a, a)
		vec3.lerp(axisy, axisy, a, 0.1)
		vec3.normalize(axisy, axisy)
		let axisz = vec3.cross(vec3.create(), axisy, [axisy[2], axisy[0], axisy[1]])
		vec3.normalize(axisz, axisz)
		let axisx = vec3.cross(vec3.create(), axisy, axisz)
		vec3.normalize(axisz, axisz)
	
		mat4.set(pipe.modelmatrix, 
			axisx[0], axisy[0], axisz[0], 0.,
			axisx[1], axisy[1], axisz[1], 0.,
			axisx[2], axisy[2], axisz[2], 0.,
			0, 0, 0, 1);

		let y = 1000
		for (let i=0; i<pipe.vertices.length; i+=3) {
			y = Math.min(y, pipe.vertices[i+1])
		}
		console.log(y)

		let tr = mat4.fromTranslation(mat4.create(), [0, 2.65, 0])
		let degreesY = 0
		let ro = mat4.fromYRotation(mat4.create(), degreesY*Math.PI/180)
		mat4.multiply(pipe.modelmatrix, ro, pipe.modelmatrix)
		mat4.multiply(pipe.modelmatrix, tr, pipe.modelmatrix)
		
		//console.log(pipe.accel)
		//console.log(vtex.data, vdim, lidar2volume_mat, 0.5, 0.1)
		pipe.voxels(vtex.data, vdim, vmat_inverse, 0.5, 0.01);

		vtex.bind().submit()
		points.bind().submit()
		spores.bind().submit()
		mesh.bind().submit()
	}
	// Compute the matrix
	let h = 0
	let a = t/10
	let r = 3//1.5
	let z = r*Math.cos(a*Math.PI*2), x = r*Math.sin(a*Math.PI*2)
	let viewmatrix = mat4.create();
	mat4.lookAt(viewmatrix, [x, h, z], [0, h, 0], [0, 1, 0]);

	let projmatrix = mat4.create();
	mat4.perspective(projmatrix, Math.PI * 0.7, dim[0]/dim[1], 0.05, 50);

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clearColor(0., 0., 0., 1);
	gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT);

	gl.enable(gl.DEPTH_TEST)
	gl.depthMask(true);

	meshprogram.begin()
		.uniform("u_viewmatrix", viewmatrix)
		.uniform("u_projmatrix", projmatrix)
	//if (pipe.count) mesh.bind().draw(pipe.count).unbind()
	meshprogram.end()



	gl.disable(gl.DEPTH_TEST)
	gl.depthMask(false);

	gl.enable(gl.BLEND);
	gl.blendFunc(gl.SRC_ALPHA, gl.ONE);
	//gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA )

	cloudprogram.begin()
		.uniform("u_viewmatrix", viewmatrix)
		.uniform("u_projmatrix", projmatrix)
		.uniform("u_pixelsize", dim[1] / 2000)
		.uniform("u_color", 0.5, 0.5, 0.5, 0.5)
	points.bind().drawPoints().unbind()
	cloudprogram.end()

	
	cloudprogram.begin()
		.uniform("u_viewmatrix", viewmatrix)
		.uniform("u_projmatrix", projmatrix)
		.uniform("u_pixelsize", dim[1] / 200)
		.uniform("u_color", 0.5, 1, 0., 1)
	spores.bind().drawPoints().unbind()
	cloudprogram.end()

	vtex.bind(0)
	voxshader.begin()
		.uniform("u_viewmatrix", viewmatrix)
		.uniform("u_projmatrix", projmatrix)
		.uniform("u_modelmatrix", vmat)
		.uniform("u_tex", 0)
	//vvao.bind().drawInstanced(vcount).unbind()
	voxshader.end()

	gl.disable(gl.BLEND);

	gl.enable(gl.DEPTH_TEST)
	gl.depthMask(true);

	// Swap buffers
	glfw.swapBuffers(window);
	glfw.pollEvents();
}

function shutdown() {
	// Close OpenGL window and terminate GLFW
	glfw.destroyWindow(window);
	glfw.terminate();

	process.exit(0);
}

animate();
