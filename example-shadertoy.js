
const gl = require("./gles3.js")
const glfw = require("./glfw3.js")
const Window = require("./window.js")
const Shadertoy = require("./shadertoy.js")

let window = new Window()


let toy = new Shadertoy(gl, {
	window,
	//dim: [2048, 2048],
	// can be GLSL code string or a file path:
	common: `shaders/shadertoyCommon.glsl`, 
	// up to 5 shaders:
	shaders: [
		{ 
			// code can be GLSL code string or a file path:
			code:`shaders/shadertoyA.glsl`,
			// inputs can refer to a shader pass (integer) or an image file path
			inputs: [0, "textures/lichen.jpg"]
		},
		{ 
			// code can be GLSL code string or a file path:
			code:`shaders/shadertoyImage.glsl`,
			// inputs can refer to a shader pass (integer) or an image file path
			inputs: [0]
		},
	],
})

Window.animate()

