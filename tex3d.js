const assert = require("assert")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
const glfw = require('./glfw3.js')
const vr = require('./openvr.js')
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
glfw.setWindowPos(window, 32, 32)
glfw.makeContextCurrent(window);
console.log(gl.glewInit());
console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Core Profile?: " + (glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE)==glfw.OPENGL_CORE_PROFILE));

// Enable vertical sync (on cards that support it)
glfw.swapInterval(1); // 0 for vsync off

function glok(msg="gl not ok: ") {
	let err = gl.getError();
	assert(!err, msg+err);
}

function createTexture3D(gl, opt={}) {
    const isFloat = !!opt.float;//&& EXT_color_buffer_float;
    const channels = opt.channels || 4; // RGBA
    const width = opt.width || 16;
    const height = opt.height || width;
    const depth = opt.depth || width;
    
    let format = gl.RGBA;
    if (channels == 1) {
        format = gl.RED;
    } else if (channels == 2) {
        format = gl.LUMINANCE_ALPHA;
    } else if (channels == 3) {
        format = gl.RGB;
    }

    let internalFormat = format;
    let type = gl.UNSIGNED_BYTE;
    if (isFloat) {
        type = gl.FLOAT;
        if (channels == 1) {
            internalFormat = gl.R32F;
        } else if (channels == 2) {
            internalFormat = gl.RG32F;
        } else if (channels == 3) {
            internalFormat = gl.RGB32F;
        } else {	
            internalFormat = gl.RGBA32F;
        }
    }

    console.log("texture", isFloat, channels, width, height, depth)
    //console.log(format, gl.RGBA);
    //console.log(internalFormat, gl.RGBA32F);
	glok('texture')

    let tex = {
        id: gl.createTexture(),
        data: null,
        isFloat: isFloat,
        width: width,
		height: height,
		depth: depth,
        channels: channels,
        format: format,
        type: type,
        filter_min: opt.filter_min || opt.filter || gl.LINEAR,
        filter_mag: opt.filter_mag || opt.filter || gl.LINEAR,
        internalFormat: internalFormat,  // type of data we are supplying,
        
        // allocate local data
        allocate() {
            if (!this.data) {
                let elements = this.width * this.height * this.depth * this.channels;
                if (this.isFloat) {
                    this.data = new Float32Array(elements);
                } else {
                    this.data = new Uint8Array(elements);
                }
            }
            return this;
        },
        
        // bind() first
        submit() {
			//gl.enable(gl.TEXTURE_3D)
			//gl.texImage3D(gl.TEXTURE_3D, mipLevel, this.internalFormat, this.width, this.height, this.depth, border, this.format, this.type, this.data);
			gl.texImage3D(gl.TEXTURE_3D, 0, this.internalFormat, this.width, this.height, this.depth, 0, this.format, this.type, this.data);
			console.log(gl.TEXTURE_3D, this.internalFormat == gl.RGBA32F, this.format == gl.RGBA, this.type == gl.FLOAT);
			let err = gl.getError();
            assert(!err, 'gl error in texture submit: '+err);
            return this;
        },
        
        bind(unit = 0) {
			let err = gl.getError();
            assert(!err, 'gl error before activeTexture: '+err);
			gl.activeTexture(gl.TEXTURE0 + unit);
			err = gl.getError();
            assert(!err, 'gl error in activeTexture: '+err);
			gl.enable(gl.TEXTURE_3D)
			err = gl.getError();
            assert(!err, 'gl error in enable: '+err);
			gl.bindTexture(gl.TEXTURE_3D, this.id);
			err = gl.getError();
            assert(!err, 'gl error in bind: '+err);
            return this;
        },
        unbind(unit = 0) {
			gl.activeTexture(gl.TEXTURE0 + unit);
			gl.enable(gl.TEXTURE_3D)
            gl.bindTexture(gl.TEXTURE_3D, null);
            return this;
        },

        // TODO read / readInto methods for accessing underlying data
        read(pos) {
            let x = Math.floor(pos[0]);
            let y = Math.floor(pos[1]);
            let z = Math.floor(pos[2]);
            let idx = ((this.height*z + y)*this.width + x) * this.channels; // TODO: assumes single-channel
            return this.data[idx];
        }
	};
	
	
	glok('tex')

    tex.allocate().bind().submit();

    // unless we get `OES_texture_float_linear` we can not filter floating point
    gl.texParameteri(gl.TEXTURE_3D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_3D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_3D, gl.TEXTURE_WRAP_R, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_3D, gl.TEXTURE_MIN_FILTER, tex.filter_min);
    gl.texParameteri(gl.TEXTURE_3D, gl.TEXTURE_MAG_FILTER, tex.filter_mag);
    
    return tex.unbind();
}

glok('befoe 3d texture')


let N = 10;

let tex3d = createTexture3D(gl, { float:true, width:N });
// for (let i=0; i<tex3d.data.length; i++) {
// 	tex3d.data[i] = Math.random();
// }
console.log(tex3d)
glok('made 3d texture')


let cubeprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;
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
	// Multiply the position by the matrix.
	vec4 vertex = vec4(a_position, 1.);
	vertex = quat_rotate(i_quat, vertex);
	vertex.xyz += i_pos.xyz;
	gl_Position = u_projmatrix * u_viewmatrix * vertex;

	v_normal = quat_rotate(i_quat, a_normal);

	vec3 tc = vertex.xyz / u_N;

	// v_color = vec4(v_normal*0.25+0.25, 1.);
	// v_color += vec4(a_texCoord*0.5, 0., 1.);
	v_color = vec4(tc, 1.);
	v_color = texture(u_tex, tc);
}
`,
`#version 330
precision mediump float;

in vec4 v_color;
in vec3 v_normal;
out vec4 outColor;

void main() {
	outColor = v_color;
}
`);
// create a VAO from a basic geometry and shader
let cube = glutils.createVao(gl, glutils.makeCube({ min:-0.1, max:0.1, div: 8 }), cubeprogram.id);


// create a VBO & friendly interface for the instances:
// TODO: could perhaps derive the fields from the vertex shader GLSL?
let cubes = glutils.createInstances(gl, [
	{ name:"i_pos", components:4 },
	{ name:"i_quat", components:4 },
], N*N*N)

// the .instances provides a convenient interface to the underlying arraybuffer
cubes.instances.forEach((obj, i) => {
	let x = 0.5 + i % N;
	let y = 0.5 + Math.floor(i/N) % N;
	let z = 0.5 + Math.floor(i/(N*N)) % N;
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
	// Get window size (may be different than the requested size)
	let dim = glfw.getFramebufferSize(window);

	// update scene:

	// // pick a random instance:
	// let obj = cubes.instances[Math.floor(Math.random() * cubes.count)];
	// // change its orientation:
	// quat.random(obj.i_quat);
	// // submit to GPU:
	// cubes.bind().submit().unbind()

	// Compute the matrix
	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	let modelmatrix = mat4.create();
	let camera_pos = [N/2+N*Math.cos(t), N/2, N/2+N*Math.sin(t)];
	let camera_at = [N/2, N/2, N/2];
	mat4.lookAt(viewmatrix, camera_pos, camera_at, [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI/3, dim[0]/dim[1], 0.01, N*3);

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clearColor(0.2, 0.2, 0.2, 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	gl.enable(gl.DEPTH_TEST)

	//tex3d.bind()

	cubeprogram.begin();
	cubeprogram.uniform("u_viewmatrix", viewmatrix);
	cubeprogram.uniform("u_projmatrix", projmatrix);
	cubeprogram.uniform("u_N", N);
	cube.bind().drawInstanced(cubes.count).unbind()
	cubeprogram.end();

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