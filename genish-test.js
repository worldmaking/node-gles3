const assert = require('assert');
const { Worker, MessageChannel, MessagePort, isMainThread, parentPort, workerData, SHARE_ENV } = require('worker_threads');
// see https://nodejs.org/api/worker_threads.html#worker_threads_class_worker

/*
	This script is adapted from the test-audio.js script

	This script spawns a web-worker (think: a separate thread of Node.js) to do the actual audio processing. That way, any heavy processing in audio does not interrupt main-thread graphics, and any laggy processing in the main thread does not interrupt audio.
*/
// // wrap shared memory as a float array
// // this can now be accessible from both threads
// let sab = new Float32Array(new SharedArrayBuffer(1024));
// console.log("sab", sab)

// load a js file as a new Worker thread:
const worker = new Worker("./genish-worker.js", { 
	// argv: becomes process.argv in worker
	// eval: true if 1st arg is a string of code rather than a filepath
	//workerData: "hello"  // pass initial data to worker
});
// here's a message channel to receive messages from it:
const workerChannel = new MessageChannel();

// receicve a message:
worker.on('message', function(msg) {
	console.log("main received message from audio:", msg)
});
//worker.on('error', ...);
//worker.on('online', ...)
worker.on('exit', (code) => { 
	console.log(`Worker stopped with exit code ${code}`) 
	process.exit(code)
})

// send it a message:
// note that postMessage will copy data. 
// To transfer without copying, also list the item in an array as the 2nd argument of postMessage
// The exception is SharedArrayBuffer, which is not copied, and can be read/written on both sides
//worker.postMessage({ shared: sab })
// worker.postMessage({ 
// 	hereIsYourPort: workerChannel.port1 
// }, [workerChannel.port1]);
// // handle replies:
// workerChannel.port2.on('message', (value) => {
// 	console.log('received:', value);
// });


setInterval(()=>{
	let n = Math.ceil(Math.random()*10 + 20) * Math.random() * 10
	let graph
	graph = `cycle( add(${n}, mul(${n*2}, add(cycle(${n*2}), cycle(${Math.random()*10})))) )`

	worker.postMessage({ 
		cmd:"graph", 
		graph: graph
	})
}, 1000)