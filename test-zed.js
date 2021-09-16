const zed = require('bindings')('zed.node');
console.log("Devices", zed.devices)
let cam = new zed.Camera().open()
let cloud = cam.cloud
let normals = cam.normals

console.log(cloud.length)

if (cam.isOpened()) {
	setInterval(()=>{
		cam.grab()
		console.log("cloud", cloud[6])
	}, 100)
}

console.log("done")