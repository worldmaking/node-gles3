
const assert = require("assert"),
	fs = require("fs"),
	path = require("path");

const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
const glfw = require('./glfw3.js')
const vr = require('./openvr.js')
const glutils = require('./glutils.js');
const PNG = require("png-js")


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

glfw.setWindowPosCallback(window, function(w, x, y) {
	console.log("window moved", w, x, y)
	return 1;
})

glfw.setMouseButtonCallback(window, function(...args) {
	console.log("mouse button", args);
})

glfw.makeContextCurrent(window);
console.log(gl.glewInit());

//can only be called after window creation!
console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Core Profile?: " + (glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE)==glfw.OPENGL_CORE_PROFILE));

// Enable vertical sync (on cards that support it)
glfw.swapInterval(1); // 0 for vsync off

const fontpng = PNG.load("CONSOLATTF.png")
const fontjson = JSON.parse(fs.readFileSync("CONSOLA.TTF-msdf.json", "utf8"))
let fontTexture = glutils.createPixelTexture(gl, fontpng.width, fontpng.height);
fontpng.decode(function(pixels) {
	assert(pixels.length == fontTexture.data.length);
	fontTexture.data = pixels;
	console.log("submitting texture")
	fontTexture.bind().submit()
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
	fontTexture.unbind();
})



let quadInstanceprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_modelmatrix;
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;

// instanced variable:
in vec4 i_fontbounds;
in vec4 i_fontcoord;

in vec4 i_quat;
in vec3 i_pos;
in vec2 i_scale;

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;
out vec4 v_color;
out vec2 v_uv;

// http://www.geeks3d.com/20141201/how-to-rotate-a-vertex-by-a-quaternion-in-glsl/
vec3 applyQuaternionToVector( vec4 q, vec3 v ){
    return v + 2.0 * cross( q.xyz, cross( q.xyz, v ) + q.w * v );
}
vec4 applyQuaternionToVector( vec4 q, vec4 v ){
    return vec4(v.xyz + 2.0 * cross( q.xyz, cross( q.xyz, v.xyz ) + q.w * v.xyz), v.w );
}

void main() {
	// 2D bounded coordinates of quadInstance:
	vec2 p = a_position.xy*i_fontbounds.zw + i_fontbounds.xy; 
	
	// Multiply the position by the matrix.
	vec4 vertex = vec4(p, 0., 1.);
	vertex.xy *= i_scale;
	vertex = applyQuaternionToVector(i_quat, vertex);
	vertex = u_modelmatrix * vertex;
	vertex.xyz += i_pos;
	gl_Position = u_projmatrix * u_viewmatrix * vertex;

	//v_color = vec4(a_normal*0.25+0.25, 1.);
	//v_color += vec4(a_texCoord*0.5, 0., 1.);
	v_color = vec4(1., a_texCoord.y, 1., 1.);

	v_uv = mix(i_fontcoord.xy, i_fontcoord.zw, a_texCoord); 
}
`,
`#version 330
precision mediump float;
uniform sampler2D u_texture;
in vec4 v_color;
in vec2 v_uv;
out vec4 outColor;

float median(float r, float g, float b) {
  	return max(min(r, g), min(max(r, g), b));
}
float aastep(float threshold, float value) {
    float afwidth = length(vec2(dFdx(value), dFdy(value))) * 0.70710678118654757;
    return smoothstep(threshold-afwidth, threshold+afwidth, value);
}

void main() {
	vec3 sample = texture(u_texture, v_uv).rgb;
	float sigDist = median(sample.r, sample.g, sample.b) - 0.5;
	float alpha = clamp(sigDist/fwidth(sigDist) + 0.5, 0.0, 1.0);
	outColor = vec4(alpha) * v_color;
}
`);
let quadInstance = glutils.createVao(gl, glutils.makeQuad({ min:0., max:1, div:8 }), quadInstanceprogram.id);

// instancing:
let quadInstanceInstanceFields = [
	{ 
		name: "i_fontbounds",
		components: 4,
		type: gl.FLOAT,
		byteoffset: 0*4 // *4 for float32
	},
	{ 
		name: "i_fontcoord",
		components: 4,
		type: gl.FLOAT,
		byteoffset: 4*4 // *4 for float32
	},
	{ 
		name: "i_pos",
		components: 4,
		type: gl.FLOAT,
		byteoffset: 8*4 // *4 for float32
	},
	{ 
		name: "i_quat",
		components: 4,
		type: gl.FLOAT,
		byteoffset: 12*4 // *4 for float32
	},
	{ 
		name: "i_scale",
		components: 2,
		type: gl.FLOAT,
		byteoffset: 16*4 // *4 for float32
	},
]
let quadInstanceInstanceByteStride = quadInstanceInstanceFields[quadInstanceInstanceFields.length-1].byteoffset + quadInstanceInstanceFields[quadInstanceInstanceFields.length-1].components*4 // *4 for float32
let quadInstanceInstanceStride = quadInstanceInstanceByteStride / 4; // 4 bytes per float
// create some instances:

let message = "Good morning!";
let quadInstanceInstanceTotal = message.length;
let quadInstanceInstanceData = new Float32Array(quadInstanceInstanceStride * quadInstanceInstanceTotal)

// a friendlier JS interface to the underlying data:
let quadInstanceInstances = []
// iterate over each instance
let x = 0;//-(message.length+1)/2;
for (let i=0; i<quadInstanceInstanceTotal; i++) {
	let char = message[i];

	// TODO: cache a reverse-lookup for this to avoid the find:
	let fontchar = fontjson.chars.find((e)=>{ return e.char===char })

	let b = i*quadInstanceInstanceByteStride;
	// make a  interface for this:
	let obj = {
		index: i,
		byteoffset: b,
	}
	for (let i in quadInstanceInstanceFields) {
		let field = quadInstanceInstanceFields[i];
		obj[field.name] = new Float32Array(quadInstanceInstanceData.buffer, b + field.byteoffset, field.components)
	}
	quadInstanceInstances = obj;

	// Y is flipped
	// something about fontjson.lineHeight and fontjson.base

	// compute a scalar to GL units
	// by default, this should make the line height ~1m?
	//let scalar = fontchar.xadvance / fontjson.common.scaleH;
	let scalar = 1 / fontjson.common.lineHeight;//fontchar.xadvance / fontjson.common.scaleH;

	// position of the anchor for the text
	obj.i_pos[0] = -3;
	obj.i_pos[1] = 0; 
	obj.i_pos[2] = 0;
	obj.i_scale[0] = 1;
	obj.i_scale[1] = 1;
	obj.i_quat[0] = 0;
	obj.i_quat[1] = 0;
	obj.i_quat[2] = 0;
	obj.i_quat[3] = 1;
	// normalized quadInstance bounds:
	obj.i_fontbounds[0] = x + fontchar.xoffset * scalar; 
	obj.i_fontbounds[1] = (fontjson.common.base - fontchar.yoffset) * scalar; 
	obj.i_fontbounds[2] = fontchar.width * scalar; 
	obj.i_fontbounds[3] = -fontchar.height * scalar; 
	// UV coordinates:
	obj.i_fontcoord[0] = fontchar.x / fontjson.common.scaleW;
	obj.i_fontcoord[1] = fontchar.y / fontjson.common.scaleH;
	obj.i_fontcoord[2] = (fontchar.x + fontchar.width) / fontjson.common.scaleW;
	obj.i_fontcoord[3] = (fontchar.y + fontchar.height) / fontjson.common.scaleH;
	
	x += fontchar.xadvance * scalar; 
	
}
console.log("ok")
let quadInstanceInstanceBuffer = gl.createBuffer()
gl.bindBuffer(gl.ARRAY_BUFFER, quadInstanceInstanceBuffer)
gl.bufferData(gl.ARRAY_BUFFER, quadInstanceInstanceData, gl.DYNAMIC_DRAW)
quadInstance.bind().setAttributes(quadInstanceInstanceBuffer, quadInstanceInstanceByteStride, quadInstanceInstanceFields, true).unbind()


// background:
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
in vec2 v_texCoord;
out vec4 outColor;

void main() {
	float y = v_texCoord.y;
	vec3 c1 = vec3(0.2, 0., 0.);
	vec3 c0 = vec3(0.8, 0.1, 0.3);
	outColor = vec4(mix(c0, c1, y), 1.);
}
`);
let quad = glutils.createVao(gl, glutils.makeQuad(), quadprogram.id);

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

	
	
	// Compute the matrix
	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	let modelmatrix = mat4.create();
	mat4.lookAt(viewmatrix, [0, 0, 3], [0, 0, 0], [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], 0.01, 100);

	//mat4.identity(modelmatrix);
	let axis = vec3.fromValues(Math.sin(t), 1., 0.);
	vec3.normalize(axis, axis);
	mat4.rotate(modelmatrix, modelmatrix, Math.sin(t)*0.1, axis)

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clearColor(0.6, 0., 0., 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	// background:
	gl.depthMask(false)
	quadprogram.begin();
	quadprogram.uniform("u_scale", 1, 1);
	quad.bind().draw().unbind();
	quadprogram.end();
	gl.depthMask(true)

	gl.enable(gl.DEPTH_TEST)
	gl.enable(gl.BLEND);
	gl.blendFunc(gl.SRC_ALPHA, gl.ONE);

	// text:
	fontTexture.bind(0)
	quadInstanceprogram.begin();
	quadInstanceprogram.uniform("u_modelmatrix", modelmatrix);
	quadInstanceprogram.uniform("u_viewmatrix", viewmatrix);
	quadInstanceprogram.uniform("u_projmatrix", projmatrix);
	quadInstance.bind().drawInstanced(quadInstanceInstanceTotal).unbind()
	quadInstanceprogram.end();

	gl.disable(gl.BLEND);
	gl.depthMask(true)

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