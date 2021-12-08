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


//gl.hint(gl.GENERATE_MIPMAP_HINT, gl.NICEST) 

let quad = glutils.createVao(gl, glutils.makeQuad(), shaderman.create(gl, "background").id);
let cube = glutils.createVao(gl, glutils.makeCube(), shaderman.create(gl, "sdfpbr").id);

// let material_names = ["alien-metal", "beaten-up-metal1", "copper-rock1", "gray-granite-flecks", "vertical-lined-metal", "rustediron2", "iron-rusted4", "greasy-pan-2"]
// //let material = loadMaterialTextures(gl, "alien-metal") 
let material = loadMaterialTextures(gl, "beaten-up-metal1") 
// //let material = loadMaterialTextures(gl, "copper-rock1")
//let material = loadMaterialTextures(gl, "gray-granite-flecks")
//let material = loadMaterialTextures(gl, "vertical-lined-metal")
//let material = loadMaterialTextures(gl, "rustediron2") 
//let material = loadMaterialTextures(gl, "iron-rusted4") 
//let material = loadMaterialTextures(gl, "greasy-pan-2")

let envfbo = glutils.makeFboWithDepth(gl, 2048, 1024, true, 0, gl.CLAMP);
let envquad = glutils.createVao(gl, glutils.makeQuad(), shaderman.create(gl, "env").id);

win.draw = function() {
	const { t, dt, fps, window } = this
	glfw.setWindowTitle(window, fps.toString())
	let dim = glfw.getFramebufferSize(window);


	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	let modelmatrix = mat4.create();
	let projviewmatrix_inverse = mat4.create()

	let camera_at = [0, 1.5, 0]
	let d = 3 
	let a = t/3 
	//console.log(a)
	let camera_pos = [d*Math.sin(a), 1.5, d*Math.cos(a)]
	mat4.lookAt(viewmatrix, camera_pos, camera_at, [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI*0.6, dim[0]/dim[1], 0.01, 20);

	mat4.multiply(projviewmatrix_inverse, projmatrix, viewmatrix)
	mat4.invert(projviewmatrix_inverse, projviewmatrix_inverse)

	let ts = t*2

	let light_pos = [100*Math.cos(ts), 100*Math.sin(ts), 0.]
	//let light_pos = [0, 4.5, 0]


	// we're going to want a light map as a mipmapped texture
	// so that in the shader we can use LOD sampling to get rough reflection
	// typically this is a samplerCube, 
	// but we can use a 2D spherical (equirectangular) map 
	// by converting 3D ray to 2D uv first
	// perhaps a sampler3D could also work?
	// whichever we use, the environment needs to be baked into a mipmapped texture first

	envfbo.begin() 
	{
		gl.viewport(0, 0, envfbo.width, envfbo.height);
		gl.clearColor(0., 0., 0., 1);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	
		shaderman.shaders.env.begin()
			.uniform("u_light_pos", light_pos)
		quad.bind().draw().unbind()
		shaderman.shaders.env.end()
	}
	envfbo.end()

	gl.viewport(0, 0, dim[0], dim[1]);
	gl.clearColor(0., 0., 0., 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	gl.disable(gl.DEPTH_TEST)
	gl.depthMask(false)

	envfbo.bind(0)
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
	gl.generateMipmap(gl.TEXTURE_2D)
	
	shaderman.shaders.background.begin()
		.uniform("u_projviewmatrix_inverse", projviewmatrix_inverse)
		.uniform("u_time", t)
		.uniform("u_light_pos", light_pos)
		.uniform("u_env_tex", 0)
	quad.bind().draw().unbind()
	shaderman.shaders.background.end()

	
	// shaderman.shaders.env.begin()
	// 	.uniform("u_light_pos", light_pos)
	// quad.bind().draw().unbind()
	// shaderman.shaders.env.end()
	
	gl.depthMask(true)
	gl.enable(gl.DEPTH_TEST)

	
	material.mra.bind(3)
	material.normal.bind(2)
	material.albedo.bind(1)
	
	shaderman.shaders.sdfpbr.begin()
		.uniform("u_projmatrix", projmatrix)
		.uniform("u_viewmatrix", viewmatrix)
		.uniform("u_modelmatrix", modelmatrix)
		.uniform("u_mra_tex", 3)
		.uniform("u_normal_tex", 2)
		.uniform("u_albedo_tex", 1)
		.uniform("u_env_tex", 0)
		.uniform("u_light_pos", light_pos)
		.uniform("u_camera_pos", camera_pos)
		.uniform("u_modelmatrix", modelmatrix)
	cube.bind().draw().unbind()
	


	shaderman.shaders.sdfpbr.end()

}

Window.animate()