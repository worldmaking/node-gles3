let fs = require("fs"), 
 	path = require("path")

let gl = require("./gles3.js")
let glfw = require("./glfw3.js")
let glutils = require("./glutils.js")
let Window = require("./window.js")
let Shaderman = require("./shaderman.js")
const glfw3 = require("./glfw3.js")

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

function png2tex3(gl, pngpath0, pngpath1, pngpath2) {
	let img0 = pnglib.sync.read(fs.readFileSync(pngpath0))
	let img1 = fs.existsSync(pngpath1) ? pnglib.sync.read(fs.readFileSync(pngpath1)) : null
	let img2 = fs.existsSync(pngpath2) ? pnglib.sync.read(fs.readFileSync(pngpath2)) : null
	let tex = glutils.createPixelTexture(gl, img0.width, img1.height)
	for (let i=0; i<tex.width * tex.height; i++) {
		tex.data[i*4+0] = img0.data[i*4]
		tex.data[i*4+1] = img1 ? img1.data[i*4] : 0
		tex.data[i*4+2] = img2 ? img2.data[i*4] : 0
	}
	tex.bind().submit()
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
	gl.generateMipmap(gl.TEXTURE_2D);
	tex.unbind();
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
let cube = glutils.createVao(gl, glutils.makeCube({ min:-0.5, max:0.5, div: 8 }), shaderman.create(gl, "sdfpbr").id);

let cubes = glutils.createInstances(gl, [
	{ name:"i_quat", components:4 },
	{ name:"i_bounds", components:4 },
	{ name:"i_pos", components:3 },
], 100)
cubes.instances.forEach(obj => {
	vec4.set(obj.i_pos, 
		(Math.random()-0.5) * 20,
		(Math.random()-0.5) * 20,
		(Math.random()-0.9) * 20
	);
	// xyz is bounding box, w is scale factor
	let s = 1
	vec4.set(obj.i_bounds, s, s, s, 1);
	quat.random(obj.i_quat);
})
cubes.bind().submit().unbind();
cubes.attachTo(cube);

// let material_names = ["alien-metal", "beaten-up-metal1", "copper-rock1", "gray-granite-flecks", "vertical-lined-metal", "rustediron2", "iron-rusted4", "greasy-pan-2"]
////let material = loadMaterialTextures(gl, "alien-metal") 
////let material = loadMaterialTextures(gl, "copper-rock1")
////let material = loadMaterialTextures(gl, "gray-granite-flecks")
////let material = loadMaterialTextures(gl, "rustediron2") 
////let material = loadMaterialTextures(gl, "iron-rusted4") 
let material = loadMaterialTextures(gl, "greasy-pan-2")
//let material = loadMaterialTextures(gl, "vertical-lined-metal")
//let material = loadMaterialTextures(gl, "beaten-up-metal1") 

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

	//let light_pos = [100*Math.cos(ts), 100*Math.sin(ts), 0.]
	let light_pos = [0, 4.5, 0]

	// update scene:
	let q = [0, 0, 0, 1]
	for (let obj of cubes.instances) {
		//let obj = cubes.instances[Math.floor(Math.random() * cubes.count)];
		// change its orientation:
		quat.random(q);
		quat.slerp(obj.i_quat, obj.i_quat, q, 0.002);
	}
	// submit to GPU:
	cubes.bind().submit().unbind()


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
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
	material.normal.bind(2)
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
	material.albedo.bind(1)
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
	
	shaderman.shaders.sdfpbr.begin()
		.uniform("u_projmatrix", projmatrix)
		.uniform("u_viewmatrix", viewmatrix)
		.uniform("u_mra_tex", 3)
		.uniform("u_normal_tex", 2)
		.uniform("u_albedo_tex", 1)
		.uniform("u_env_tex", 0)
		.uniform("u_light_pos", light_pos)
		.uniform("u_camera_pos", camera_pos)
	cube.bind().draw().unbind()
	shaderman.shaders.sdfpbr.end()

	shaderman.shaders.sdfpbr.begin()
		.uniform("u_projmatrix", projmatrix)
		.uniform("u_viewmatrix", viewmatrix)
		.uniform("u_mra_tex", 3)
		.uniform("u_normal_tex", 2)
		.uniform("u_albedo_tex", 1)
		.uniform("u_env_tex", 0)
		.uniform("u_light_pos", light_pos)
		.uniform("u_camera_pos", camera_pos)
	cube.bind().drawInstanced(cubes.count).unbind()
	shaderman.shaders.sdfpbr.end()

	// cubeprogram.begin();
	// cubeprogram.uniform("u_viewmatrix", viewmatrix);
	// cubeprogram.uniform("u_projmatrix", projmatrix);
	// cubeprogram.uniform("u_nearfar", near, far);
	// cube.bind().drawInstanced(cubes.count).unbind()
	// cubeprogram.end();

}

Window.animate()