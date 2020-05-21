#include "node-api-helpers.h"

#include <k4a/k4a.h>

void napi_finalize_k4a_device(napi_env env, void* data, void* hint) {
	printf("closing kinect device %p\n", data);

	k4a_device_close((k4a_device_t)data);
}

void napi_finalize_k4a_capture(napi_env env, void* data, void* hint) {
	printf("releasing capture %p\n", data);

	k4a_capture_release((k4a_capture_t)data);
}

napi_value device_get_installed_count(napi_env env, const napi_callback_info info) {
    
	uint32_t count = k4a_device_get_installed_count();

	printf("found %d devices\n", count);
	napi_value ncount;
    assert(napi_ok == napi_create_int32(env, count, &ncount));
	return ncount;
}

napi_value device_open(napi_env env, const napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	uint32_t count = k4a_device_get_installed_count();
	uint32_t index;
	napi_get_value_uint32(env, argv[0], &index);
	if (index >= count) index = 0;


	// Open the first plugged in Kinect device
	k4a_device_t device = NULL;
	if (K4A_RESULT_SUCCEEDED == k4a_device_open(index, &device)) {

		napi_value ndevice;

		assert(napi_ok == napi_create_external(env, device, napi_finalize_k4a_device, nullptr, &ndevice));

		return ndevice;
	} else {
		// TODO throw error
		// napi_throw_error
		return nullptr;
	}
}

napi_value device_get_serialnum(napi_env env, const napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get device:
	napi_value ndevice = argv[0];
	napi_valuetype ndevice_type;
	assert(napi_ok == napi_typeof(env, ndevice, &ndevice_type));
	assert(ndevice_type == napi_external);
	k4a_device_t device;
	assert(napi_ok == napi_get_value_external(env, ndevice, &(void *)device));

	// Allocate memory for the serial, then acquire it
	size_t serial_size = 0;
	k4a_device_get_serialnum(device, NULL, &serial_size);
	char *serial = (char*)(malloc(serial_size));
	k4a_device_get_serialnum(device, serial, &serial_size);

	napi_value nserial;
    napi_create_string_utf8(env, serial, serial_size, &nserial);
	free(serial);
	return nserial;
}

napi_value device_start_cameras(napi_env env, const napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get device:
	napi_value ndevice = argv[0];
	napi_valuetype ndevice_type;
	assert(napi_ok == napi_typeof(env, ndevice, &ndevice_type));
	assert(ndevice_type == napi_external);
	k4a_device_t device;
	assert(napi_ok == napi_get_value_external(env, ndevice, &(void *)device));

	// Configure a stream of 4096x3072 BRGA color data at 15 frames per second
	k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
	config.color_format     = K4A_IMAGE_FORMAT_COLOR_BGRA32;
	config.color_resolution = K4A_COLOR_RESOLUTION_1080P;
	config.depth_mode 		= K4A_DEPTH_MODE_NFOV_UNBINNED;
	config.camera_fps       = K4A_FRAMES_PER_SECOND_30;
	config.synchronized_images_only = false; // set true if both depth and colour enabled
	config.depth_delay_off_color_usec = 0;
	config.wired_sync_mode = K4A_WIRED_SYNC_MODE_STANDALONE;
	config.subordinate_delay_off_master_usec = 0;
	config.disable_streaming_indicator = false;

	// Start the camera with the given configuration
	k4a_result_t result = k4a_device_start_cameras(device, &config);
	napi_value nresult;
	assert(napi_ok == napi_get_boolean(env, result, &nresult));
	return nresult;
}

napi_value device_start_imu(napi_env env, const napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get device:
	napi_value ndevice = argv[0];
	napi_valuetype ndevice_type;
	assert(napi_ok == napi_typeof(env, ndevice, &ndevice_type));
	assert(ndevice_type == napi_external);
	k4a_device_t device;
	assert(napi_ok == napi_get_value_external(env, ndevice, &(void *)device));

	// Start the camera with the given configuration
	k4a_result_t result = k4a_device_start_imu(device);
	napi_value nresult;
	assert(napi_ok == napi_get_boolean(env, result, &nresult));
	return nresult;
}

napi_value device_get_capture(napi_env env, const napi_callback_info info) {
	size_t argc = 2;
	napi_value argv[2];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get device:
	napi_value ndevice = argv[0];
	napi_valuetype ndevice_type;
	assert(napi_ok == napi_typeof(env, ndevice, &ndevice_type));
	assert(ndevice_type == napi_external);
	k4a_device_t device;
	assert(napi_ok == napi_get_value_external(env, ndevice, &(void *)device));

	// get timeout:
	int32_t timeout_in_ms = 1000/30;
	napi_get_value_int32(env, argv[1], &timeout_in_ms);
	
	k4a_capture_t capture;
	k4a_wait_result_t res = k4a_device_get_capture(device, &capture, timeout_in_ms);

	switch (res) {
	case K4A_WAIT_RESULT_SUCCEEDED: {
		napi_value ncapture;
		//assert(napi_ok == napi_create_external(env, capture, napi_finalize_k4a_capture, nullptr, &ncapture));
		assert(napi_ok == napi_create_external(env, capture, nullptr, nullptr, &ncapture));
		return ncapture;
	} break;
	//case K4A_WAIT_RESULT_FAILED:
		//printf("Failed to read a capture\n");
	}
	return nullptr;
}



napi_value device_get_imu_sample(napi_env env, const napi_callback_info info) {
	size_t argc = 2;
	napi_value argv[2];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get device:
	napi_value ndevice = argv[0];
	napi_valuetype ndevice_type;
	assert(napi_ok == napi_typeof(env, ndevice, &ndevice_type));
	assert(ndevice_type == napi_external);
	k4a_device_t device;
	assert(napi_ok == napi_get_value_external(env, ndevice, &(void *)device));

	// get timeout:
	int32_t timeout_in_ms = 0;
	napi_get_value_int32(env, argv[1], &timeout_in_ms);

	k4a_imu_sample_t imu_sample;
	k4a_device_get_imu_sample(device, &imu_sample, timeout_in_ms);

	// now, return the contents of the imu_sample_t as a javascript object?

	//printf("imu sample temp %f \n", imu_sample.temperature);

	return nullptr;
}


napi_value device_stop_imu(napi_env env, const napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get device:
	napi_value ndevice = argv[0];
	napi_valuetype ndevice_type;
	assert(napi_ok == napi_typeof(env, ndevice, &ndevice_type));
	assert(ndevice_type == napi_external);
	k4a_device_t device;
	assert(napi_ok == napi_get_value_external(env, ndevice, &(void *)device));

	k4a_device_stop_imu(device);
	return nullptr;
}


napi_value device_stop_cameras(napi_env env, const napi_callback_info info) {
	size_t argc = 1;
	napi_value argv[1];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get device:
	napi_value ndevice = argv[0];
	napi_valuetype ndevice_type;
	assert(napi_ok == napi_typeof(env, ndevice, &ndevice_type));
	assert(ndevice_type == napi_external);
	k4a_device_t device;
	assert(napi_ok == napi_get_value_external(env, ndevice, &(void *)device));

	// Start the camera with the given configuration
	k4a_device_stop_cameras(device);

	return nullptr;
}


napi_value capture_get_color_image(napi_env env, const napi_callback_info info) {
	size_t argc = 2;
	napi_value argv[2];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get capture:
	napi_value ncapture = argv[0];
	napi_valuetype ncapture_type;
	assert(napi_ok == napi_typeof(env, ncapture, &ncapture_type));
	assert(ncapture_type == napi_external);
	k4a_capture_t capture;
	assert(napi_ok == napi_get_value_external(env, ncapture, &(void *)capture));

	k4a_image_t image = k4a_capture_get_color_image(capture);
	if (!image) return nullptr;
	napi_value nimage;
	//assert(napi_ok == napi_create_external(env, capture, napi_finalize_k4a_capture, nullptr, &ncapture));
	assert(napi_ok == napi_create_external(env, image, nullptr, nullptr, &nimage));
	return nimage;
}

napi_value capture_get_depth_image(napi_env env, const napi_callback_info info) {
	size_t argc = 2;
	napi_value argv[2];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get capture:
	napi_value ncapture = argv[0];
	napi_valuetype ncapture_type;
	assert(napi_ok == napi_typeof(env, ncapture, &ncapture_type));
	assert(ncapture_type == napi_external);
	k4a_capture_t capture;
	assert(napi_ok == napi_get_value_external(env, ncapture, &(void *)capture));

	k4a_image_t image = k4a_capture_get_depth_image(capture);
	if (!image) return nullptr;
	napi_value nimage;
	//assert(napi_ok == napi_create_external(env, capture, napi_finalize_k4a_capture, nullptr, &ncapture));
	assert(napi_ok == napi_create_external(env, image, nullptr, nullptr, &nimage));
	return nimage;
}

napi_value capture_get_ir_image(napi_env env, const napi_callback_info info) {
	size_t argc = 2;
	napi_value argv[2];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get capture:
	napi_value ncapture = argv[0];
	napi_valuetype ncapture_type;
	assert(napi_ok == napi_typeof(env, ncapture, &ncapture_type));
	assert(ncapture_type == napi_external);
	k4a_capture_t capture;
	assert(napi_ok == napi_get_value_external(env, ncapture, &(void *)capture));

	k4a_image_t image = k4a_capture_get_ir_image(capture);
	if (!image) return nullptr;
	napi_value nimage;
	//assert(napi_ok == napi_create_external(env, capture, napi_finalize_k4a_capture, nullptr, &ncapture));
	assert(napi_ok == napi_create_external(env, image, nullptr, nullptr, &nimage));
	return nimage;
}

napi_value capture_release(napi_env env, const napi_callback_info info) {
	size_t argc = 2;
	napi_value argv[2];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get capture:
	napi_value ncapture = argv[0];
	napi_valuetype ncapture_type;
	assert(napi_ok == napi_typeof(env, ncapture, &ncapture_type));
	assert(ncapture_type == napi_external);
	k4a_capture_t capture;
	assert(napi_ok == napi_get_value_external(env, ncapture, &(void *)capture));
	
	k4a_capture_release(capture);

	// want to trash the ncapture now -- can it be done?

	return nullptr;
}


napi_value image_get_width_pixels(napi_env env, const napi_callback_info info) {
	size_t argc = 2;
	napi_value argv[2];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get image:
	napi_value nimage = argv[0];
	napi_valuetype n_type;
	assert(napi_ok == napi_typeof(env, nimage, &n_type));
	assert(n_type == napi_external);
	k4a_image_t image;
	assert(napi_ok == napi_get_value_external(env, nimage, &(void *)image));
	
	int px = k4a_image_get_width_pixels(image);
	napi_value res = nullptr;
	assert(napi_ok == napi_create_int32(env, px, &res));
	return res;
}
napi_value image_get_height_pixels(napi_env env, const napi_callback_info info) {
	size_t argc = 2;
	napi_value argv[2];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get image:
	napi_value nimage = argv[0];
	napi_valuetype n_type;
	assert(napi_ok == napi_typeof(env, nimage, &n_type));
	assert(n_type == napi_external);
	k4a_image_t image;
	assert(napi_ok == napi_get_value_external(env, nimage, &(void *)image));
	
	int px = k4a_image_get_height_pixels(image);
	napi_value res = nullptr;
	assert(napi_ok == napi_create_int32(env, px, &res));
	return res;
}
napi_value image_get_stride_bytes(napi_env env, const napi_callback_info info) {
	size_t argc = 2;
	napi_value argv[2];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get image:
	napi_value nimage = argv[0];
	napi_valuetype n_type;
	assert(napi_ok == napi_typeof(env, nimage, &n_type));
	assert(n_type == napi_external);
	k4a_image_t image;
	assert(napi_ok == napi_get_value_external(env, nimage, &(void *)image));
	
	int px = k4a_image_get_stride_bytes(image);
	napi_value res = nullptr;
	assert(napi_ok == napi_create_int32(env, px, &res));
	return res;
}

napi_value image_get_buffer(napi_env env, const napi_callback_info info) {
	size_t argc = 2;
	napi_value argv[2];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get image:
	napi_value nimage = argv[0];
	napi_valuetype n_type;
	assert(napi_ok == napi_typeof(env, nimage, &n_type));
	assert(n_type == napi_external);
	k4a_image_t image;
	assert(napi_ok == napi_get_value_external(env, nimage, &(void *)image));
	
	uint8_t * bytes = k4a_image_get_buffer(image);
	size_t length = k4a_image_get_size(image);
	napi_value res = nullptr;
	assert(napi_ok == napi_create_external_arraybuffer(env, bytes, length, nullptr, nullptr, &res));
	return res;
}

napi_value image_release(napi_env env, const napi_callback_info info) {
	size_t argc = 2;
	napi_value argv[2];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get image:
	napi_value nimage = argv[0];
	napi_valuetype n_type;
	assert(napi_ok == napi_typeof(env, nimage, &n_type));
	assert(n_type == napi_external);
	k4a_image_t image;
	assert(napi_ok == napi_get_value_external(env, nimage, &(void *)image));
	
	k4a_image_release(image);

	// want to trash the nimage now -- can it be done?

	return nullptr;
}

napi_value Init(napi_env env, napi_value exports) {

    napi_property_descriptor export_properties[] = {
		{ "device_get_installed_count", nullptr, device_get_installed_count, nullptr, nullptr, nullptr, napi_default, nullptr	},
		{ "device_open", nullptr, device_open, nullptr, nullptr, nullptr, napi_default, nullptr	},
		{ "device_get_serialnum", nullptr, device_get_serialnum, nullptr, nullptr, nullptr, napi_default, nullptr	},
		// k4a_device_get_version
		{ "device_start_cameras", nullptr, device_start_cameras, nullptr, nullptr, nullptr, napi_default, nullptr	},
		{ "device_stop_cameras", nullptr, device_stop_cameras, nullptr, nullptr, nullptr, napi_default, nullptr	},
		{ "device_start_imu", nullptr, device_start_imu, nullptr, nullptr, nullptr, napi_default, nullptr	},
		{ "device_stop_imu", nullptr, device_stop_imu, nullptr, nullptr, nullptr, napi_default, nullptr	},
		{ "device_get_capture", nullptr, device_get_capture, nullptr, nullptr, nullptr, napi_default, nullptr	},
		{ "device_get_imu_sample", nullptr, device_get_imu_sample, nullptr, nullptr, nullptr, napi_default, nullptr	},
		// k4a_device_get_calibration, k4a_device_get_raw_calibration
		// k4a_device_get_color_control, k4a_device_set_color_control

		{ "capture_get_color_image", nullptr, capture_get_color_image, nullptr, nullptr, nullptr, napi_default, nullptr	},
		{ "capture_get_depth_image", nullptr, capture_get_depth_image, nullptr, nullptr, nullptr, napi_default, nullptr	},
		{ "capture_get_ir_image", nullptr, capture_get_ir_image, nullptr, nullptr, nullptr, napi_default, nullptr	},
		// k4a_capture_get_temperature_c
		{ "capture_release", nullptr, capture_release, nullptr, nullptr, nullptr, napi_default, nullptr	},

		
		{ "image_get_height_pixels", nullptr, image_get_height_pixels, nullptr, nullptr, nullptr, napi_default, nullptr	},
		{ "image_get_width_pixels", nullptr, image_get_width_pixels, nullptr, nullptr, nullptr, napi_default, nullptr	},
		{ "image_get_stride_bytes", nullptr, image_get_stride_bytes, nullptr, nullptr, nullptr, napi_default, nullptr	},
		{ "image_get_buffer", nullptr, image_get_buffer, nullptr, nullptr, nullptr, napi_default, nullptr	},
		// k4a_image_get_format
		// k4a_image_get_timestamp_usec
		// k4a_image_get_device_timestamp_usec
		// k4a_image_get_system_timestamp_nsec
		// k4a_image_get_exposure_usec
		// k4a_image_get_white_balance
		// k4a_image_get_iso_speed
		{ "image_release", nullptr, image_release, nullptr, nullptr, nullptr, napi_default, nullptr	},

		// k4a_calibration_color_2d_to_depth_2d
		// k4a_calibration_get_from_raw
		// k4a_calibration_3d_to_3d
		// k4a_calibration_2d_to_3d
		// k4a_calibration_2d_to_2d
		// k4a_calibration_3d_to_2d

		// k4a_transformation_create, k4a_transformation_destroy
		// k4a_transformation_depth_image_to_color_camera
		// k4a_transformation_depth_image_to_color_camera_custom
		// k4a_transformation_depth_image_to_point_cloud
		// k4a_transformation_color_image_to_depth_camera
	};
    assert(napi_define_properties(env, exports, 
	sizeof(export_properties) / sizeof(export_properties[0]), export_properties) == napi_ok);
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)