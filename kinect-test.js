const assert = require("assert")
const k4a = require("./k4a.js");
console.log(k4a);


async function run() {
	
	let dev = k4a.device_open()
	console.log(dev)
	if (!dev) return;
	console.log(k4a.device_get_serialnum(dev))

	assert(!k4a.device_start_cameras(dev) )   // returns true on error
	assert(!k4a.device_start_imu(dev) )   // returns true on error

	for (let i=0; i<50; i++) {

		//await sleep(1000/30)
		let capture = k4a.device_get_capture(dev)
		if (capture) {

			{let image = k4a.capture_get_color_image(capture);
			if (image) {
				let w = k4a.image_get_width_pixels(image);
				let h = k4a.image_get_height_pixels(image);
				let s = k4a.image_get_stride_bytes(image);
				let buf = k4a.image_get_buffer(image);
				console.log("color", w, h, s, h*s, buf)

				k4a.image_release(image);
			}}
			{let image = k4a.capture_get_depth_image(capture);
			if (image) {
				let w = k4a.image_get_width_pixels(image);
				let h = k4a.image_get_height_pixels(image);
				let s = k4a.image_get_stride_bytes(image);
				let buf = k4a.image_get_buffer(image);
				console.log("depth", w, h, s, h*s, buf)

				k4a.image_release(image);
			}}
			{let image = k4a.capture_get_ir_image(capture);
			if (image) {
				let w = k4a.image_get_width_pixels(image);
				let h = k4a.image_get_height_pixels(image);
				let s = k4a.image_get_stride_bytes(image);
				let buf = k4a.image_get_buffer(image);
				console.log("ir", w, h, s, h*s, buf)

				k4a.image_release(image);
			}}
			k4a.capture_release(capture);
			
			k4a.device_get_imu_sample(dev);
		}
	}
	k4a.device_stop_imu(dev);
	k4a.device_stop_cameras(dev)
}

function sleep(ms) {
	return new Promise((resolve) => {
		setTimeout(resolve, ms);
	});
}  

try {
	run();
} catch(e) {
	console.error(e);
}