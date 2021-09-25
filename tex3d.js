const assert = require("assert")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
const glfw = require('./glfw3.js')
const vr = require('./openvr.js')
const glutils = require('./glutils.js');


const N = 32;
const DIM = vec3.fromValues(N, N, N)

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
glfw.setWindowPos(window, 32, 32)
glfw.makeContextCurrent(window);
console.log(gl.glewInit());
console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Core Profile?: " + (glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE)==glfw.OPENGL_CORE_PROFILE));

// Enable vertical sync (on cards that support it)
glfw.swapInterval(1); // 0 for vsync off


let tex3d = glutils.createTexture3D(gl, { 
	float:true, 
	channels: 1,
	width:N 
});
// create a duplicate: 
tex3d.data.forEach((v,i,a) => a[i] = 0.1*Math.random())   // or data.slice()
tex3d.data1 = tex3d.data.slice()
// copy it back
tex3d.data.set(tex3d.data1)
tex3d.bind().submit()

let cubeprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
uniform mat4 u_modelmatrix;
uniform float u_N;
uniform sampler3D u_tex;

// instanced variable:
in vec4 i_pos;
in vec4 i_quat;

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
	vec3 tc = i_pos.xyz / u_N;
	v_color = texture(u_tex, tc);

	float s = min(1., abs(v_color.x));
	
	// Multiply the position by the matrix.
	vec4 vertex = vec4(a_position * s, 1.);
	vertex = quat_rotate(i_quat, vertex);
	vertex.xyz += i_pos.xyz;
	gl_Position = u_projmatrix * u_viewmatrix * u_modelmatrix * vertex;

	v_normal = quat_rotate(i_quat, a_normal);


	// v_color = vec4(v_normal*0.25+0.25, 1.);
	// v_color += vec4(a_texCoord*0.5, 0., 1.);
	v_color = vec4(tc, 1.);
	v_color = vec4(texture(u_tex, tc));
}
`,
`#version 330
precision mediump float;

in vec4 v_color;
in vec3 v_normal;
out vec4 outColor;

void main() {
	outColor = v_color;
	//outColor = vec4(1.);
	outColor.a = (1.);
}
`);

// create a VAO from a basic geometry and shader
let cube = glutils.createVao(gl, glutils.makeCube({ min:-0.5, max:0.5, div: 2 }), cubeprogram.id);

let M = N

// create a VBO & friendly interface for the instances:
// TODO: could perhaps derive the fields from the vertex shader GLSL?
let cubes = glutils.createInstances(gl, [
	{ name:"i_pos", components:4 },
	{ name:"i_quat", components:4 },
], M*M*M)

// the .instances provides a convenient interface to the underlying arraybuffer
cubes.instances.forEach((obj, i) => {
	let x = 0.5 + i % M;
	let y = 0.5 + Math.floor(i/M) % M;
	let z = 0.5 + Math.floor(i/(M*M)) % M;
	// each field is exposed as a corresponding typedarray view
	// making it easy to use other libraries such as gl-matrix
	// this is all writing into one contiguous block of binary memory for all instances (fast)
	vec4.set(obj.i_pos, 
		x,
		y,
		z,
		1
	);
	quat.set(obj.i_quat, 0, 0, 0, 1);
})
cubes.bind().submit().unbind();

// attach these instances to an existing VAO:
cubes.attachTo(cube);


let volprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
uniform mat4 u_modelmatrix;
uniform float u_N;
uniform sampler3D u_tex;

// instanced variable:
in vec4 i_pos;
in vec4 i_quat;

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;
out vec4 v_color;
out vec3 v_normal;
out vec3 v_tc;
out vec3 v_eyepos, v_raydir, v_rayorigin;

// http://www.geeks3d.com/20141201/how-to-rotate-a-vertex-by-a-quaternion-in-glsl/
vec3 quat_rotate( vec4 q, vec3 v ){
	return v + 2.0 * cross( q.xyz, cross( q.xyz, v ) + q.w * v );
}
vec4 quat_rotate( vec4 q, vec4 v ){
	return vec4(v.xyz + 2.0 * cross( q.xyz, cross( q.xyz, v.xyz ) + q.w * v.xyz), v.w );
}

// equiv. quat_rotate(quat_conj(q), v):
// q must be a normalized quaternion
vec3 quat_unrotate(in vec4 q, in vec3 v) {
	// return quat_mul(quat_mul(quat_conj(q), vec4(v, 0)), q).xyz;
	// reduced:
	vec4 p = vec4(
		q.w*v.x - q.y*v.z + q.z*v.y,  // x
		q.w*v.y - q.z*v.x + q.x*v.z,  // y
		q.w*v.z - q.x*v.y + q.y*v.x,  // z
		q.x*v.x + q.y*v.y + q.z*v.z   // w
	);
	return vec3(
		p.w*q.x + p.x*q.w + p.y*q.z - p.z*q.y,  // x
		p.w*q.y + p.y*q.w + p.z*q.x - p.x*q.z,  // y
		p.w*q.z + p.z*q.w + p.x*q.y - p.y*q.x   // z
	);
}


void main() {
	v_tc = a_position.xyz;
	//v_color = texture(u_tex, tc);
	
	// Multiply the position by the matrix.
	vec4 vertex = vec4(a_position, 1.);
	vertex = quat_rotate(i_quat, vertex);
	vertex.xyz += i_pos.xyz;
	vec4 worldpos = u_modelmatrix * vertex;
	gl_Position = u_projmatrix * u_viewmatrix * worldpos;

	// derive eyepos (worldspace)
	v_eyepos = -(u_viewmatrix[3].xyz)*mat3(u_viewmatrix);

	// derive ray (texture-space)
	v_rayorigin = v_tc;
	v_raydir = (quat_unrotate(i_quat, worldpos.xyz - v_eyepos));
	// could we compute the ray end? intersection of ray & bounding box

	v_normal = quat_rotate(i_quat, a_normal);

	// v_color = vec4(v_normal*0.25+0.25, 1.);
	// v_color += vec4(a_texCoord*0.5, 0., 1.);
	v_color = vec4(v_tc, 1.);
	//v_color = vec4(texture(u_tex, tc));
}
`,
`#version 330
precision mediump float;

uniform sampler3D u_tex;
in vec4 v_color;
in vec3 v_normal;
in vec3 v_tc;
in vec3 v_eyepos, v_raydir, v_rayorigin;
out vec4 outColor;

// assume box b:  0,0,0 to 1,1,1
float rayBoxExitDistance(vec3 ro, vec3 rd) {
	// pt = ro + rd*t => t = (pt-ro)/rd
	// assumes glsl handles case of rd component==0
    vec3 t1 = (0. - ro)/rd;
    vec3 t2 = (1. - ro)/rd;
	return min(min(max(t1.x, t2.x), max(t1.z, t2.z)), max(t1.y, t2.y));
}

void main() {
	vec3 ro = v_rayorigin;
	vec3 rd = normalize(v_raydir);

	float tmax = rayBoxExitDistance(ro, rd);

	// ok now step from t=0 to t=tmax
	float a = 0.;
	float dt = 1./10.;
	//float t0 = fract(tmax/dt);
	
	for (float t=0.; t < tmax; t += dt) {
		float weight = min(1., tmax-t);
		vec3 pt = ro + t*rd;
		float c = texture(u_tex, pt).x;
		//a = max(a, c);
		// naive additive blending
		//a += c*dt * weight*8.; 
		// transmittance:
		float opacity = exp(-t * abs(c));
		//a += trans * dt * weight;
		//a = max(a, c*dt * weight*50.);
		// Cout(v) = Cin(v) * (1 - Opacity(x)) + Color(x) * Opacity(x)
		float c1 = c*weight*8;
		a = mix(c1, a, opacity);
	}

	outColor = vec4(v_tc, 0.2);
	// float v = texture(u_tex, v_tc).r;
	// outColor = vec4(rd, 0.5);
	outColor *= vec4(tmax);
	outColor = vec4(a);
}
`);

// create a VAO from a basic geometry and shader
let vol = glutils.createVao(gl, glutils.makeCube({ min:0, max:1, div: 4 }), volprogram.id);

let t = glfw.getTime();
let fps = 60;


// Euclidean modulo. assumes n > 0
function wrap(a, n) { 
	const r = a % n;
	return r < 0 ? r + n : r; //a % n + (Math.sign(a) !== Math.sign(n) ? n : 0); 
}

function al_field3d_index(dim, x, y, z) {
	const [dimx, dimy, dimz] = dim;
	return  wrap(x, dimx) + wrap(y, dimy) * dimx + wrap(z, dimz) * dimx*dimy;
}

function al_field3d_splat(dim, ptr, v, p) {
	const [x, y, z] = p;
	const [dimx, dimy, dimz] = dim;
	const x0 = Math.floor(x), y0 = Math.floor(y), z0 = Math.floor(z);
	const x1 = x0+1, y1 = y0+1, z1 = z0+1;
	const xb = x-x0, yb = y-y0, zb = z-z0;
	const xa = x1-x, ya = y1-y, za = z1-z;
	ptr[al_field3d_index(dim, x0, y0, z0)] += v*(xa*ya*za);
	ptr[al_field3d_index(dim, x1, y0, z0)] += v*(xb*ya*za);
	ptr[al_field3d_index(dim, x0, y1, z0)] += v*(xa*yb*za);
	ptr[al_field3d_index(dim, x1, y1, z0)] += v*(xb*yb*za);
	ptr[al_field3d_index(dim, x0, y0, z1)] += v*(xa*ya*zb);
	ptr[al_field3d_index(dim, x1, y0, z1)] += v*(xb*ya*zb);
	ptr[al_field3d_index(dim, x0, y1, z1)] += v*(xa*yb*zb);
	ptr[al_field3d_index(dim, x1, y1, z1)] += v*(xb*yb*zb);
}

function al_field3d_read(dim, ptr, v, p) {
	const [x, y, z] = p;
	const [dimx, dimy, dimz] = dim;
	const x0 = Math.floor(x), y0 = Math.floor(y), z0 = Math.floor(z);
	const x1 = x0+1, y1 = y0+1, z1 = z0+1;
	const xb = x-x0, yb = y-y0, zb = z-z0;
	const xa = x1-x, ya = y1-y, za = z1-z;
	return  ptr[al_field3d_index(dim, x0, y0, z0)]*(xa*ya*za) +
			ptr[al_field3d_index(dim, x1, y0, z0)]*(xb*ya*za) +
			ptr[al_field3d_index(dim, x0, y1, z0)]*(xa*yb*za) +
			ptr[al_field3d_index(dim, x1, y1, z0)]*(xb*yb*za) +
			ptr[al_field3d_index(dim, x0, y0, z1)]*(xa*ya*zb) +
			ptr[al_field3d_index(dim, x1, y0, z1)]*(xb*ya*zb) +
			ptr[al_field3d_index(dim, x0, y1, z1)]*(xa*yb*zb) +
			ptr[al_field3d_index(dim, x1, y1, z1)]*(xb*yb*zb);
}

// equal input-output, like a linear interp, using @diffuse 
// kernel of 6 neighbours
function al_simple_diffuse(dim, iptr, optr, diffuse=0.3) {
	const [dimx, dimy, dimz] = dim;
	const div6 = 1/6;
	for (let z=0;z<dimz;z++) {
		for (let y=0;y<dimy;y++) {
			for (let x=0;x<dimx;x++) {
				const here = al_field3d_index(dim, x, y, z);
				const prev = iptr[here];
				// the six cardinal neighbours:
				const va = iptr[al_field3d_index(dim, x-1,y,  z  )];
				const vb = iptr[al_field3d_index(dim, x+1,y,  z  )];
				const vc = iptr[al_field3d_index(dim, x,  y-1,z  )];
				const vd = iptr[al_field3d_index(dim, x,  y+1,z  )];
				const ve = iptr[al_field3d_index(dim, x,  y,  z-1)];
				const vf = iptr[al_field3d_index(dim, x,  y,  z+1)];
				// expressed as a set of differentials
				const da = va - prev;
				const db = vb - prev;
				const dc = vc - prev;
				const dd = vd - prev;
				const de = ve - prev;
				const df = vf - prev;
				const magdif = Math.min(Math.pow(Math.abs((da + db + dc + dd + de + df))*div6, 2), 1);

				// apply average differential:
				const delta = (da + db + dc + dd + de + df);
				const diff = magdif;
				// let pow = .2
				// const delta = 	Math.sign(da)*Math.pow(Math.abs(da), pow) +
				// 				Math.sign(db)*Math.pow(Math.abs(db), pow) +
				// 				Math.sign(dc)*Math.pow(Math.abs(dc), pow) +
				// 				Math.sign(dd)*Math.pow(Math.abs(dc), pow) +
				// 				Math.sign(de)*Math.pow(Math.abs(de), pow) +
				// 				Math.sign(df)*Math.pow(Math.abs(df), pow);

				optr[here] = prev + diff*delta*div6;
			}
		}
	}
}

// Gauss-Seidel relaxation scheme:
function al_field3d_diffuse(dim, iptr, optr, diffuse=0.1, passes=10) {
	const [dimx, dimy, dimz] = dim;
	let div = (1.0/((1.+6.*diffuse)));
	for (let n=0 ; n<passes; n++) {
		for (let z=0;z<dimz;z++) {
			for (let y=0;y<dimy;y++) {
				for (let x=0;x<dimx;x++) {
					const here = al_field3d_index(dim, x, y, z);
					const prev = iptr[here];
					const va00 = optr[al_field3d_index(dim, x-1,y,  z  )];
					const vb00 = optr[al_field3d_index(dim, x+1,y,  z  )];
					const v0a0 = optr[al_field3d_index(dim, x,  y-1,z  )];
					const v0b0 = optr[al_field3d_index(dim, x,  y+1,z  )];
					const v00a = optr[al_field3d_index(dim, x,  y,  z-1)];
					const v00b = optr[al_field3d_index(dim, x,  y,  z+1)];
					optr[here] = div*(prev + diffuse*(va00 + vb00 + v0a0 + v0b0 + v00a + v00b));
				}
			}
		}
	}
}

let p = vec3.fromValues(
	(N*Math.random()),
	(N*Math.random()),
	(N*Math.random())
)
let q = vec3.fromValues(
	(N*Math.random()),
	(N*Math.random()),
	(N*Math.random())
)


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

	// update scene:

	// copy last frame's data:
	//tex3d.data.set(tex3d.data)
	//console.log(p)
	// decay:
	//tex3d.data.forEach((v, i, a) => a[i] = v * 0.99999)
	// drop energy in at a random point:
	{
		let r = vec3.random(vec3.create(), Math.random())//for (let i=0; i<3; i++) p[i] = wrap(p[i] + 2*(Math.random()-0.5) , N)
		vec3.add(p, p, r)
		r = vec3.random(vec3.create(), Math.random())//for (let i=0; i<3; i++) p[i] = wrap(p[i] + 2*(Math.random()-0.5) , N)
		vec3.add(q, q, r)
		al_field3d_splat(DIM, tex3d.data, 1, p)
		al_field3d_splat(DIM, tex3d.data, -1, q) // balance total of what we add & what we remove
	}
	// copy last frame's data:
	tex3d.data1.set(tex3d.data)
	// diffuse data1 into data:
	al_simple_diffuse(DIM, tex3d.data1, tex3d.data)

	// track total content of field:
	//console.log(tex3d.data.reduce((a, b) => a+b) / tex3d.data.length)

	// Compute the matrix
	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	let modelmatrix = mat4.create();
	let angle = t;
	let r = 2;
	let camera_pos = [r*Math.cos(angle), 1.5, r*Math.sin(angle)];
	let camera_at = [0, 1.5, 0];
	mat4.lookAt(viewmatrix, camera_pos, camera_at, [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], 0.01, 20);

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clearColor(0.1, 0.1, 0.1, 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	gl.enable(gl.DEPTH_TEST)
	tex3d.bind().submit()

	
	mat4.identity(modelmatrix)
	mat4.translate(modelmatrix, modelmatrix, [-1, 0.5, -1, 1])
	mat4.scale(modelmatrix, modelmatrix, [2/N, 2/N, 2/N])
	
	cubeprogram.begin();
	cubeprogram.uniform("u_viewmatrix", viewmatrix);
	cubeprogram.uniform("u_projmatrix", projmatrix);
	cubeprogram.uniform("u_modelmatrix", modelmatrix);
	cubeprogram.uniform("u_N", M);
	cubeprogram.uniform("u_tex", 0);
	cube.bind().drawInstanced(cubes.count).unbind()
	cubeprogram.end();

	


	gl.enable(gl.BLEND);
	gl.blendFunc(gl.SRC_ALPHA, gl.ONE);
	gl.depthMask(false)

	mat4.identity(modelmatrix)
	mat4.translate(modelmatrix, modelmatrix, [-1, 0.5, -1, 1])
	mat4.scale(modelmatrix, modelmatrix, [2, 2, 2])

	volprogram.begin();
	volprogram.uniform("u_viewmatrix", viewmatrix);
	volprogram.uniform("u_projmatrix", projmatrix);
	volprogram.uniform("u_modelmatrix", modelmatrix);
	volprogram.uniform("u_N", M);
	volprogram.uniform("u_tex", 0);
	vol.bind().draw().unbind()
	volprogram.end();

	gl.disable(gl.BLEND);
	gl.depthMask(true)


	tex3d.unbind()

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