const assert = require('assert');
const { Worker, MessageChannel, MessagePort, isMainThread, parentPort, workerData, SHARE_ENV } = require('worker_threads');
// see https://nodejs.org/api/worker_threads.html#worker_threads_class_worker

/*
	Ops
*/
let opsList = [
	{
		"op": "cycle",
		"classification": "source",
		"inputs": {
			"freq": { kind: "knob", default: 1, range: [0, 1000], help: "wave frequency" },
			"reset": { kind: "jack", default: 0, help: "phase reset on nonzero" }
		},
		"outputs": {
			"out1": { help: "sinusoidal waveform", range: [-1, 1] }
		}
	},
	{
		"op": "phasor",
		"classification": "source",
		"inputs": {
			"freq": { kind: "knob", default: 1, range: [0, 1000], help: "wave frequency" },
			"reset": { kind: "jack", default: 0, help: "phase reset on nonzero" }
		},
		"outputs": {
			"out1": { help: "bipolar saw waveform", range: [-1, 1] }
		}
	},
	{
		"op": "speaker",
		"classification": "throughput",
		"inputs": {
			"in": { kind: "jack", default: 0, help: "output sound" }
		},
		"outputs": {}
	},
]
/*
	Approximate structure of graph from automerge
*/
let doc = {
	cycle_modltr: {
		uuid: "modltr",
		outputs: [
			{
				name: "sine",
				connections: {
					phasor_carrier_freq: 'cable',
					speaker_spkr_in: 'cable'
				}
			}
		],
		inputs: [
			{
				name: 'freq',
				kind: 'knob', // if cable is not plugged in
				value: 200
			},
			{
				name: 'phase',
				kind: 'jack' // if cable is plugged in
			}
		]
	},
	phasor_carrier: {
		uuid: "carrier",
		outputs: [
			{
				name: "phasor",
				connections: {
					speaker_spkr_in: 'cable'
				}
			}
		],
		inputs: [
			{
				name: 'freq',
				kind: 'jack'
			},
			{
				name: 'reset',
				kind: 'jack'
			}
		]
	},
	speaker_spkr: {
		uuid: "spkr",
		inputs: [
			{
				name: 'in',
				kind: 'jack'
			}
		]
	}
}

let doc1 = {
	"71624ca3-1465-42a2-ba7b-73bff030086e": {
		"category": "source",
		"inputs": [
			{
				"_props": {
					"index": 0,
					"kind": "inlet"
				},
				"name": "freq"
			},
			{
				"_props": {
					"index": 1,
					"kind": "inlet"
				},
				"name": "phase"
			}
		],
		"name": "cycle",
		"outputs": [
			{
				"name": "out1",
				"connections": {},
			},
			{
				"_props": {
					"history": false,
					"index": 1,
					"kind": "outlet"
				},
				"connections": {},
				"name": "out2"
			}
		],
		"position": [
			0.3747916519641876,
			1.6486183404922485,
			-1.412360668182373
		],
		"quaternion": [
			0.07021407425411384,
			-0.27171729228014174,
			0.005961854776044823,
			0.9597937969259097
		],
		"uuid": "71624ca3-1465-42a2-ba7b-73bff030086e"
	},
	"93854ca3-1465-42a2-ba7b-73bff030086e": {
		"category": "source",
		"inputs": [
			{
				"_props": {
					"index": 0,
					"kind": "inlet"
				},
				"name": "freq"
			},
			{
				"_props": {
					"index": 1,
					"kind": "inlet"
				},
				"name": "phase"
			}
		],
		"name": "cycle",
		"outputs": [
			{
				"name": "out1",
				"connections": {},
			},
			{
				"_props": {
					"history": false,
					"index": 1,
					"kind": "outlet"
				},
				"connections": {},
				"name": "out2"
			}
		],
		"position": [
			0.3747916519641876,
			1.6486183404922485,
			-1.412360668182373
		],
		"quaternion": [
			0.07021407425411384,
			-0.27171729228014174,
			0.005961854776044823,
			0.9597937969259097
		],
		"uuid": "71624ca3-1465-42a2-ba7b-73bff030086e"
	},
	"48a3c67a-1f82-4c7c-a54e-944f62f4567d": {
		"category": "source",
		"inputs": [
			{
				"_props": {
					"index": 0,
					"kind": "inlet"
				},
				"name": "freq"
			},
			{
				"_props": {
					"index": 1,
					"kind": "inlet"
				},
				"name": "phase"
			}
		],
		"name": "cycle",
		"outputs": [
			{
				"_props": {
					"history": false,
					"index": 0,
					"kind": "outlet"
				},
				"connections": {},
				"name": "out1"
			},
			{
				"_props": {
					"history": false,
					"index": 1,
					"kind": "outlet"
				},
				"connections": {},
				"name": "out2"
			}
		],
		"position": [
			0.6048542857170105,
			1.6396149396896362,
			-1.2471022605895996
		],
		"quaternion": [
			0.059283288374310406,
			-0.3955686397100479,
			-0.005165422289528393,
			0.9165065484669049
		],
		"uuid": "48a3c67a-1f82-4c7c-a54e-944f62f4567d"
	}
}

let doc2 = {
	"71624ca3-1465-42a2-ba7b-73bff030086e": {
		uuid: "71624ca3-1465-42a2-ba7b-73bff030086e",
		name: "cycle",
		inputs: [
			{
				name: 'freq',
				kind: 'knob', 
				value: 200  
			},
			{
				name: 'reset',
				kind: 'jack', 
				value: 0
			}
		],
		outputs: [
			{
				name: "sine",
				connections: {
					"48a3c67a-1f82-4c7c-a54e-944f62f4567d": {
						"input": 'cable',
					}
				}
			}
		],
		"position": [
			0.3747916519641876,
			1.6486183404922485,
			-1.412360668182373
	  	],
	  	"quaternion": [
			0.07021407425411384,
			-0.27171729228014174,
			0.005961854776044823,
			0.9597937969259097
	 	],
	},
	"93854ca3-1465-42a2-ba7b-73bff030086e": {
		"name": "phasor",
		"uuid": "93854ca3-1465-42a2-ba7b-73bff030086e",
		"category": "source",
		"inputs": [
			{
				"name": "freq",
				"kind": "knob",
				"value": 50
			},
			{
				"name": "reset",
				"kind": "jack",
				"value": 0
			}
		],
		"outputs": [
			{
				"name": "out1",
				"connections": {
					"48a3c67a-1f82-4c7c-a54e-944f62f4567d": {
						"input": "cable"
					}
				},
			}
		],
		"position": [
			0.3747916519641876,
			1.6486183404922485,
			-1.412360668182373
		],
		"quaternion": [
			0.07021407425411384,
			-0.27171729228014174,
			0.005961854776044823,
			0.9597937969259097
		]
	},
	"48a3c67a-1f82-4c7c-a54e-944f62f4567d": {
		uuid: "48a3c67a-1f82-4c7c-a54e-944f62f4567d",
		name: "speaker",
		inputs: [
			{
				name: 'input',
				kind: 'jack',
				value: 0
			}
		],
		outputs: [],
		"position": [
			0.6051448583602905,
			1.6397514343261719,
			-1.2468757629394531
	  	],
	  	"quaternion": [
			0.05902307659135185,
			-0.39606825426548437,
			-0.0058165816976738895,
			0.9163036426121353
	  	],
	}
}

function rebuild(doc) {
	let id = 0;
	function makeUID(name) { return name + (id++) }

	// first, convert the graph to a format that is more useful for us here:
	let chain = []
	let cables = []
	let lastobj

	// loop over the objects in the doc:
	Object.keys(doc).forEach(uuid => {
		let obj = doc[uuid]
		lastobj = obj
		// // add a "connections" field to all the object's inputs:
		// if (obj.inputs) obj.inputs.forEach(input => {
		// 	input.connections = []
		// })
		// find all the cable connections:
		if (obj.outputs) obj.outputs.forEach(output => {
			if (output.connections) Object.entries(output.connections).forEach(([dst, conn]) => {
				Object.entries(conn).forEach(([input, type]) => {
					cables.push({ src: uuid, output: output.name, dst, input, type })
				})
			})
		})

		// for any end-of-chain items:
		if (obj.op == "speaker") {
			chain.push(obj)
		}
	})
	if (chain.length < 1) chain.push(lastobj)

	let operations = []
	let memo = []
	for (let i = 0; i < chain.length; i++) {
		let obj = chain[i]
		// ensure we only process each object once:
		if (memo[obj.uuid]) continue;
		memo[obj.uuid] = 1

		// build a default statement for this op:
		let op = {
			name: obj.name,
			uuid: obj.uuid,
			inputs: obj.inputs ? obj.inputs.map(input => input.value || 0) : [],
			outputs: obj.outputs ? obj.outputs.map(out => `${out.name}_${obj.uuid}`) : []
		}
		operations.push(op)

		// now refine the operation to fill in the inputs according to the patch cables

		// first, get the cables that connect to this object
		let conns = cables.filter(conn => conn.dst == obj.uuid)
		// for each input 
		if (obj.inputs) obj.inputs.forEach((input, i) => {
			// get the cables that connect to this input
			let inputs = conns.filter(conn => conn.input == input.name).map(conn => {
				// for any op that we used, add it to the chain:
				if (!memo[conn.src]) chain.push(doc[conn.src])
				return `${conn.output}_${conn.src}`
			})
			console.log(inputs)

			while (inputs.length > 1) {
				// combine into an add operation:
				let id = makeUID("add_")
				operations.push({
					name: "add",
					uuid: id,
					inputs: [inputs.pop(), inputs.pop()],
					outputs: [id]
				})
				inputs.push(id)
			}
			if (inputs.length) op.inputs[i] = inputs[0]
		})
	}

	// this will have built them in reverse order (by pulling from the outputs)
	// we want to reverse this to generate code
	operations.reverse()

	console.log("operations = ", JSON.stringify(operations, null, "  "))
	// the final stage will be to iterate over a list of statements, looking a bit like this:
	/*

	{
		outputs: ["mdltr_sine"]
		op: "cycle",
		args: [200, 0]
	},
	{
		outputs: ["carrier_phasor"]
		op: "phasor",
		args: ["modltr_sine", 0]
	},
	{
		outputs: ["spkr_output"]
		op: "speaker",
		args: ["carrier_phasor"]
	},
	

		makeUID("modltr")
		nodes["modltr_sine"] = genish["cycle"](200, 0)

		makeUID("carrier")
		nodes["carrier_phasor"] = genish["phasor"](nodes["modltr_sine"], 0)

		makeUID("spkr")
		nodes["spkr_output"] = nodes["carrier_phasor"]

		let graph = nodes["spkr_output"]
	*/

}

rebuild(doc2)

/*
	This script is adapted from the test-audio.js script

	This script spawns a web-worker (think: a separate thread of Node.js) to do the actual audio processing. 
	That way, any heavy processing in audio does not interrupt main-thread graphics, 
	and any laggy processing in the main thread does not interrupt audio.
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
//const workerChannel = new MessageChannel();

// receicve a message:
worker.on('message', function (msg) {
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

// // periodically send random graphs:
// setInterval(() => {
// 	let n = Math.ceil(Math.random() * 10 + 20) * Math.random() * 10
// 	let graph
// 	graph = `cycle( add(${n}, mul(${n * 2}, add(cycle(${n * 2}), cycle(${Math.random() * 10})))) )`

// 	worker.postMessage({
// 		cmd: "graph",
// 		graph: graph
// 	})
// }, 1000)

// kill the audio thread after 10 seconds
setTimeout(() => {
	worker.postMessage({ cmd: "end" })
}, 10 * 1000)

