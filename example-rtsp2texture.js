/*

gst-launch-1.0 rtspsrc location="rtsp://192.168.86.178:554/h264cif?username=admin&password=123456" latency=0 buffer-mode=auto ! rtph264depay ! avdec_h264 ! glimagesink 
gst-launch-1.0 rtspsrc location="rtsp://192.168.86.178:554/h264cif?username=admin&password=123456" latency=0 buffer-mode=auto ! rtph264depay ! avdec_h264 ! videoconvert ! video/x-raw,format=GRAY8 ! glimagesink 

Try calibrating using calibdb.net (the image is at calibdb.net/board.png)
*/


const gl = require("./gles3.js")
const glutils = require("./glutils.js")
const glfw = require("./glfw3.js")
const Window = require("./window.js")

const assert = require("assert"), fs = require("fs")
const { vec2, vec3, vec4, quat, mat2, mat2d, mat3, mat4} = require("gl-matrix")

const gstreamer = require('gstreamer-superficial');
const { PNG } = require("pngjs")
const pnglib = require("pngjs").PNG


class RTSP2Tex {
    tex = null;
    pipeline = null;

    constructor(url, dim) {
        this.url = url
        this.dim = dim
        this.tex = glutils.createTexture(gl, {
            width: dim[0],
            height: dim[1],
            channels: 1,
        })
        
        // avdec_h264 appears to give 1.5 bytes per pixel (12 bit color?) 
        // adding the videoconvert to UYVY gives us 2 bytes per pixel
        // adding the videoconvert to GRAY8 gives us 1 byte per pixel
        this.pipeline = new gstreamer.Pipeline(`rtspsrc location="${url}" latency=0 buffer-mode=auto ! rtph264depay ! avdec_h264 ! videoconvert ! video/x-raw,format=GRAY8 ! appsink name=sink`);
        this.appsink = this.pipeline.findChild('sink');

        this.pipeline.play();

        this.onPull = (buf) => {
            if (buf) {
                const bytes = new Uint8Array(buf.buffer)
                //console.log('BUFFER size', buf.buffer, buf.buffer.byteLength / 640 / 480);
                // copy into texture:
                this.tex.data.set(bytes)
                //console.log(tex.data)
                this.appsink.pull(this.onPull);
            } else {
                console.log('NULL BUFFER');
                setTimeout(() => this.appsink.pull(this.onPull), 500);
            }
        }

        this.appsink.pull(this.onPull);
    }
}


// const { RTSPClient, H264Transport, AACTransport } = require("yellowstone");
// const { Writable } = require("stream")
// const transform = require("sdp-transform");

// // User-specified details here.
// const url = "rtsp://192.168.86.178:554/h264cif";
// const username = "admin";
// const password = "123456";

// // Step 1: Create an RTSPClient instance
// const client = new RTSPClient(username, password);
// let headerWritten = false
// const stream = new Writable()
// // .h264 file header
// const H264_HEADER = Buffer.from([0x00,0x00,0x00,0x01]);
// let rtpPackets = [];


// // Step 2: Connect to a specified URL using the client instance.
// // "keepAlive" option is set to true by default
// // "connection" option is set to "udp" by default. 
// client.connect(url, { connection: "tcp" })
//   .then((details) => {
//     console.log("RTSP Connected", JSON.stringify(details, null, "  "));

//     if (details.codec == "H264") {
//         //const videoFile = fs.createWriteStream(filename + '.264');
//         // Step 4: Create H264Transport passing in the client, file, and details
//         // This class subscribes to the 'data' event, looking for the video payload
//         //const h264 = new H264Transport(client, videoFile, details);
//         // The "data" event is fired for every RTP packet.
//         client.on("data", (channel, data, packet) => {
//             if (once) {
//                 once = 0
//                 console.log("RTP:", "Channel=" + channel, "TYPE=" + packet.payloadType, "ID=" + packet.id, "TS=" + packet.timestamp, "M=" + packet.marker);
//                 console.log(channel)
//                 console.log(packet)
//                 console.log(data)
//             }

//             // Accumatate RTP packets
//             rtpPackets.push(packet.payload);
            
//             // When Marker is set to 1 pass the group of packets to processRTPFrame()
//             if (packet.marker == 1) {
                
//                 const nals = [];
//                 let partialNal = [];

//                 for (let i = 0; i < rtpPackets.length; i++) {
//                     const packet = rtpPackets[i];
//                     const nal_header_f_bit = (packet[0] >> 7) & 0x01;
//                     const nal_header_nri = (packet[0] >> 5) & 0x03;
//                     const nal_header_type = (packet[0] >> 0) & 0x1F;

//                     console.log(nal_header_type)

//                     if (nal_header_type >= 1 && nal_header_type <= 23) { // Normal NAL. Not fragmented
//                         nals.push(packet);
//                     } else if (nal_header_type == 24) { // Aggregation type STAP-A. Multiple NAls in one RTP Packet
//                         let ptr = 1; // start after the nal_header_type which was '24'
//                         // if we have at least 2 more bytes (the 16 bit size) then consume more data
//                         while (ptr + 2 < (packet.length - 1)) {
//                         let size = (packet[ptr] << 8) + (packet[ptr + 1] << 0);
//                         ptr = ptr + 2;
//                         nals.push(packet.slice(ptr,ptr+size));
//                         ptr = ptr + size;
//                         }
//                     } else if (nal_header_type == 25) { // STAP-B
//                         // Not supported
//                     } else if (nal_header_type == 26) { // MTAP-16
//                         // Not supported
//                     } else if (nal_header_type == 27) { // MTAP-24
//                         // Not supported
//                     } else if (nal_header_type == 28) { // Frag FU-A
//                         // NAL is split over several RTP packets
//                         // Accumulate them in a tempoary buffer
//                         // Parse Fragmentation Unit Header
//                         const fu_header_s = (packet[1] >> 7) & 0x01;  // start marker
//                         const fu_header_e = (packet[1] >> 6) & 0x01;  // end marker
//                         const fu_header_r = (packet[1] >> 5) & 0x01;  // reserved. should be 0
//                         const fu_header_type = (packet[1] >> 0) & 0x1F; // Original NAL unit header

//                         // Check Start and End flags
//                         if (fu_header_s == 1 && fu_header_e == 0) { // Start of Fragment}
//                         const reconstructed_nal_type = (nal_header_f_bit << 7)
//                                                         + (nal_header_nri << 5)
//                                                         + fu_header_type;
//                         partialNal = [];
//                         partialNal.push(reconstructed_nal_type);

//                         // copy the rest of the RTP payload to the temp buffer
//                         for (let x=2; x< packet.length;x++) partialNal.push(packet[x]);
//                         }

//                         if (fu_header_s == 0 && fu_header_e == 0) { // Middle part of fragment}
//                         for (let x=2; x< packet.length;x++) partialNal.push(packet[x]);
//                         }

//                         if (fu_header_s == 0 && fu_header_e == 1) { // End of fragment}
//                         for (let x=2; x< packet.length;x++) partialNal.push(packet[x]);
//                         nals.push(Buffer.from(partialNal));
//                         }
//                     } else if (nal_header_type == 29) { // Frag FU-B
//                         // Not supported
//                     }
//                 }

//                 // Write out all the NALs
//                 //console.log(nals.length)
//                 for (let x = 0; x < nals.length; x++) {
//                     //this.stream.write(H264_HEADER);
//                     //this.stream.write(nals[x]);
//                     console.log(nals[x])
//                 }
                
//                 rtpPackets = [];
//             }
//         });
//     }

//     // process connection details
//     const fmtp = details.mediaSource.fmtp[0]
//     const fmtpConfig = transform.parseParams(fmtp.config);
//     const splitSpropParameterSets = fmtpConfig['sprop-parameter-sets'].toString().split(',');
//     const sps_base64 = splitSpropParameterSets[0];
//     const pps_base64 = splitSpropParameterSets[1];
//     const sps = new Buffer(sps_base64, "base64");
//     const pps = new Buffer(pps_base64, "base64");
//     // stream.write(H264_HEADER);
//     // stream.write(sps);
//     // stream.write(H264_HEADER);
//     // stream.write(pps);

//     headerWritten = true;

//     // Step 5: Start streaming!
//     client.play();
//   })
//   .catch(e => console.log(e));

// let once = 1



// // The "controlData" event is fired for every RTCP packet.
// client.on("controlData", (channel, rtcpPacket) => {
//   console.log("RTCP:", "Channel=" + channel, "TS=" + rtcpPacket.timestamp, "PT=" + rtcpPacket.packetType);
// });

// // The "log" event allows you to optionally log any output from the library.
// // You can hook this into your own logging system super easily.
// /*
// client.on("log", (data, prefix) => {
//   console.log(prefix + ": " + data);
// });
// */

const calib = JSON.parse(fs.readFileSync("camera_calibration.json"))

let window = new Window()

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
    // Multiply the position by the matrix.
    vec3 vertex = a_position.xyz;
    gl_Position = u_projmatrix * u_viewmatrix * u_modelmatrix * vec4(vertex, 1);
    v_uv = vec2(a_texCoord.x, 1.-a_texCoord.y);

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
    outColor = vec4(v_uv, 0., 1.);
	outColor.rgb = texture(u_tex, v_uv).rrr;
	outColor.rgba = texture(u_tex, undistort(v_uv, u_centerfocal, u_radial, u_tangential)).rrrr;
}
`);
let shape_geom = glutils.makeQuad({
    min: [-1.5, -1],
    max: [1.5, 1, 1],
    // div: [1, 3, 3]
})
let shape_vao = glutils.createVao(gl, shape_geom, shape_program.id);
let feed_dim = [640, 480]
let feeds = [
    new RTSP2Tex("rtsp://192.168.86.179:554/h264cif?username=admin&password=123456", feed_dim),
    new RTSP2Tex("rtsp://192.168.86.178:554/h264cif?username=admin&password=123456", feed_dim),
    new RTSP2Tex("rtsp://192.168.86.177:554/h264cif?username=admin&password=123456", feed_dim),
    
    //new RTSP2Tex("rtsp://192.168.86.188:554/h264cif?username=admin&password=123456", feed_dim),
]

let feedpose = [
    [-1.5, 0.1, 0, -1.95],
    [0, 0, 0, -2.1],
    [1.8, -0.2, 0, -2.6],
]


window.draw = function() {

	let { t, dim } = this;

	let f = 0
    gl.viewport(0, 0, dim[0], dim[1]);
	gl.clearColor(0, 0, 0, 1);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

	// Compute the matrix
	let viewmatrix = mat4.create();
	let projmatrix = mat4.create();
	let modelmatrix = mat4.create();
	mat4.lookAt(viewmatrix, [0, 0, 2], [0, 0, 0], [0, 1, 0]);
	mat4.perspective(projmatrix, Math.PI/2, dim[0]/dim[1], 0.01, 30);

    gl.disable(gl.DEPTH_TEST)
	gl.depthMask(false)
    gl.enable(gl.BLEND);
	//gl.blendEquation(gl.FUNC_ADD)
	gl.blendFunc(gl.SRC_ALPHA, gl.ONE);
	//gl.blendFuncSeparate(gl.ONE, gl.ONE, gl.ZERO, gl.ONE_MINUS_SRC_ALPHA);

    for (let i=0; i<feeds.length; i++) {
        mat4.identity(modelmatrix);
        mat4.translate(modelmatrix, modelmatrix, feedpose[i])
        mat4.rotateZ(modelmatrix, modelmatrix, feedpose[i][3])
        feeds[i].tex.bind().submit()
        shape_program.begin();
            shape_program.uniform("u_modelmatrix", modelmatrix);
            shape_program.uniform("u_viewmatrix", viewmatrix);
            shape_program.uniform("u_projmatrix", projmatrix);
            shape_program.uniform("u_tex", 0);
            shape_program.uniform("u_centerfocal", calib.center[0]/calib.image_size[0], calib.center[1]/calib.image_size[1], calib.focal[0]/calib.image_size[0], calib.focal[1]/calib.image_size[1]);
            shape_program.uniform("u_tangential", calib.tangential);
            shape_program.uniform("u_radial", calib.radial);
            shape_vao.bind().draw().unbind();
        shape_program.end();
        feeds[i].tex.unbind()
    }

    gl.enable(gl.DEPTH_TEST)
	gl.depthMask(true)
    gl.disable(gl.BLEND)
}

let imagecount = 0


window.onkey = function(key, scan, down, mod) {
    if (down) {
        console.log(key, down, mod);
        if (key == 70) { // F
            // toggle fullscreen:
            this.setFullscreen(!this.fullscreen);
        } else if (key == 83) { // s = save images
            for (let i=0; i<feeds.length; i++) {
                const feed = feeds[i]
                const tex = feed.tex
                let options = {
                    width: feed.dim[0],
                    height: feed.dim[1]
                }
                let png = new PNG(options)
                for (let i=0; i<options.width * options.height; i++) {
                    // grayscale to RGBA
                    png.data[i*4+0] = tex.data[i]
                    png.data[i*4+1] = tex.data[i]
                    png.data[i*4+2] = tex.data[i]
                    png.data[i*4+3] = 0xff
                }
                let buffer = pnglib.sync.write(png, options);
                fs.writeFileSync(`feed${i}_${imagecount}.png`, buffer);
            }
            imagecount++
        }
    }
}

Window.animate()