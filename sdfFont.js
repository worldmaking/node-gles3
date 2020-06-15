
const assert = require("assert"),
fs = require("fs"),
path = require("path");

const glfw = require("./glfw3.js")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
const glutils = require('./glutils.js');

const PNG = require("png-js");

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

glfw.setWindowPos(window, 30, 30)

//can only be called after window creation!
console.log('GL ' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MAJOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_VERSION_MINOR) + '.' + glfw.getWindowAttrib(window, glfw.CONTEXT_REVISION) + " Profile: " + glfw.getWindowAttrib(window, glfw.OPENGL_PROFILE));

// Enable vertical sync (on cards that support it)
glfw.swapInterval(1); // 0 for vsync off



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
	outColor = vec4(v_texCoord * 0.2, 0., 1.);
}
`);
let quad = glutils.createVao(gl, glutils.makeQuad(), quadprogram.id);

function createSDFFont(gl, pngpath, jsonpath) {
	let png = PNG.load(pngpath);
	let json = JSON.parse(fs.readFileSync(jsonpath, "utf8"));
	let font = {
		png: png,
		json: json,
		texture: glutils.createPixelTexture(gl, png.width, png.height),
		// add to json a quick lookup table by character:
		lookup: {},
		// add a quick scalar factor:
		scale: 1 / json.info.size,
	}
	json.chars.forEach(char => { 
		font.lookup[char.char.toString()] = char; 
		// cache UVs here:
		char.texCoords = vec4.set(vec4.create(),
			char.x / json.common.scaleW,
			char.y / json.common.scaleH,
			(char.x + char.width) / json.common.scaleW,
			(char.y + char.height) / json.common.scaleH
		);	
		// cache quad bounds here:
		char.quad = vec4.set(vec4.create(),
			char.xoffset * font.scale,
			(json.common.base - char.yoffset) * font.scale,
			char.width * font.scale,
			-char.height * font.scale
		); 
	})

	png.decode(pixels => {
		assert(pixels.length == font.texture.data.length);
		font.texture.data = pixels;
		font.texture.bind().submit()
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
		font.texture.unbind();
	})

	return font;
}

let font = createSDFFont(gl, "CONSOLATTF.png", "CONSOLA.TTF-msdf.json")


let textquadprogram = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;

// instanced variable:
in mat4 i_modelmatrix;
in vec4 i_fontbounds;
in vec4 i_fontcoord;
//in vec4 i_color;

// geometry variables:
in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;

out vec4 v_color;
out vec2 v_uv;

// http://www.geeks3d.com/20141201/how-to-rotate-a-vertex-by-a-quaternion-in-glsl/
vec3 quat_rotate( vec4 q, vec3 v ){
	return v + 2.0 * cross( q.xyz, cross( q.xyz, v ) + q.w * v );
}
vec4 quat_rotate( vec4 q, vec4 v ){
	return vec4(v.xyz + 2.0 * cross( q.xyz, cross( q.xyz, v.xyz ) + q.w * v.xyz), v.w );
}

void main() {
	// 2D bounded coordinates of textquad:
	vec2 p = a_position.xy*i_fontbounds.zw + i_fontbounds.xy; 
	
	// Multiply the position by the matrix.
	vec4 vertex = i_modelmatrix * vec4(p, 0., 1.);
	gl_Position = u_projmatrix * u_viewmatrix * vertex;

	// if needed:
	// v_normal = (i_modelmatrix * vec4(0., 0., 1., 1.)).xyz;

	v_color = vec4(1.);
	// if needed:
//	v_color = i_color;

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
	outColor = v_color * alpha;
}
`);
let textquad = glutils.createVao(gl, glutils.makeQuad({ min:0., max:1, div:8 }), textquadprogram.id);

// create a VBO & friendly interface for the instances:
// TODO: could perhaps derive the fields from the vertex shader GLSL?
let textquads = glutils.createInstances(gl, [
	{ name: "i_modelmatrix", components: 16 },
	{ name: "i_fontbounds", components: 4 },
	{ name: "i_fontcoord", components: 4 },
//	{ name: "i_color", components: 4 },
])

// bind instance VBO to VAO:
textquads.attachTo(textquad);

// message is a string
// idx is the instance index to start adding character at (allows appending strings)
const mat4_idenity = mat4.create();
function setMessage(message, modelmatrix=mat4_idenity, idx=0) {
	// reallocate if necessary:
	textquads.allocate(idx + message.length);
	// the .instances provides a convenient interface to the underlying arraybuffer
	let x = 0;
	let y = 0;
	for (var i = 0; i < message.length; i++) {
		let c = message.charAt(i).toString();
		// space characters don't render, just update cursor:
		if (c === " ") {
			x += font.lookup[" "].xadvance * font.scale;
		} else if (c === "\t") {
			x += font.lookup[" "].xadvance * font.scale * 3;
		} else if (c === "\n") {
			x = 0;
			y -= font.json.common.lineHeight * font.scale;
		} else {
			const char = font.lookup[c];
			if (!char) {
				console.error("couldn't find character: ", c, typeof(c));
				continue;
			}
			// get instance interface for this character:
			let obj = textquads.instances[idx];
			// the anchor coordinate system for the text:
			mat4.copy(obj.i_modelmatrix, modelmatrix);
			// color:
//			vec4.set(obj.i_color, 1, 1, 1, 1)
			// bounding coordinates of the quad for this character:
			vec4.copy(obj.i_fontbounds, char.quad);
			// offset by character location:
			obj.i_fontbounds[0] += x;
			obj.i_fontbounds[1] += y;
			// UV coordinates for this character:	
			vec4.copy(obj.i_fontcoord, char.texCoords);
			// next instance:
			idx++; 
			// update cursor:
			x += char.xadvance * font.scale;
		}
	}
	// submit VBO and attach to VAO:
	textquads.bind().submit().unbind()

	// return the used instance count so we know how many to render
	return idx;
}

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
	//if(wsize) console.log("FB size: "+wsize.width+', '+wsize.height);

	// Compute the matrix
	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	let modelmatrix = mat4.create();
	mat4.lookAt(viewmatrix, [0, 0, 3], [0, 0, 0], [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], 0.01, 100);

	mat4.fromTranslation(modelmatrix, [-4, 1, 0]);
	let axis = vec3.fromValues(Math.sin(t), 1., 0.);
	vec3.normalize(axis, axis);
	mat4.rotate(modelmatrix, modelmatrix, 0.1*Math.cos(t*3), axis)


	textquads.count = setMessage("hello\nworld")
	// append with newline:
	textquads.count = setMessage(`fps ${fps}`, modelmatrix, textquads.count);
	
	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clearColor(0.2, 0.2, 0.2, 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	gl.enable(gl.DEPTH_TEST)
	gl.depthMask(false)

	// background 
	quadprogram.begin();
	quadprogram.uniform("u_scale", 1, 1);
	quad.bind().draw().unbind();
	quadprogram.end();

	
	gl.enable(gl.BLEND);
	gl.blendFunc(gl.SRC_ALPHA, gl.ONE);
	gl.depthMask(false)

	// text:
	font.texture.bind(0)
	textquadprogram.begin();
	//textquadprogram.uniform("u_modelmatrix", modelmatrix);
	textquadprogram.uniform("u_viewmatrix", viewmatrix);
	textquadprogram.uniform("u_projmatrix", projmatrix);
	textquad.bind().drawInstanced(textquads.count).unbind()
	textquadprogram.end();

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
