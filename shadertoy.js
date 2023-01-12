
const fs = require("fs"), 
 	path = require("path")
const gl = require("./gles3.js")
const glfw = require("./glfw3.js")
const glutils = require("./glutils.js")
const Window = require("./window.js")
const assert = require("assert")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")

const pnglib = require("pngjs").PNG

function png2tex(gl, pngpath) {
	let img = pnglib.sync.read(fs.readFileSync(pngpath))
	let tex = glutils.createPixelTexture(gl, img.width, img.height)
	tex.data = img.data
	tex.bind().submit()
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
	gl.generateMipmap(gl.TEXTURE_2D);
	tex.unbind();
	return tex
}

/*
	https://www.shadertoy.com/howto

	Important: coordinates are in pixels, not unipolar normalized texcoords
	
	### Buffers

	Must have a final "Image" shader
	Can have up to 4 "Buffer" shaders (FBOs)
	Can also have 1 "cubemap" shader
	(Can have "Common" glsl code inserted into all shaders)

	Each buffer has a block of code plus up to four inputs, writing to a color texture in an FBO at screen dim

	### Channel Inputs

	Each shader (Image, Buffer A, B, C, D, Cubemap) can have up to 4 texture inputs (iChannel0, 1, 2, 3)
	They can be different for each shader. Textures can have different resolutions, channel counts, types (2D, cube, 3d, video, audio, camera, etc)
	In the end, this just boils down to binding different things to each texture unit 0, 1, 2, 3


*/

let win = new Window()
win.mouse = {
	pix: [0, 0],
	isdown: false,
	isclick: false,
	vec: [-1, -1, -1, -1]
}

// Enable vertical sync (on cards that support it)
glfw.swapInterval(1); // 0 for vsync off

const quad_geom = glutils.makeQuad(gl)

const vert = 
`#version 330
in vec4 a_position;
void main() {
    gl_Position = a_position;
}`

const frag_header = 
`#version 330
uniform vec3 iResolution;	// output pixels (not sure what the z component is for?)
uniform float iTime; 		// seconds
uniform float iTimeDelta; 	// seconds
uniform float iFrameRate; 	// fps
uniform float iFrame; 		// integer count
uniform vec4 iDate;			// year, month0, day0, seconds

uniform vec4 iMouse; 
//      mouse.xy  = mouse position during last button down (i.e. drag, in pixels)
//  abs(mouse.zw) = mouse position during last button click (in pixels)
// sign(mouze.z)  = button is down (drag)
// sign(mouze.w)  = button is clicked (only on first frame)

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;
// uniform vec3 iChannelResolution[4]; // just use textureSize(iChannelN,0) instead

out vec4 outColor;
`

const frag_footer = `
void main() {
	mainImage(outColor, gl_FragCoord.xy);
}
`

const common = ``


let quad_program = glutils.makeProgram(gl, vert, frag_header + common + `

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord/iResolution.xy;

    // sample texture and output to screen
	if (uv.x < 0.25) {
    	fragColor = texture(iChannel0, uv);
	} else if (uv.x < 0.5) {
    	fragColor = texture(iChannel1, uv);
	} else if (uv.x < 0.75) {
    	fragColor = texture(iChannel2, uv);
	} else {
    	fragColor = texture(iChannel3, uv);
	} 
}

` + frag_footer);

let quad_vao = glutils.createVao(gl, quad_geom, quad_program.id);

//let fbo = glutils.makeFboWithDepth(gl, 1024, 1024, false)

// each buffer has 
let buffers = [

]

let textures = [
	png2tex(gl, 'textures/metal_albedo.png'),
	png2tex(gl, 'textures/metal_normal.png'),
	png2tex(gl, 'textures/metal_metallic.png'),
	png2tex(gl, 'textures/metal_roughness.png'),
]

win.onpointermove = function(x, y) {
	let { dim, mouse } = this
	mouse.pix = [ (x*0.5+0.5) * dim[0], (y*0.5+0.5) * dim[1] ]
}

win.onpointerbutton = function(button, action, mods) {
	let mouse = win.mouse
	mouse.isdown = !!action
	mouse.isclick = !!action
}

win.draw = function() {
	const { t, frame, dt, fps, dim, gl, mouse } = this;

	if (mouse.isclick) {
		mouse.vec[2] = mouse.pix[0]
		mouse.vec[3] = mouse.pix[1]
	} else {
		mouse.vec[3] = -Math.abs(mouse.vec[3])
	}
	if (mouse.isdown) {
		mouse.vec[0] = mouse.pix[0]
		mouse.vec[1] = mouse.pix[1]
	} else {
		mouse.vec[2] = -Math.abs(mouse.vec[2])
	}

	// iDate
	let date = new Date()
	let iDate = [
		date.getFullYear(),
		date.getMonth(),
		date.getDate(),
		date.getHours()*3600 + date.getMinutes()*60 + date.getSeconds() + date.getMilliseconds() / 1000
	]

	//console.log(dt, fps)

	let f = 0
	gl.clearColor(f, 1-f, 0, 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	textures[0].bind(0)
	textures[1].bind(1)
	textures[2].bind(2)
	textures[3].bind(3)

	quad_program.begin()
		.uniform("iResolution", dim[0], dim[1], 0)
		.uniform("iTime", t)
		.uniform("iFrame", frame)
		.uniform("iMouse", mouse.vec)
		.uniform("iDate", iDate)
		.uniform("iTimeDelta", dt)
		.uniform("iFrameRate", fps)
		.uniform("iChannel0", 0)
		.uniform("iChannel1", 1)
		.uniform("iChannel2", 2)
		.uniform("iChannel3", 3)

	// draw without vao:
	gl.bindBuffer(gl.ARRAY_BUFFER, quad_vao.vertexBuffer);
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, quad_vao.indexBuffer);
	quad_vao.draw()
	
	quad_program.end();

	if (mouse.isclick) {
		mouse.isclick = false
	
	}
}


Window.animate()