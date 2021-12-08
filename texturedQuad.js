let fs = require("fs"), 
 	path = require("path")

let gl = require("./gles3.js")
let glfw = require("./glfw3.js")
let glutils = require("./glutils.js")
let Window = require("./window.js")
let Shaderman = require("./shaderman.js")
const glfw3 = require("./glfw3.js")

const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")
const PNG = require("png-js");
function png2tex(gl, pngpath) {
	let png = PNG.load(pngpath);
	let tex = glutils.createPixelTexture(gl, png.width, png.height)
	png.decode(pixels => {
		tex.data = pixels;
		tex.bind().submit()
		gl.generateMipmap(gl.TEXTURE_2D);
		tex.unbind();
	})
	return tex
}

let shaderman = new Shaderman(gl)
let win = new Window()

let name = "greasy-pan-2"
let albedo = png2tex(gl, path.join("textures", `${name}_albedo.png`))

let quad = glutils.createVao(gl, glutils.makeQuad(), shaderman.create(gl, "texturedquad").id);


win.draw = function() {
	const { t, dt, fps, window } = this
	glfw.setWindowTitle(window, fps.toString())
	let dim = glfw.getFramebufferSize(window);

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clearColor(0., 0., 0., 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	let modelmatrix = mat4.create();
	let projviewmatrix_inverse = mat4.create()

	let camera_at = [0, 1.5, 0]
	let d = 1
	let a = 0 //t/3
	let camera_pos = [d*Math.sin(a), 1.5, d*Math.cos(a)]
	mat4.lookAt(viewmatrix, camera_pos, camera_at, [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI*0.6, dim[0]/dim[1], 0.01, 20);

	mat4.multiply(projviewmatrix_inverse, projmatrix, viewmatrix)
	mat4.invert(projviewmatrix_inverse, projviewmatrix_inverse)

	let ts = t/10

	//let light_pos = [100*Math.cos(ts), 100*Math.sin(ts), 0.]
	let light_pos = [0, 4.5, 0]
	mat4.translate(modelmatrix, modelmatrix, camera_at)
	mat4.rotateX(modelmatrix, modelmatrix, 0.1*Math.sin(t))
	mat4.rotateY(modelmatrix, modelmatrix, 0.1*Math.sin(t/3))

	
	albedo.bind(0)

	shaderman.shaders.texturedquad.begin()
		.uniform("u_projmatrix", projmatrix)
		.uniform("u_viewmatrix", viewmatrix)
		.uniform("u_modelmatrix", modelmatrix)
		.uniform("u_albedo_tex", 0)
		.uniform("u_time", t)
		// .uniform("u_light_pos", light_pos)
		// .uniform("u_camera_pos", camera_pos)
	quad.bind().draw().unbind()
	shaderman.shaders.texturedquad.end()

}

Window.animate()