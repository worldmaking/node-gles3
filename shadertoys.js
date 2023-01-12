/*
    Emulating shadertoy support

    Shadertoy can have iChannel0, iChannel1, etc. textures

    void mainImage( out vec4 fragColor, in vec2 fragCoord ) ... 
where fragCoord contains the pixel coordinates for which the shader needs to compute a color. 
The coordinates are in pixel units, ranging from 0.5 to resolution-0.5, over the rendering surface, 
where the resolution is passed to the shader through the iResolution uniform (see below).

    https://www.shadertoy.com/howto

    Shader can be fed with different types of per-frame static information by using the following uniform variables:

    uniform vec3 iResolution;
    uniform float iTime;
    uniform float iTimeDelta;
    uniform float iFrame;
    uniform float iChannelTime[4];
    uniform vec4 iMouse;
    uniform vec4 iDate;
    uniform float iSampleRate;
    uniform vec3 iChannelResolution[4];
    uniform samplerXX iChanneli;
*/

const assert = require("assert"),
	fs = require("fs"),
	os = require("os"),
	path = require("path")
const { Worker } = require('worker_threads')

const glespath = path.join("..", "node-gles3");
const gl = require(path.join(glespath, 'gles3.js')),
	glfw = require(path.join(glespath, 'glfw3.js')),
	vr = require(path.join(glespath, 'openvr.js')),
	glutils = require(path.join(glespath, 'glutils.js')),
	Shaderman = require(path.join(glespath, 'shaderman.js')),
    Window = require(path.join(glespath, 'window.js')),
    spout = require(path.join(glespath, 'spout.js'))

const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")

const shadertoy_vertex_code = `#version 300 es
in vec4 a_position;
in vec2 a_texCoord;
out vec2 v_uv;

varying vec2 v_position;

void main() {
    gl_Position = a_position;
    v_position = a_position.xy;
    v_uv = a_texCoord;
}`

const shadertoy_frag_include = `#version 300 es
precision mediump float;

uniform vec3 iResolution; // pixels
uniform float iTime; // seconds
uniform float iTimeDelta; // seconds
uniform float iFrame; // integer
//      mouse.xy  = mouse position during last button down
//  abs(mouse.zw) = mouse position during last button click
// sign(mouze.z)  = button is down
// sign(mouze.w)  = button is clicked
uniform vec4 iMouse; // pixels
uniform vec4 iDate; // iDate.w is seconds
uniform vec3 iChannelResolution[4]; // pixels
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;

//uniform float iSampleRate;
//uniform float iChannelTime[4]; // ???

varying vec2 fragCoord;

void main() {
    vec4 fragColor;

    mainImage(fragColor, fragCoord);

    gl_FragColor = fragColor;
}
`


let win1 = new Window({

    // width, height, monitor, fullscreen, title, sync

    mouse: {
        pos: [0.5, 0.5],
        down: 0, click: 0,
    },

    init() {
        console.log("init")
    },
    
    setuniforms(shader) {
        shader.uniform("iTime", this.t)
        shader.uniform("iTimeDelta", this.dt)
        shader.uniform("iFrame", this.frame)
        // uniform vec3 iResolution; // pixels
        // uniform vec4 iMouse; // pixels
        shader.uniform("iMouse", this.mouse.pos[0], this.mouse.pos[1], this.mouse.down, this.mouse.click)
        // uniform vec4 iDate; // iDate.w is seconds
        // uniform vec3 iChannelResolution[4]; // pixels
        // uniform sampler2D iChannel0;
        // uniform sampler2D iChannel1;
        // uniform sampler2D iChannel2;
        // uniform sampler2D iChannel3;
    },

	draw() {
        // this.t, this.dt, this.fps, this.frame, this.dim

		let f = 0
		gl.clearColor(f, 1-f, 0, 1);
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

        this.mouse.click = 0
	},

    // onkey(key, scan, down, mod)
    onpointermove(x, y) {
        this.mouse.pos[0] = x
        this.mouse.pos[1] = y
    },
    onpointerbutton(button, action, mods) {
        if (button == 0) {
            this.mouse.down = action
            this.mouse.click = action
        }
    },
    // onpointerscroll(dy, dx)
    // 
})

Window.animate()