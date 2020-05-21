const ffi = require('ffi'),
	ref = require('ref'),
	struct = require('ref-struct'),
	refarray = require('ref-array');

const size_t = ref.types.size_t
const size_t_ptr = ref.refType(size_t)
const uint32_t = ref.types.uint32;
const char = ref.types.char
const char_ptr = ref.refType(char)
const k4a_device_t = ref.refType(ref.types.void);
const k4a_device_ptr = ref.refType(k4a_device_t);

const k4a_image_format_t = ref.types.int;
const k4a_color_resolution_t = ref.types.int;
const k4a_fps_t = ref.types.int;
const k4a_wired_sync_mode_t = ref.types.int;
const k4a_depth_mode_t = ref.types.int;

const k4a_device_configuration_t = struct({
	color_format: k4a_image_format_t,
	color_resolution: k4a_color_resolution_t,
	depth_mode: k4a_depth_mode_t,
	camera_fps: k4a_fps_t,
	synchronized_images_only: ref.types.bool,
	depth_delay_off_color_usec: ref.types.int32,
	wired_sync_mode: k4a_wired_sync_mode_t,
	subordinate_delay_off_master_usec: ref.types.uint32,
	disable_streaming_indicator: ref.types.bool
});

const k4a_float3_t = struct({
	x: ref.types.float,
	y: ref.types.float,
	z: ref.types.float,
})

const k4a_imu_sample_t = struct({
	temperature: ref.types.float,            /**< Temperature reading of this sample (Celsius). */
    acc_sample: k4a_float3_t,      /**< Accelerometer sample in meters per second squared. */
    acc_timestamp_usec: ref.types.uint64,  /**< Timestamp of the accelerometer in microseconds. */
    gyro_sample: k4a_float3_t,     /**< Gyro sample in radians per second. */
    gyro_timestamp_usec: ref.types.uint64, /**< Timestamp of the gyroscope in microseconds */
})
const k4a_imu_sample_ptr = ref.refType(k4a_imu_sample_t)

const k4a_device_configuration_ptr = ref.refType(k4a_device_configuration_t);

const lib = ffi.Library('build/Release/k4a.dll', {
	k4a_device_get_installed_count: [uint32_t, []],
	k4a_device_open: ['int', [uint32_t, k4a_device_ptr]],
	k4a_device_close: ['int', [k4a_device_t]],
	k4a_device_get_serialnum: ['int', [k4a_device_t, char_ptr, size_t_ptr]],
	k4a_device_start_cameras: ['int', [k4a_device_t, k4a_device_configuration_ptr]],
	k4a_device_stop_cameras: ['int', [k4a_device_t]],


	k4a_device_get_imu_sample: ['int', [k4a_device_t, k4a_imu_sample_ptr, ref.types.int32]],
});

lib.K4A_RESULT_SUCCEEDED = 0;
lib.K4A_RESULT_FAILED = 1;


lib.K4A_IMAGE_FORMAT_COLOR_MJPG = 0;
lib.K4A_IMAGE_FORMAT_COLOR_NV12 = 1;
lib.K4A_IMAGE_FORMAT_COLOR_YUY2 = 2;
lib.K4A_IMAGE_FORMAT_COLOR_BGRA32 = 3;
lib.K4A_IMAGE_FORMAT_DEPTH16 = 4;
lib.K4A_IMAGE_FORMAT_IR16 = 5;
lib.K4A_IMAGE_FORMAT_CUSTOM8 = 6;
lib.K4A_IMAGE_FORMAT_CUSTOM16 = 7;
lib.K4A_IMAGE_FORMAT_CUSTOM = 8;

lib.K4A_COLOR_RESOLUTION_OFF = 0; /**< Color camera will be turned off with this setting */
lib.K4A_COLOR_RESOLUTION_720P = 1;    /**< 1280 * 720  16:9 */
lib.K4A_COLOR_RESOLUTION_1080P = 2;   /**< 1920 * 1080 16:9 */
lib.K4A_COLOR_RESOLUTION_1440P = 3;   /**< 2560 * 1440 16:9 */
lib.K4A_COLOR_RESOLUTION_1536P = 4;   /**< 2048 * 1536 4:3  */
lib.K4A_COLOR_RESOLUTION_2160P = 5;   /**< 3840 * 2160 16:9 */
lib.K4A_COLOR_RESOLUTION_3072P = 6;   /**< 4096 * 3072 4:3  */

lib.K4A_DEPTH_MODE_OFF = 0,        /**< Depth sensor will be turned off with this setting. */
lib.K4A_DEPTH_MODE_NFOV_2X2BINNED = 1, /**< Depth captured at 320x288. Passive IR is also captured at 320x288. */
lib.K4A_DEPTH_MODE_NFOV_UNBINNED = 2,  /**< Depth captured at 640x576. Passive IR is also captured at 640x576. */
lib.K4A_DEPTH_MODE_WFOV_2X2BINNED = 3, /**< Depth captured at 512x512. Passive IR is also captured at 512x512. */
lib.K4A_DEPTH_MODE_WFOV_UNBINNED = 4,  /**< Depth captured at 1024x1024. Passive IR is also captured at 1024x1024. */
lib.K4A_DEPTH_MODE_PASSIVE_IR = 5

lib.K4A_FRAMES_PER_SECOND_5 = 0, /**< 5 FPS */
lib.K4A_FRAMES_PER_SECOND_15 = 1,    /**< 15 FPS */
lib.K4A_FRAMES_PER_SECOND_30 = 2,    /**< 30 FPS */

lib.K4A_WIRED_SYNC_MODE_STANDALONE = 0, /**< Neither 'Sync In' or 'Sync Out' connections are used. */
lib.K4A_WIRED_SYNC_MODE_MASTER = 1,     /**< The 'Sync Out' jack is enabled and synchronization data it driven out the
								   connected wire. While in master mode the color camera must be enabled as part of the
								   multi device sync signalling logic. Even if the color image is not needed, the color
								   camera must be running.*/
lib.K4A_WIRED_SYNC_MODE_SUBORDINATE = 2

//////////////////////////////////////////////////////////////////////////////////////

function sleep(ms) {
	return new Promise((resolve) => {
	  setTimeout(resolve, ms);
	});
}  

async function init() {
	console.log("num devices:", lib.k4a_device_get_installed_count())

	// get device:
	let devptr = ref.alloc(k4a_device_ptr)
	if (lib.K4A_RESULT_SUCCEEDED == lib.k4a_device_open(0, devptr)) {  // 1st arg is the device index
		let dev = devptr.deref()

		// get serial:
		let sizeptr = ref.alloc(size_t_ptr) 
		lib.k4a_device_get_serialnum(dev, null, sizeptr)
		let buf = new Uint8Array(sizeptr[0])
		lib.k4a_device_get_serialnum(dev, buf, sizeptr)
		console.log("serial:", String.fromCharCode.apply(null, buf))

		let config = new k4a_device_configuration_t({
			color_format: lib.K4A_IMAGE_FORMAT_COLOR_BGRA32,
			color_resolution: lib.K4A_COLOR_RESOLUTION_1080P,
			depth_mode: lib.K4A_DEPTH_MODE_OFF,
			camera_fps: lib.K4A_FRAMES_PER_SECOND_30,
			synchronized_images_only: true,
			depth_delay_off_color_usec: 0,
			wired_sync_mode: lib.K4A_WIRED_SYNC_MODE_STANDALONE,
			subordinate_delay_off_master_usec: 0,
			disable_streaming_indicator: false
		})

		if (lib.K4A_RESULT_SUCCEEDED == lib.k4a_device_start_cameras(dev, config.ref())) {

			let imu = new k4a_imu_sample_t();

			for (let i=0; i<5; i++) {

				await sleep(1000)

				lib.k4a_device_get_imu_sample(dev, imu.ref(), 0);

				console.log(imu)
			}

			lib.k4a_device_stop_cameras(dev);
		}
		lib.k4a_device_close(dev);
	};
} // init()

init();