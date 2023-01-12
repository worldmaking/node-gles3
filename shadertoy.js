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
	common = ""; 	// string of glsl for all shaders
	textures = {}; 	// library of textures, indexed by file path or pass number
	programs = [];
	inputs = []; 	// input textures for each program

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
			this.programs[i] = this.makeProgram(gl, code);

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

	makeProgram(gl, code) {
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

		return glutils.makeProgram(gl, vert, frag_header + this.common + code + frag_footer);
	}

	render(window, gl) {
		const { dim, fbo, textures } = this;
		const { t, dt, fps, frame, mouse } = window;

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
	common: `/* deeply inspired by the amazing work of @wyatt */

	#define DIM iResolution.xy
	
	#define PI 3.14159265359
	
	#define A(uv) texture(iChannel0, uv/DIM)
	#define B(uv) texture(iChannel1, uv/DIM)
	#define C(uv) texture(iChannel2, uv/DIM)
	#define D(uv) texture(iChannel3, uv/DIM)
	
	#define norm(v) ((v)/(length(v)+1e-10))
	
	// distance of pt to 2D line segment from start to end
	float line2(vec2 pt, vec2 start, vec2 end) {
		vec2 g = end-start, h = pt-start;
		return length(h - g*clamp(dot(g,h)/dot(g,g), 0.0, 1.));
	}
	
	vec3 hsl2rgb( in vec3 c )
	{
		vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );
		return c.z + c.y * (rgb-0.5)*(1.0-abs(2.0*c.z-1.0));
	}`,

	shaders: [
		{ 
			code:`
			// back project along the vector field to guess where we were previously:
			vec4 prev(vec2 coord) {
				coord -= A(coord).xy*0.5;
				coord -= A(coord).xy*0.5;
				return A(coord);
			}

			void mainImage( out vec4 OUT, in vec2 COORD )
			{
				OUT = vec4(0);
				
				// past neighborhood states (per flow)
				vec4 p = prev(COORD),
					n = prev(COORD + vec2( 0, 1)),
					s = prev(COORD + vec2( 0,-1)),
					e = prev(COORD + vec2( 1, 0)),
					w = prev(COORD + vec2(-1, 0));
				// diffused past:
				vec4 avg = (n+s+e+w)*0.25;
				// ordered difference in the pressure/convergence/disorder (A.z) 
				// creates velocity in me (OUT.xy)
				vec2 force = -0.25*vec2(e.z-w.z, n.z-s.z);
				// new velocity derived from neighbourhood average
				// should this be p.xy rather than avg.xy?
				// either the velocity or the pressure should be diffused, but not both
				float blend = 0.;  // I like blend=0 more, it gives more turbulence; 1 is more smoky
				//OUT.xy = avg.xy + force;
				OUT.xy = mix(p.xy, avg.xy, blend) + force; 
				
				// variance in the velocity (A.xy) near me creates pressure/convergence/disorder in me
				float press = -0.25*(e.x + n.y - w.x - s.y);
				// should this be avg.z rather than p.z  ?
				//OUT.z = p.z + press;
				OUT.z = mix(avg.z, p.z, blend) + press;
				
				/*
					This whole thing about bouncing energy between the velocity and pressure reminds me of scatter junctions in physical models!
				*/
				
				// mass transport
				float transport = -0.25*(e.x*e.w - w.x*w.w + n.y*n.w - s.y*s.w);
				// can mix between p.w and avg.w here to allow general diffusion of mass
				// slightly unrealistic in that this can result in negative mass
				OUT.w = mix(p.w, avg.w, 0.) + transport;
				
				// optional add forces
				float d = line2(COORD, DIM/2. - DIM.y*0.2* vec2(sin(iTime*0.2),cos(iTime*0.2)), DIM/2. + DIM.y*0.4* vec2(sin(iTime*.1618),cos(iTime*.1618)));
				if (d < 1.) {
					OUT = vec4(cos(iTime*PI), sin(iTime*PI), 0, 1);
				}
				if (iMouse.z > 0. && length(iMouse.xy - COORD) < 4.) {
					OUT = vec4(COORD/DIM - 0.5, 0., 1.);
				}
				
				// optional decays
				// xy or z, don't need to do both
				// OUT.xy *= 0.99;
				//OUT.z *= 0.9999;
				OUT.w *= 0.9999;
				
				// boundary:
				float b = 4.;
				if (COORD.x < b || COORD.y < b || DIM.x-COORD.x < b || DIM.y-COORD.y < b) {
					OUT = vec4(0);
				}
				
			}`,
			inputs: [0]
		},
		{ 
			code:`
			void mainImage( out vec4 OUT, in vec2 COORD )
			{
			// OUT = vec4(0.0,0.0,1.0,1.0);
			//
				// Normalized pixel coordinates (from 0 to 1)
			//  vec2 uv = COORD/DIM;

				// Time varying pixel color
			// vec3 col = 0.5 + 0.5*cos(iTime+uv.xyx+vec3(0,2,4));

				// Output to screen
			// OUT = vec4(norm(col),1.0);
				
				// line test
			// float d = line2(COORD, vec2(40,40), vec2(200,200));
				//OUT = vec4(exp(-d));
				
				vec4 a = A(COORD);
				
				OUT.rgb = a.www * hsl2rgb(vec3(0.25*dot(a.xy, vec2(1,0)), abs(a.z), 0.75));
			}`,
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

