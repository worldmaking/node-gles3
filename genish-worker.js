const assert = require('assert');
const { Worker, MessageChannel, MessagePort, isMainThread, parentPort, workerData, SHARE_ENV } = require('worker_threads');

if (isMainThread) {
	console.error("this script is intended to run as a worker thread, do not invoke it directly")
	process.exit()
}

// // `workerData` can be passed here in the `new Worker` constructor
// console.log(workerData);

// Set up audio driver
const audio = require('./audio.js');
//console.log("AUDIO DEVICES")
//console.log(audio.devices)
// start audio processing
// optional argument to specify settings, defaults to:
// {
// 	samplerate: 48000,
// 	indevice: 0,
// 	inchannels: 2,
// 	outdevice: 0,
// 	outchannels: 2,
// }
audio.start()
// console.log("AUDIO")
// console.log(audio)

// Now setup genish.js
const gen = require("./genish.js")
gen.samplerate = audio.samplerate
const { 
	add, sub, mul, div, mod, pow, exp, 
	abs, round, floor, ceil, min, max, sign, 
	delta, gate, selector, slide, ifelse, 
	bool, not, eq, neq, and, gt, gte, lt, lte, ltp, gtp, 
	sin, cos, tan, asin, acos, atan, tanh,
	accum, counter, phasor, phasorN, cycle, cycleN, 
	rate, train, 
	noise, dcblock, sah, latch,
	t60, mtof, mstosamps, 
	wrap, fold, clamp, mix, 
	param, history, memo, 
	attack, decay, env, ad, adsr, bang, pan, 
	data, peek, peekDyn, poke, delay, 
} = gen;
console.log("GEN")
console.log(gen)
// this will hold our generated audio code
// left undefined for now:
let kernel = gen.gen.createCallback(cycle(440), 2048)

// handle messages from main thread:
parentPort.on("message", (msg) => {
	if (typeof msg == "object") {
		switch(msg.cmd) {
			case "graph": {
				console.log("received graph from parent", msg.graph);
				// make a basic graph:
				let graph = eval(msg.graph)
				// 2nd argument here is a memory allocation
				// TODO we need to figure out how to assign this more sensibly
				kernel = gen.gen.createCallback(graph, 2048)
			} break;
			default:
				console.log("got message object from parent", msg);
		}
	} else {
		console.log("got message from parent", typeof msg, msg)
	}
})

// send a message back to parent:
// parentPort.postMessage("yo from thread")
// // parentPort.once('message', (msg) => {
// // 	assert(msg.hereIsYourPort instanceof MessagePort);
// // 	// send a message back:
// // 	msg.hereIsYourPort.postMessage('the worker is sending this');
// // 	//msg.hereIsYourPort.close();
// // });


// Now start the main loop of this audio worker thread:
let frameIdx = 0;
let time = 0; // in seconds
let lasttime = 0
function runAudioProcess() {
	let dt = time - lasttime // seconds since last update()
	lasttime = time
	// this is the time in the ringbuffer that has most recently been played (and is now zeroed)
	// so we are safe to fill the buffer up to this point:
	let at = audio.t
	let ouch = audio.outchannels
	let inch = audio.inchannels
	let secondsPerFrame = 1/audio.samplerate
	//console.log(at, time)
	// continue filling ringbuffer until we catch up to that point:
	while (frameIdx != at) {
		let inframe = audio.inbuffer.subarray(frameIdx*inch)
		let outframe = audio.outbuffer.subarray(frameIdx*ouch)
		// compute next output:
		let L = kernel ? kernel.call(kernel) : 0
		let R = L
		// write to output:
		outframe[0] += L;
		outframe[1] += R;
		// time passes:
		time += secondsPerFrame; 
		frameIdx = (frameIdx+1) % audio.frames;
	}

	// play for 10 seconds:
	if (time > 10) {
		audio.end()
		process.exit()
	} else {
		//console.log(dt)
		setTimeout(runAudioProcess, audio.pollms/2);
	}
}
runAudioProcess();