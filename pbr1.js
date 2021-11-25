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

function png2tex3(gl, pngpath0, pngpath1, pngpath2) {
	let png0 = PNG.load(pngpath0);
	let png1 = PNG.load(pngpath1);
	let png2 = PNG.load(pngpath2);
	let tex = glutils.createPixelTexture(gl, png0.width, png0.height)
	function submit() {
		tex.bind().submit()
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
		tex.unbind();
	}
	png0.decode(pixels => {
		for (let i=0; i<png0.width * png0.height; i++) {
			tex.data[i*4+0] = pixels[i*4]
		}
		submit()
	})
	png1.decode(pixels => {
		for (let i=0; i<png0.width * png0.height; i++) {
			tex.data[i*4+1] = pixels[i*4]
		}
		submit()
	})
	png2.decode(pixels => {
		for (let i=0; i<png0.width * png0.height; i++) {
			tex.data[i*4+2] = pixels[i*4]
		}
		submit()
	})
	return tex
}

function loadMaterialTextures(gl, name) {
	return {
		albedo: png2tex(gl, path.join("textures", `${name}_albedo.png`)),
		normal: png2tex(gl, path.join("textures", `${name}_normal.png`)),
		mra:    png2tex3(gl, path.join("textures", `${name}_metallic.png`), 
						     path.join("textures", `${name}_roughness.png`),
						     path.join("textures", `${name}_ao.png`))
	}
}

let shaderman = new Shaderman(gl)
let win = new Window()

let quad = glutils.createVao(gl, glutils.makeQuad(), shaderman.create(gl, "background").id);
let cube = glutils.createVao(gl, glutils.makeCube(), shaderman.create(gl, "pbr1").id);

//let material = loadMaterialTextures(gl, "alien-metal") 
let material = loadMaterialTextures(gl, "beaten-up-metal1") 

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
	let d = 3
	let a = t/3
	let camera_pos = [d*Math.sin(a), 1.5, d*Math.cos(a)]
	mat4.lookAt(viewmatrix, camera_pos, camera_at, [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI*0.6, dim[0]/dim[1], 0.01, 20);

	mat4.multiply(projviewmatrix_inverse, projmatrix, viewmatrix)
	mat4.invert(projviewmatrix_inverse, projviewmatrix_inverse)

	let ts = t/10

	
	gl.disable(gl.DEPTH_TEST)
	gl.depthMask(false)

	shaderman.shaders.background.begin()
		.uniform("u_projviewmatrix_inverse", projviewmatrix_inverse)
		.uniform("u_time", t)
		.uniform("u_light_pos", Math.cos(ts), Math.sin(ts), 0.)
	quad.bind().draw().unbind()
	shaderman.shaders.background.end()
	
	gl.depthMask(true)
	gl.enable(gl.DEPTH_TEST)

	
	material.mra.bind(3)
	material.normal.bind(2)
	material.albedo.bind(1)

	shaderman.shaders.pbr1.begin()
		.uniform("u_projmatrix", projmatrix)
		.uniform("u_viewmatrix", viewmatrix)
		.uniform("u_modelmatrix", modelmatrix)
		.uniform("u_mra_tex", 3)
		.uniform("u_normal_tex", 2)
		.uniform("u_albedo_tex", 1)
		.uniform("u_light_pos", Math.cos(ts), Math.sin(ts), 0.)
		.uniform("u_camera_pos", camera_pos)
	cube.bind().draw().unbind()
	shaderman.shaders.pbr1.end()

}

Window.animate()