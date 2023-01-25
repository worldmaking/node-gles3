/*



*/


const gl = require("./gles3.js")
const glutils = require("./glutils.js")
const glfw = require("./glfw3.js")
const Window = require("./window.js")

const assert = require("assert"), fs = require("fs"), path = require("path")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")

const jpeg = require('jpeg-js');
const pnglib = require("pngjs").PNG

function png2tex(gl, tex, pngpath) {
	let img = pnglib.sync.read(fs.readFileSync(pngpath))
	//let tex = glutils.createPixelTexture(gl, img.width, img.height)
    tex.width = img.width
    tex.height = img.height
    tex.allocate()
	tex.data = img.data
	tex.bind().submit()
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
	gl.generateMipmap(gl.TEXTURE_2D);
	tex.unbind();
	return tex
}


const calib = JSON.parse(fs.readFileSync("camera_calibration.json"))
const aspect = calib.image_size[0]/calib.image_size[1]
console.log(calib)

let window = new Window({
    width: calib.image_size[0], height: calib.image_size[0]
})

let shape_program = glutils.makeProgram(gl,
`#version 330
uniform mat4 u_modelmatrix;
uniform mat4 u_viewmatrix;
uniform mat4 u_projmatrix;

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;
out vec2 v_uv;

void main() {
    v_uv = a_texCoord;
	gl_Position = vec4(a_position, 1.);
}
`,
`#version 330
precision mediump float;

uniform sampler2D u_tex;
uniform vec4 u_centerfocal;  // center xy, focal xy
uniform vec2 u_tangential;  // p1, p2
uniform vec3 u_radial;       // k1, k2, k3
in vec2 v_uv;
out vec4 outColor;

vec2 undistort(vec2 uv, vec4 centerfocal, vec3 radial, vec2 tangential) {
    // translate, scale
    vec2 xy = (uv - centerfocal.xy) / centerfocal.zw;
    // radial & tangential:
    float r2 = dot(xy, xy);
    float r4 = r2*r2;
    float r6 = r2*r4;
    vec2 tang = 2.*(xy.x*xy.y*tangential) + tangential.yx*(r2 + 2.*xy);
    float rad = (radial.x*r2 + radial.y*r4 + radial.z*r6);
    xy = xy*(1.+rad) + tang;
    // scale, translate
    return xy*centerfocal.zw + centerfocal.xy;
}

void main() {
    vec2 uv = undistort(v_uv, u_centerfocal, u_radial, u_tangential);

	outColor.rgb = texture(u_tex, v_uv).rrr;
	outColor.rgb = texture(u_tex, uv).rrr;
}
`);
let shape_geom = glutils.makeQuad({
    min: [-1, -1/aspect],
    max: [1, 1/aspect],
    div: 64
})
let shape_vao = glutils.createVao(gl, shape_geom, shape_program.id);

let tex = glutils.createTexture(gl)
const dir = "chessboards"
const names = fs.readdirSync(dir)
let counter = 0
function loadimage() {
    png2tex(gl, tex, path.join(dir, names[counter]))
    counter = (counter + 1) % names.length
}
loadimage()
setInterval(loadimage, 3000)


window.draw = function() {

	let { t, dim } = this;

	let f = 0
	gl.clearColor(0, 0, 0, 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	// Compute the matrix
	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	let modelmatrix = mat4.create();
	mat4.lookAt(viewmatrix, [0, 0, 1.5], [0, 0, 0], [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], 0.01, 30);

    mat4.identity(modelmatrix);
    tex.bind().submit()
    shape_program.begin();
        shape_program.uniform("u_modelmatrix", modelmatrix);
        shape_program.uniform("u_viewmatrix", viewmatrix);
        shape_program.uniform("u_projmatrix", projmatrix);
        shape_program.uniform("u_centerfocal", calib.center[0]/calib.image_size[0], calib.center[1]/calib.image_size[1], calib.focal[0]/calib.image_size[0], calib.focal[1]/calib.image_size[1]);
        shape_program.uniform("u_tangential", calib.tangential);
        shape_program.uniform("u_radial", calib.radial);
        
        shape_program.uniform("u_tex", 0);
        shape_vao.bind().draw().unbind();
    shape_program.end();
    tex.unbind()
}

Window.animate()




