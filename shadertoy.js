/* 
	Need to modify the design, since a buffer can feedback to itself, it actually needs an extra texture & swap.

	That also means we can't use texture IDs directly. 
*/

const fs = require("fs"), 
 	path = require("path"),
	assert = require("assert")
const gl = require("./gles3.js")
const glfw = require("./glfw3.js")
const glutils = require("./glutils.js")
const Window = require("./window.js")
const pnglib = require("pngjs").PNG
const jpeg = require('jpeg-js');

function png2tex(gl, imgpath) {
	let img = pnglib.sync.read(fs.readFileSync(imgpath))
	let tex = glutils.createPixelTexture(gl, img.width, img.height)
	tex.data = img.data
	tex.bind().submit()
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
	gl.generateMipmap(gl.TEXTURE_2D);
	tex.unbind();
	return tex
}

function jpg2tex(gl, imgpath) {
	let jpg = jpeg.decode(fs.readFileSync(imgpath));
	let tex = glutils.createPixelTexture(gl, jpg.width, jpg.height)
	assert(tex.data.length == jpg.data.length);
	tex.data = jpg.data;
	tex.bind().submit()
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
	tex.unbind();

	return tex;
}

function img2tex(gl, imgpath) {
	let ext = path.extname(imgpath).toLowerCase()
	if (ext == ".png") {
		return png2tex(gl, imgpath)
	} else {
		return jpg2tex(gl, imgpath)
	}
}


let win = new Window()

win.mouse = {
	pix: [0, 0],
	isdown: false,
	isclick: false,
	vec: [-1, -1, -1, -1]
}

win.onpointermove = function(x, y) {
	let { dim, mouse } = this
	mouse.pix = [ (x*0.5+0.5) * dim[0], (y*0.5+0.5) * dim[1] ]
}

win.onpointerbutton = function(button, action, mods) {
	let mouse = win.mouse
	mouse.isdown = !!action
	mouse.isclick = !!action
}

win.onkey = function(key, scan, down, mod) {
	if (down==1) {
		if (key == 70) { // F
			// toggle fullscreen:
			this.setFullscreen(!this.fullscreen);
		} else {
			console.log(key, down, mod);
		}
	}
}

// Enable vertical sync (on cards that support it)
glfw.swapInterval(1); // 0 for vsync off

class Shadertoy {
	dim = [720, 480];
	textures = {}; 	// library of textures, indexed by file path or pass number
	common = ""; 	// string of glsl for all shaders
	// the passes:
	codes = [];
	programs = [];
	inputs = []; 	// input textures for each program

	watched = {};	// list of files we are watching for changes on

	vert = 
		`#version 330
		in vec4 a_position;
		void main() {
			gl_Position = a_position;
		}`

	frag_header = 
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

	frag_footer = `
		void main() {
			mainImage(outColor, gl_FragCoord.xy);
		}
		`

	/*
		options = {
			dim: vec2
			common: string of glsl for all shaders
			shaders: array of
				{ code: string, textures:[array of file path or shader number] }
		}
	*/
	constructor(gl, options) {
		if (options.dim) this.dim = options.dim
		if (options.common) this.common = options.common

		// is common a filepath?
		if (fs.existsSync(this.common)) {
			// add file to watch list:
			this.watched[this.common] = {
				filepath: this.common,
				mtime: fs.statSync(this.common).mtimeMs,
				kind: "include"
			}
			this.common = fs.readFileSync(this.common);
		}

		this.fbo = gl.createFramebuffer();

		this.output_texture = gl.createTexture();
		this.last_texture = this.output_texture;
		gl.bindTexture(gl.TEXTURE_2D, this.output_texture);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA32F, this.dim[0], this.dim[1], 0, gl.RGBA, gl.FLOAT, null);
		//gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

		// create an output texture for each shader:
		for (let i = 0; i < 4; i++) {
			let tex = gl.createTexture();
			gl.bindTexture(gl.TEXTURE_2D, tex);
			gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA32F, this.dim[0], this.dim[1], 0, gl.RGBA, gl.FLOAT, null);
			//gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

			this.textures[i] = tex
		}

		for (let i = 0; i < options.shaders.length; i++) {
			let { code, inputs } = options.shaders[i]

			// create the shader:
			if (fs.existsSync(code)) {
				// add file to watch list:
				this.watched[code] = {
					filepath: code,
					slot: i,
					mtime: fs.statSync(code).mtimeMs,
					kind: "program"
				}
				code = fs.readFileSync(code);
			}

			this.makeProgram(gl, code, i);

			// load inputs:
			this.inputs[i] = inputs || []
			this.inputs[i].forEach(path => {
				if (typeof path == "string") {
					if (!this.textures[path]) {
						this.textures[path] = img2tex(gl, path).id
					}
				}
			})
		}

		this.display_program = glutils.makeProgram(gl, `#version 330
		in vec4 a_position;
		void main() {
			gl_Position = a_position;
		}`, `
		#version 330
		uniform vec3 iResolution;	// output pixels (not sure what the z component is for?)
		uniform sampler2D iChannel0;
		out vec4 fragColor;

		void main() {
			vec2 uv = gl_FragCoord.xy/iResolution.xy;
			fragColor = texture(iChannel0, uv);
		}
		`);
		this.quad = glutils.createVao(gl, glutils.makeQuad(), this.display_program.id);
	}

	makeProgram(gl, code, i) {
		console.log("reload", i)
		this.codes[i] = code;
		this.programs[i] = glutils.makeProgram(gl, this.vert, this.frag_header + this.common + code + this.frag_footer);
	}

	render(window, gl) {
		const { dim, fbo, textures } = this;
		const { t, dt, fps, frame, mouse } = window;

		if (Math.floor(t-dt) < Math.floor(t)) {   
			// once per second
			
			// check for file changes:
			Object.values(this.watched).forEach(o => {
				let mtime = fs.statSync(o.filepath).mtimeMs
				if (mtime != o.mtime) {
					console.log("reload", o.filepath)
					o.mtime = mtime
					let code = fs.readFileSync(o.filepath);
					if (code) {
						if (o.kind == "program") {
							// destroy existing shader (.dispose())
							if (this.programs[o.slot]) this.programs[o.slot].dispose()
							// create & install new shader
							//this.programs[o.slot] = glutils.makeProgram(gl, vert, frag_header + this.common + code + frag_footer)
							this.makeProgram(gl, code, o.slot)
						} else if (o.kind == "include") {
							this.common = code
							// need to reload all programs:
							for (let i=0; i<this.codes.length; i++) {
								this.makeProgram(gl, this.codes[i], i)
							}
						}
					}
				}
			})
		}

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
	
		gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
		for (let i=0; i<this.programs.length; i++) {
	
			gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, this.output_texture, 0);
			this.last_texture = this.output_texture;

			gl.viewport(0, 0, dim[0], dim[1]);
			gl.disable(gl.DEPTH_TEST)
			gl.depthMask(false)
			gl.clearColor(0, 0, 0, 1);
			gl.clear(gl.COLOR_BUFFER_BIT);

			let program = this.programs[i]
			program.begin()
				.uniform("iResolution", dim[0], dim[1], 0)
				.uniform("iTime", t)
				.uniform("iFrame", frame)
				.uniform("iMouse", mouse.vec)
				.uniform("iDate", iDate)
				.uniform("iTimeDelta", dt)
				.uniform("iFrameRate", fps)

			this.inputs[i].map((k, i) => {
				const id = textures[k]
				if (typeof id == "number") {
					gl.activeTexture(gl.TEXTURE0 + i);
					gl.bindTexture(gl.TEXTURE_2D, id);
					program.uniform("iChannel"+i, i)
				} else {
					program.uniform("iChannel"+i, -1)
				}
			})

			gl.bindBuffer(gl.ARRAY_BUFFER, this.quad.vertexBuffer);
			gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.quad.indexBuffer);
			this.quad.draw()
			program.end()

			// now swap textures this.output_texture and textures[i]
			let tmp = this.output_texture;
			this.output_texture = textures[i];
			textures[i] = tmp;
		}
		gl.bindFramebuffer(gl.FRAMEBUFFER, null);

		if (mouse.isclick) {
			mouse.isclick = false
		}
	}

	display(window, gl, texid) {
		const { dim } = window;
		// now show final:
		gl.viewport(0, 0, dim[0], dim[1]);
		gl.enable(gl.DEPTH_TEST)
		gl.depthMask(true)
		gl.clearColor(0.2, 0.2, 0.2, 1);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
		// use key to select which buffer to show?
		gl.activeTexture(gl.TEXTURE0);
		gl.bindTexture(gl.TEXTURE_2D, texid);
		this.display_program.begin()
			.uniform("iResolution", dim[0], dim[1], 0)
			.uniform("iChannel0", 0)
		this.quad.bind().draw().unbind()
		this.display_program.end();
	}
}

let toy = new Shadertoy(gl, {
	dim: [2048, 2048],
	common: `shaders/shadertoyCommon.glsl`,
	shaders: [
		{ 
			code:`shaders/shadertoyA.glsl`,
			inputs: [0]
		},
		{ 
			code:`shaders/shadertoyImage.glsl`,
			inputs: [0]
		},
	]
})

win.draw = function() {
	const { fps, gl, window } = this;
	glfw.setWindowTitle(window, `fps ${fps}`);

	toy.render(this, gl)
	toy.display(this, gl, toy.last_texture)
}

Window.animate()

