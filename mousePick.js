const assert = require("assert")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
const glfw = require('./glfw3.js')
const glutils = require('./glutils.js');

let a = []
let b = new Float32Array(3);
console.log(a, b, Array.isArray(a), Array.isArray(b), ArrayBuffer.isView(b), b instanceof Float32Array)

let viewmatrix = mat4.create();
let projmatrix = mat4.create();
let viewmatrix_inverse = mat4.create();
let projmatrix_inverse = mat4.create();
let cubes;
let updating = true;
let t = 0
let fps = 60;

let mouse = {
	// mouse position in homongenous clip coordinates:
	clip: [0, 0],
	// mouse position in camera space:
	cam_near: vec3.fromValues(0, 0, 0),
	cam_far: vec3.fromValues(0, 0, -1),
	// mouse position in world space:
	world_near: vec3.fromValues(0, 0, 0),
	world_far: vec3.fromValues(0, 0, -1),
	// mouse ray direction vectors:
	cam_dir: vec3.create(),
	world_dir: vec3.create(),

	updateVectors(projmatrix_inverse, viewmatrix_inverse) {
		// near plane point
		vec3.transformMat4(this.cam_near, [this.clip[0], this.clip[1], -1], projmatrix_inverse);	
		vec3.transformMat4(this.world_near, this.cam_near, viewmatrix_inverse);
		// far plane point
		vec3.transformMat4(this.cam_far, [this.clip[0], this.clip[1], +1], projmatrix_inverse);	
		vec3.transformMat4(mouse.world_far, mouse.cam_far, viewmatrix_inverse);
		// mouse ray:
		vec3.sub(this.cam_dir, this.cam_far, this.cam_near);	
		vec3.normalize(this.cam_dir, this.cam_dir);
		vec3.sub(this.world_dir, this.world_far, this.world_near);
		vec3.normalize(this.world_dir, this.world_dir);
	},
}


function initGL() {
	if (!glfw.init()) {
		console.log("Failed to initialize GLFW");
		process.exit(-1);
	}
	let version = glfw.getVersion();
	console.log('glfw ' + version.major + '.' + version.minor + '.' + version.rev);
	console.log('glfw version-string: ' + glfw.getVersionString());
}

function makeWindow() {
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

	glfw.setWindowPos(window, 32, 32)

	//can only be called after window creation!
	console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Core Profile?: " + (glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE)==glfw.OPENGL_CORE_PROFILE));

	// Enable vertical sync (on cards that support it)
	glfw.swapInterval(1); // 0 for vsync off

	glfw.setCursorPosCallback(window, (window, px, py) => {
		// convert to clip:
		const pix_dim = vec2.div([1, 1], 
			glfw.getWindowContentScale(window), 
			glfw.getFramebufferSize(window)
		);
		mouse.clip[0] =  2*px*pix_dim[0] - 1;
		mouse.clip[1] = -2*py*pix_dim[1] + 1;
	})

	glfw.setKeyCallback(window, (win, key, scan, down, mod) => {
		if (down && key == 32) updating = !updating;
	})

	return window;
}

function makeRender() {

	let floorprogram = glutils.makeProgram(gl,
		`#version 330
		uniform mat4 u_viewmatrix;
		uniform mat4 u_projmatrix;
		
		in vec2 a_position;
		out vec2 v_texCoord;
		
		void main() {
			gl_Position = u_projmatrix * u_viewmatrix * vec4(a_position.x, 0., a_position.y, 1.);
			v_texCoord = a_position;
		}`,
		`#version 330
		precision mediump float;
		
		in vec2 v_texCoord;
		out vec4 outColor;
		
		void main() {
			vec2 grid = smoothstep(0.45, 0.55, abs(0.5-mod(v_texCoord + 0.5, 1.)));
			float bar = max(grid.x, grid.y);
			outColor = vec4(bar);
		}
		`
	);
	let floor_vao = glutils.createVao(gl, glutils.makeQuad({
		min: -10, max: 10, div: 20
	}), floorprogram.id);

	let cubeprogram = glutils.makeProgram(gl,
		`#version 330
		uniform mat4 u_viewmatrix;
		uniform mat4 u_projmatrix;
		
		// instanced variable:
		in vec4 i_quat;
		in vec3 i_pos;
		in float i_highlight;
		in vec3 i_scale;
		
		in vec3 a_position;
		in vec3 a_normal;
		in vec2 a_texCoord;
		out vec4 v_color;
		out vec3 v_normal;
		
		// http://www.geeks3d.com/20141201/how-to-rotate-a-vertex-by-a-quaternion-in-glsl/
		vec3 quat_rotate( vec4 q, vec3 v ){
			return v + 2.0 * cross( q.xyz, cross( q.xyz, v ) + q.w * v );
		}
		vec4 quat_rotate( vec4 q, vec4 v ){
			return vec4(v.xyz + 2.0 * cross( q.xyz, cross( q.xyz, v.xyz ) + q.w * v.xyz), v.w );
		}
		
		void main() {
			// Multiply the position by the matrix.
			vec4 vertex = vec4(a_position * i_scale, 1.);
			vertex = quat_rotate(i_quat, vertex);
			vertex.xyz += i_pos.xyz;
			gl_Position = u_projmatrix * u_viewmatrix * vertex;
		
			v_normal = quat_rotate(i_quat, a_normal);
			v_color = mix(vec4(v_normal*0.5+0.5, i_highlight), vec4(1.), i_highlight);
		}
		`,
		`#version 330
		precision mediump float;
		
		in vec4 v_color;
		out vec4 outColor;
		
		void main() {
			outColor = v_color;
		}
		`
	);
	// create a VAO from a basic geometry and shader
	let cube_geom = glutils.makeCube({ min:-1, max:1, div: 8 });
	let cube = glutils.createVao(gl, cube_geom, cubeprogram.id);
	
	// create a VBO & friendly interface for the instances:
	// TODO: could perhaps derive the fields from the vertex shader GLSL?
	cubes = glutils.createInstances(gl, [
		{ name:"i_quat", components:4 },
		{ name:"i_pos", components:3 },
		{ name:"i_highlight", components:1 },
		{ name:"i_scale", components:3 },
	], 4000)

	// the .instances provides a convenient interface to the underlying arraybuffer
	cubes.instances.forEach(obj => {
		// each field is exposed as a corresponding typedarray view
		// making it easy to use other libraries such as gl-matrix
		// this is all writing into one contiguous block of binary memory for all instances (fast)
		vec3.set(obj.i_pos, 
			(Math.random()-0.5) * 15,
			Math.random() + 1,
			(Math.random()-0.5) * 15
		);
		quat.random(obj.i_quat);
		//quat.set(obj.i_quat, 0, 0, 0, 1)

		vec3.set(obj.i_scale, 
			0.5*Math.random(), 
			0.25*Math.random(), 
			0.03);

		obj.i_highlight[0] = 0;

		// cache the bounding box in each object:
		obj.obb = {
			p0: vec3.negate(vec3.create(), obj.i_scale),
			p1: obj.i_scale
		};
	})
	cubes.bind().submit().unbind();

	// attach these instances to an existing VAO:
	cubes.attachTo(cube);

	return function() {
		floorprogram.begin();
		floorprogram.uniform("u_viewmatrix", viewmatrix);
		floorprogram.uniform("u_projmatrix", projmatrix);
		floor_vao.bind().draw().unbind()
		floorprogram.end();

		cubeprogram.begin();
		cubeprogram.uniform("u_viewmatrix", viewmatrix);
		cubeprogram.uniform("u_projmatrix", projmatrix);
		cube.bind().drawInstanced(cubes.count).unbind()
		cubeprogram.end();
	}
}

initGL();
let window = makeWindow();
let render = makeRender();
animate();

// p0, p1 are the min/max bounding points of the cube
// rayDir is assumed to be normalized to length 1
// boxPos, boxQuat, rayOrigin, rayDir are all assumed to be in world space
function intersectCube(boxPos, boxQuat, p0, p1, rayOrigin, rayDir) {
	// convert ray origin/direction to object-space:
	let origin = vec3.sub(vec3.create(), rayOrigin, boxPos);
	glutils.quat_unrotate(origin, boxQuat, origin);
	let dir = glutils.quat_unrotate(vec3.create(), boxQuat, rayDir);

	// using p = origin + dir*t
	// get ray `t` for each bounding plane of the cube:
	let t0 = [
		(p0[0]-origin[0])/dir[0],
		(p0[1]-origin[1])/dir[1],
		(p0[2]-origin[2])/dir[2],
	];
	let t1 = [
		(p1[0]-origin[0])/dir[0],
		(p1[1]-origin[1])/dir[1],
		(p1[2]-origin[2])/dir[2],
	];
	// sort into first (entry) and second (exit) hits:
	let tmin = vec3.min(vec3.create(), t0, t1); 
	let tmax = vec3.max(vec3.create(), t0, t1);
	// ray is a hit if the furthest entry plane is before the nearest exit plane
	let tentry = Math.max(tmin[0], tmin[1], tmin[2])
	let texit = Math.min(tmax[0], tmax[1], tmax[2])
	// hit if entry is before exit:
	return [tentry <= texit && texit > 0, tentry];
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
	// Get window size (may be different than the requested size)
	let dim = glfw.getFramebufferSize(window);

	// Compute the matrices
	if (updating) {
		let a = t;
		let d = 2 * (Math.cos(t)+2);
		let h = 1.5;
		mat4.lookAt(viewmatrix, [d*Math.cos(a), h, d*Math.sin(t)], [0, h, 0], [0, 1, 0]);
		mat4.invert(viewmatrix_inverse, viewmatrix);
		mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], 0.01, 30);
		mat4.invert(projmatrix_inverse, projmatrix);
	}
	// update mouse coordinates:
	mouse.updateVectors(projmatrix_inverse, viewmatrix_inverse);
	
	// hit test on each cube:
	let hits = []
	// naive hit-test by looping over all and testing in turn
	cubes.instances.forEach((obj, i) => {
		// check for hits:
		let [hit, distance] = intersectCube(obj.i_pos, obj.i_quat, obj.obb.p0, obj.obb.p1, mouse.world_near, mouse.world_dir);
		if (hit) hits.push([obj, distance]);
		obj.i_highlight[0] = 0;
	})
	// if there are hits, sort them by distance
	// then highlight the nearest
	if (hits.length) {
		hits.sort((a,b)=>a[1]-b[1]);
		hits[0][0].i_highlight[0] = 1;
	}

	cubes.bind().submit()

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	gl.enable(gl.DEPTH_TEST)

	render();

	// Swap buffers
	glfw.swapBuffers(window);
	glfw.pollEvents();
	
}

function shutdown() {
	if (window) {
		glfw.destroyWindow(window);
		window = null
	}
	glfw.terminate();
	process.exit(0);
}
