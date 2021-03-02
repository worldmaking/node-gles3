const audio = require('./audio.js');

/*
- This should be running in a Worker. 
- Worker's job is to fill the buffer up to whatever index it is at.
*/

/* 
List available playback and capture devices & their features
For example: 

{
  playback: [
    {
      index: 0,
      name: 'Built-in Output',
      minChannels: 2,
      maxChannels: 2,
      minSampleRate: 44100,
      maxSampleRate: 96000
    },
    {
      index: 1,
      name: 'iShowU Audio Capture',
      minChannels: 2,
      maxChannels: 2,
      minSampleRate: 44100,
      maxSampleRate: 192000
    },
    {
      index: 2,
      name: 'ZoomAudioDevice',
      minChannels: 2,
      maxChannels: 2,
      minSampleRate: 48000,
      maxSampleRate: 48000
    }
  ],
  capture: [
    {
      index: 0,
      name: 'Built-in Microphone',
      minChannels: 2,
      maxChannels: 2,
      minSampleRate: 44100,
      maxSampleRate: 96000
    },
    {
      index: 1,
      name: 'NDI Audio',
      minChannels: 2,
      maxChannels: 2,
      minSampleRate: 44100,
      maxSampleRate: 48000
    },
    {
      index: 2,
      name: 'iShowU Audio Capture',
      minChannels: 2,
      maxChannels: 2,
      minSampleRate: 44100,
      maxSampleRate: 192000
    },
    {
      index: 3,
      name: 'ZoomAudioDevice',
      minChannels: 2,
      maxChannels: 2,
      minSampleRate: 48000,
      maxSampleRate: 48000
    }
  ]
}
*/
console.log(audio.devices)

// start audio processing
// optional argument to specify settings
audio.start({
	// these are the defaults:
	samplerate: 48000,
	indevice: 0,
	inchannels: 2,
	outdevice: 0,
	outchannels: 2,
})

/*
	e.g.:
{
  outname: 'Built-in Output',
  inname: 'Built-in Microphone',
  outchannels: 2,
  inchannels: 2,
  outbuffer: Float32Array(2880),
  inbuffer: Float32Array(2880),
  samplerate: 48000,
  frames: 1440,
  latency: 0.029999999329447746,
  pollms: 15
}
*/
console.log(audio)

// minimal sine oscillator:
nextSample = (function() {
	let phase = 0;
	let radiansPerFrame = Math.PI * 2 * 440/audio.samplerate;
	return function () {
		// compute next output:
		phase += radiansPerFrame;
		return 0.1 * Math.sin(phase);
	}
})();

let frameIdx = 0;
let time = 0; // in seconds
let lasttime = 0
function update() {
	let dt = time - lasttime // seconds since last update()
	lasttime = time
	// this is the time in the ringbuffer that has most recently been played (and is now zeroed)
	// so we are safe to fill the buffer up to this point:
	let at = audio.t
	let outch = audio.outchannels
	let inch = audio.inchannels
	let secondsPerFrame = 1/audio.samplerate
	//console.log(at, time)
	// continue filling ringbuffer until we catch up to that point:
	while (frameIdx != at) {
		let inframe = audio.inbuffer.subarray(frameIdx*inch)
		let outframe = audio.outbuffer.subarray(frameIdx*outch)
		// compute next output:
		let L = inframe[0]
		let R = nextSample(time)
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
		console.log(audio)
	} else {
		//console.log(dt)
		setTimeout(update, audio.pollms); // 300ms is enough. how??
	}
}

update();