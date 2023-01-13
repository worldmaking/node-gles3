
const gl = require("./gles3.js")
const glfw = require("./glfw3.js")
const Window = require("./window.js")
const Shadertoy = require("./shadertoy.js")

let window = new Window()


let toy = new Shadertoy(gl, {
	window,
	//dim: [2048, 2048],
	common: `shaders/shadertoyCommon.glsl`,
	shaders: [
		{ 
			code:`shaders/shadertoyA.glsl`,
			inputs: [0, "textures/lichen.jpg"]
		},
		{ 
			code:`shaders/shadertoyImage.glsl`,
			inputs: [0]
		},
	],
})

Window.animate()

