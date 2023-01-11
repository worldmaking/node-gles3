
const fs = require("fs"), 
 	path = require("path")
const gl = require("./gles3.js")
const glfw = require("./glfw3.js")
const glutils = require("./glutils.js")
const Window = require("./window.js")
const assert = require("assert")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")


/*
	https://www.shadertoy.com/howto

	Important: coordinates are in pixels, not unipolar normalized texcoords

	void mainImage( out vec4 fragColor, in vec2 fragCoord );
*/

let win = new Window()
win.mouse = {
	pix: [0, 0],
	isdown: false,
	isclick: false,
	vec: [-1, -1, -1, -1]
}

const vert = 
`#version 330
in vec4 a_position;
in vec2 a_texCoord;
uniform vec2 u_scale;
out vec2 v_texCoord;
void main() {
    gl_Position = a_position;
    gl_Position.xy = gl_Position.xy;
	v_texCoord = a_texCoord;
}`

const frag = 
`#version 330
out vec4 outColor;
in vec2 v_texCoord;

void main() {
	outColor = vec4(1.);
	//mainImage(outColor, in vec2 fragCoord )


}

`

const quad_geom = glutils.makeQuad(gl)



const Shadertoy = {
	
}



///////////////////

// let quad_program = glutils.makeProgram(gl, `#version 330
// in vec4 a_position;
// in vec2 a_texCoord;
// uniform vec2 u_scale;
// out vec2 v_texCoord;
// void main() {
//     gl_Position = a_position;
//     gl_Position.xy = gl_Position.xy;
// 	v_texCoord = a_texCoord;
// }`,
// `#version 330
// precision mediump float;

// in vec4 v_color;
// out vec4 outColor;

// void main() {
// 	outColor = v_color;
// }
// `);

let quad_program = glutils.makeProgram(gl, vert, 
`#version 330
uniform vec3 iResolution;
uniform float iTime; // seconds
// uniform float iTimeDelta;
uniform float iFrame;
// uniform float iChannelTime[4];
uniform vec4 iMouse; 
//      mouse.xy  = mouse position during last button down (i.e. drag, in pixels)
//  abs(mouse.zw) = mouse position during last button click (in pixels)
// sign(mouze.z)  = button is down (drag)
// sign(mouze.w)  = button is clicked (only on first frame)
// uniform vec4 iDate;
// uniform float iSampleRate;
// uniform vec3 iChannelResolution[4];
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;
uniform sampler2D iChannel3;
in vec2 v_texCoord;
out vec4 outColor;


// Created by inigo quilez - iq/2013
// https://www.youtube.com/c/InigoQuilez
// https://iquilezles.org/

// Shows how to use the mouse input (only left button supported):
//
//      mouse.xy  = mouse position during last button down
//  abs(mouse.zw) = mouse position during last button click
// sign(mouze.z)  = button is down
// sign(mouze.w)  = button is clicked



// See also:
//
// Input - Keyboard    : https://www.shadertoy.com/view/lsXGzf
// Input - Microphone  : https://www.shadertoy.com/view/llSGDh
// Input - Mouse       : https://www.shadertoy.com/view/Mss3zH
// Input - Sound       : https://www.shadertoy.com/view/Xds3Rr
// Input - SoundCloud  : https://www.shadertoy.com/view/MsdGzn
// Input - Time        : https://www.shadertoy.com/view/lsXGz8
// Input - TimeDelta   : https://www.shadertoy.com/view/lsKGWV
// Inout - 3D Texture  : https://www.shadertoy.com/view/4llcR4


float distanceToSegment( vec2 a, vec2 b, vec2 p )
{
	vec2 pa = p - a, ba = b - a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	return length( pa - ba*h );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 p = fragCoord / iResolution.x;
    vec2 cen = 0.5*iResolution.xy/iResolution.x;
    vec4 m = iMouse / iResolution.x;
	
	vec3 col = vec3(0.0);

	if( m.z>0.0 ) // button is down
	{
		float d = distanceToSegment( m.xy, abs(m.zw), p );
        col = mix( col, vec3(1.0,1.0,0.0), 1.0-smoothstep(.004,0.008, d) );
	}
	if( m.w>0.0 ) // button click
	{
        col = mix( col, vec3(1.0,1.0,1.0), 1.0-smoothstep(0.1,0.105, length(p-cen)) );
    }

	col = mix( col, vec3(1.0,0.0,0.0), 1.0-smoothstep(0.03,0.035, length(p-    m.xy )) );
    col = mix( col, vec3(0.0,0.0,1.0), 1.0-smoothstep(0.03,0.035, length(p-abs(m.zw))) );

	fragColor = vec4( col, 1.0 );
}


void main() {
	outColor = vec4(1);
	outColor = vec4(v_texCoord, 0., 1.);
	outColor = vec4(sin(iTime));
	mainImage(outColor, gl_FragCoord.xy );
}

`);

let quad_vao = glutils.createVao(gl, quad_geom, quad_program.id);


win.onpointermove = function(x, y) {
	let { dim, mouse } = this
	mouse.pix = [ (x*0.5+0.5) * dim[0], (y*0.5+0.5) * dim[1] ]
}

win.onpointerbutton = function(button, action, mods) {
	let mouse = win.mouse
	mouse.isdown = !!action
	mouse.isclick = !!action
}

win.draw = function() {
	const { t, frame, dim, gl, mouse } = this;

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

	let f = 0
	gl.clearColor(f, 1-f, 0, 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	quad_program.begin()
		.uniform("iResolution", dim[0], dim[1], 0)
		.uniform("iTime", t)
		.uniform("iFrame", frame)
		.uniform("iMouse", mouse.vec)
	quad_vao.bind().draw().unbind();
	quad_program.end();

	console.log(mouse)

	if (mouse.isclick) {
		mouse.isclick = false
	
	}
}

Window.animate()