
const glfw = require("./glfw3.js")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const gl = require('./gles3.js') 
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

let window = glfw.createWindow(720, 720, "Test");
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
glfw.swapInterval(1); // 0 for vsync off


let fbo = glutils.makeFboWithDepth(gl, 2048, 2048)
console.log(fbo)

let fboslab = glutils.createFBO(gl, 256, 256)

let quadprogram = glutils.makeProgram(gl,
`#version 330
in vec4 a_position;
in vec2 a_texCoord;
out vec2 v_texCoord;
void main() {
    gl_Position = a_position;
	v_texCoord = a_texCoord;
}`,
`#version 330
precision mediump float;
uniform sampler2D u_tex0;
uniform sampler2D u_tex1;
uniform int u_usebloom;
in vec2 v_texCoord;
out vec4 outColor;


void main() {
	vec4 blur = texture(u_tex1, v_texCoord) ;

	vec4 raw = texture(u_tex0, v_texCoord);
	outColor = u_usebloom > 0 ? max(raw, blur) : raw;
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
out vec2 v_tc;

void main() {
	// Multiply the position by the matrix.
	vec3 vertex = a_position.xyz;
	gl_Position = u_projmatrix * u_viewmatrix * u_modelmatrix * vec4(vertex, 1);

	// in case rendering with gl.POINTS:
	gl_PointSize = 4.;

	v_color = vec4(a_normal*0.25+0.25, 1.);
	v_color += vec4(a_texCoord*0.5, 0., 1.);

	v_tc = a_texCoord;
}
`,
`#version 330
precision mediump float;

in vec4 v_color;
in vec2 v_tc;
out vec4 outColor;

void main() {

	vec2 tc = abs(mod(v_tc * 4., 1.)-0.5);
	float t = max(tc.x, tc.y) > 0.4 ? 1. : 0.;
	outColor = v_color;
	outColor.rgb = vec3( t );
}
`);
let cube = glutils.createVao(gl, glutils.makeCube({ div: 8 }), cubeprogram.id);


let blurslab = glutils.createSlab(gl, `#version 330
precision mediump float;

uniform sampler2D u_tex0;
in vec2 v_texCoord;
out vec4 outColor;


// gaussian
float u_kernel[9] = float[9](
	0.045, 0.122, 0.045,
	0.122, 0.332, 0.122,
	0.045, 0.122, 0.045
);

void main() {
	vec2 onePixel = vec2(1) / vec2(textureSize(u_tex0, 0));

	vec4 centre = texture(u_tex0, v_texCoord);

	vec4 colorSum =
      texture(u_tex0, v_texCoord + onePixel * vec2(-1, -1)) * u_kernel[0] +
      texture(u_tex0, v_texCoord + onePixel * vec2( 0, -1)) * u_kernel[1] +
      texture(u_tex0, v_texCoord + onePixel * vec2( 1, -1)) * u_kernel[2] +
      texture(u_tex0, v_texCoord + onePixel * vec2(-1,  0)) * u_kernel[3] +
	  centre * u_kernel[4] +
      texture(u_tex0, v_texCoord + onePixel * vec2( 1,  0)) * u_kernel[5] +
      texture(u_tex0, v_texCoord + onePixel * vec2(-1,  1)) * u_kernel[6] +
      texture(u_tex0, v_texCoord + onePixel * vec2( 0,  1)) * u_kernel[7] +
      texture(u_tex0, v_texCoord + onePixel * vec2( 1,  1)) * u_kernel[8] ;

	outColor = colorSum;
}
`)

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
	mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], 0.01, 10);

	//mat4.identity(modelmatrix);
	let axis = vec3.fromValues(Math.sin(t), 1., 0.);
	vec3.normalize(axis, axis);
	mat4.rotate(modelmatrix, modelmatrix, t, axis)


	fbo.begin()
	{
		gl.viewport(0, 0, fbo.width, fbo.height);
		gl.enable(gl.DEPTH_TEST)
		gl.depthMask(true)
		gl.clearColor(0, 0, 0, 1);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

		cubeprogram.begin();
		cubeprogram.uniform("u_modelmatrix", modelmatrix);
		cubeprogram.uniform("u_viewmatrix", viewmatrix);
		cubeprogram.uniform("u_projmatrix", projmatrix);
		//cube.bind().drawPoints().unbind();
		cube.bind().draw().unbind();
		cube.unbind();
		cubeprogram.end();
	}
	fbo.end();

	let src = fbo.colorTexture

	for (let i=0; i<10; i++) {
		fboslab.begin()
		{
			gl.viewport(0, 0, fboslab.width, fboslab.height);
			gl.disable(gl.DEPTH_TEST)
			gl.depthMask(false)
			gl.clearColor(1, 0, 0, 1);
			gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

			gl.bindTexture(gl.TEXTURE_2D, src);
			blurslab.begin()
			blurslab.draw()
			blurslab.end()

			src = fboslab.back.id
		}
		fboslab.end()
	}
	
	gl.viewport(0, 0, dim[0], dim[1]);
	gl.enable(gl.DEPTH_TEST)
	gl.depthMask(true)
	gl.clearColor(0.2, 0.2, 0.2, 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	// render the cube with the texture we just rendered to
   	fboslab.back.bind(1)
	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, fbo.colorTexture);
	
	quadprogram.begin();
	quadprogram.uniform("u_tex0", 0);
	quadprogram.uniform("u_tex1", 1);
	quadprogram.uniform("u_usebloom", Math.floor(t) % 2);
	quad.bind().draw().unbind();
	quadprogram.end();


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