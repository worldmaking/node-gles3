/*
	Each particle is one fragment of the texture atlas
	One way is to tie an ID to the particle, and use that ID to look up other properties if needed (e.g. particle velocity, if not driven entirely by e.g. fluid or curl noise)
	
	# convert random images into a square frame
	mogrify -gravity center -resize 64x64 *.jpg
	# merge them into an atlas 
	montage -tile 16x16 -density 300 -border 0 -geometry 64x64 -gravity center -extent 32x32 -background none *.jpg out.png

	# merge image sequence into movie
	ffmpeg -i img%04d.png out.mp4
*/

const gl = require("./gles3.js")
const glfw = require("./glfw3.js")
const Window = require("./window.js")
const Shadertoy = require("./shadertoy.js")

let w = 6*1920
let h = 1080
let dimdiv = 8
let window = new Window({
    width: w/dimdiv,
    height: h/dimdiv
})


let toy = new Shadertoy(gl, {
	window,
	dim: [w, h],
	// can be GLSL code string or a file path:
	common: `shaders/siggraph-common.glsl`, 
	// up to 5 shaders:
	shaders: [
		{ 
			// code can be GLSL code string or a file path:
			code:`shaders/siggraph-A.glsl`,
			// inputs can refer to a shader pass (integer) or an image file path
			inputs: [0, 1]
		},
		{ 
			// code can be GLSL code string or a file path:
			code:`shaders/siggraph-B.glsl`,
			// inputs can refer to a shader pass (integer) or an image file path
			inputs: [0, 1, 2, "textures/rgbanoise_medium.png"]
		},
		{ 
			// code can be GLSL code string or a file path:
			code:`shaders/siggraph-D.glsl`,
			// inputs can refer to a shader pass (integer) or an image file path
			inputs: [0, 1, 2, "textures/siggraph.png"]
		} ,
		{ 
			// code can be GLSL code string or a file path:
			code:`shaders/siggraph.glsl`,
			// inputs can refer to a shader pass (integer) or an image file path
			inputs: [0, 1, 2, 2]
		},
	],
})

Window.animate()

