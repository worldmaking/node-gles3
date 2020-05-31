#ifndef GL_KINECT_AZURE_H
#define GL_KINECT_AZURE_H

#include <k4a/k4a.h>
#include "al/al_glm.h"

// see https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/examples/fastpointcloud/main.cpp
/*
The example precomputes a lookup table by storing x- and y-scale factors for every pixel. At runtime, the 3d XY-coordinates of a pixel in millimeters are derived by multiplying the pixel's depth value with the corresponding xy-scale factors. This method represents an alternative to calling k4a_transformation_depth_image_to_point_cloud().
*/

/*static void write_point_cloud(const char *file_name, const k4a_image_t point_cloud, int point_count)
{
    int width = k4a_image_get_width_pixels(point_cloud);
    int height = k4a_image_get_height_pixels(point_cloud);

    k4a_float3_t *point_cloud_data = (k4a_float3_t *)(void *)k4a_image_get_buffer(point_cloud);

    // save to the ply file
    std::ofstream ofs(file_name); // text mode first
    ofs << "ply" << std::endl;
    ofs << "format ascii 1.0" << std::endl;
    ofs << "element vertex"
        << " " << point_count << std::endl;
    ofs << "property float x" << std::endl;
    ofs << "property float y" << std::endl;
    ofs << "property float z" << std::endl;
    ofs << "end_header" << std::endl;
    ofs.close();

    std::stringstream ss;
    for (int i = 0; i < width * height; i++)
    {
        if (isnan(point_cloud_data[i].xyz.x) || isnan(point_cloud_data[i].xyz.y) || isnan(point_cloud_data[i].xyz.z))
        {
            continue;
        }

        ss << (float)point_cloud_data[i].xyz.x << " " << (float)point_cloud_data[i].xyz.y << " "
           << (float)point_cloud_data[i].xyz.z << std::endl;
    }

    std::ofstream ofs_text(file_name, std::ios::out | std::ios::app);
    ofs_text.write(ss.str().c_str(), (std::streamsize)ss.str().length());
}*/

struct KinectAzure {
	k4a_device_t device;
	k4a_device_configuration_t config;
	k4a_calibration_t calibration;
	k4a_transformation_t transformation;
	k4a_image_t color_image;
	// float2, same res as depth image:
	k4a_image_t xy_table;
	// float3, same res as depth image:
	k4a_image_t point_cloud;
	uint32_t point_count;

	k4a_imu_sample_t imu_sample;
	glm::mat4 cloud2world;

	char * serial;
	uint32_t device_count;

	bool open(uint32_t index=0) {
		device_count = k4a_device_get_installed_count();

		if (index < device_count && K4A_RESULT_SUCCEEDED == k4a_device_open(index, &device)) {
			// get serial:
			// Allocate memory for the serial, then acquire it
			size_t serial_size = 0;
			k4a_device_get_serialnum(device, NULL, &serial_size);
			serial = (char*)(calloc(serial_size, 1));
			k4a_device_get_serialnum(device, serial, &serial_size);

			// create a default config:
			config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
			config.color_format     = K4A_IMAGE_FORMAT_COLOR_BGRA32;
			config.color_resolution = K4A_COLOR_RESOLUTION_1080P;
			config.depth_mode 		= K4A_DEPTH_MODE_NFOV_UNBINNED;
			config.camera_fps       = K4A_FRAMES_PER_SECOND_30;
			config.synchronized_images_only = true; // set false if not both depth and colour enabled
			config.depth_delay_off_color_usec = 0;
			config.wired_sync_mode = K4A_WIRED_SYNC_MODE_STANDALONE;
			config.subordinate_delay_off_master_usec = 0;
			config.disable_streaming_indicator = false;

			cloud2world = glm::mat4();

			update_calibration();
			
			return true;
		}
		return false;
	}

	void update_calibration() {
		k4a_device_get_calibration	(device, config.depth_mode, config.color_resolution, &calibration);
		if (transformation) k4a_transformation_destroy(transformation);
		transformation = k4a_transformation_create(&calibration);

		if (xy_table) k4a_image_release(xy_table);
		k4a_image_create(K4A_IMAGE_FORMAT_CUSTOM,
			calibration.depth_camera_calibration.resolution_width,
			calibration.depth_camera_calibration.resolution_height,
			calibration.depth_camera_calibration.resolution_width * (int)sizeof(k4a_float2_t),
			&xy_table);

		//create_xy_table(calibration, xy_table);
		k4a_float2_t *table_data = (k4a_float2_t *)(void *)k4a_image_get_buffer(xy_table);
		int width = calibration.depth_camera_calibration.resolution_width;
		int height = calibration.depth_camera_calibration.resolution_height;
		k4a_float2_t p;
		k4a_float3_t ray;
		int valid;
		for (int y = 0, idx = 0; y < height; y++) {
			p.xy.y = (float)y;
			for (int x = 0; x < width; x++, idx++) {
				p.xy.x = (float)x;
				k4a_calibration_2d_to_3d(&calibration, &p, 1.f, K4A_CALIBRATION_TYPE_DEPTH, K4A_CALIBRATION_TYPE_DEPTH, &ray, &valid);
				if (valid) {
					table_data[idx].xy.x = ray.xyz.x;
					table_data[idx].xy.y = ray.xyz.y;
				} else  {
					table_data[idx].xy.x = nanf("");
					table_data[idx].xy.y = nanf("");
				}
			}
		}
		
		if (point_cloud) k4a_image_release(point_cloud);	
		k4a_image_create(K4A_IMAGE_FORMAT_CUSTOM,
			calibration.depth_camera_calibration.resolution_width,
			calibration.depth_camera_calibration.resolution_height,
			calibration.depth_camera_calibration.resolution_width * (int)sizeof(k4a_float3_t),
			&point_cloud);

		if (color_image) k4a_image_release(color_image);	
		k4a_image_create(K4A_IMAGE_FORMAT_COLOR_BGRA32,
			calibration.depth_camera_calibration.resolution_width,
			calibration.depth_camera_calibration.resolution_height,
			calibration.depth_camera_calibration.resolution_width * 4,
			&color_image);
	}

	bool get_imu(int32_t timeout_in_ms = 0) {
		
		// try to get an IMU too:
		k4a_wait_result_t res = k4a_device_get_imu_sample(device, &imu_sample, timeout_in_ms);
		if (res == K4A_WAIT_RESULT_TIMEOUT && timeout_in_ms > 0) {
			printf("timed out waiting for capture\n");
		} else if (res == K4A_WAIT_RESULT_SUCCEEDED) {

			return true;
		}
		return false;
	}

	bool capture(int32_t timeout_in_ms = 0) {
		k4a_capture_t capture;
		k4a_wait_result_t res = k4a_device_get_capture(device, &capture, timeout_in_ms);
		if (res == K4A_WAIT_RESULT_TIMEOUT && timeout_in_ms > 0) {
			printf("timed out waiting for capture\n");
		} else if (res == K4A_WAIT_RESULT_SUCCEEDED) {
			k4a_image_t depth = k4a_capture_get_depth_image(capture);
			//k4a_image_t ir = k4a_capture_get_ir_image(capture);

			if (depth && xy_table && point_cloud) {
				int width = k4a_image_get_width_pixels(depth);
				int height = k4a_image_get_height_pixels(depth);

				uint16_t *depth_data = (uint16_t *)(void *)k4a_image_get_buffer(depth);
				glm::vec2 *xy_table_data = (glm::vec2 *)(void *)k4a_image_get_buffer(xy_table);
				glm::vec3 *point_cloud_data = (glm::vec3 *)(void *)k4a_image_get_buffer(point_cloud);

				point_count = 0;
				for (int i = 0; i < width * height; i++) {
					if (depth_data[i] != 0 && !isnan(xy_table_data[i].x) && !isnan(xy_table_data[i].y)) {

						float depth_m = (float)depth_data[i] * 0.001f;

						glm::vec3 v = glm::vec3(
							xy_table_data[i].x * depth_m,
							-xy_table_data[i].y * depth_m,
							-depth_m
						);

						// point_cloud_data[i].xyz.x = -0.001f * xy_table_data[i].xy.x * (float)depth_data[i];
						// point_cloud_data[i].xyz.y = -0.001f * xy_table_data[i].xy.y * (float)depth_data[i];
						// point_cloud_data[i].xyz.z = -0.001f * (float)depth_data[i];

						point_cloud_data[i] = transform(cloud2world, v);

						point_count++;
					} else {
						point_cloud_data[i].x = nanf("");
						point_cloud_data[i].y = nanf("");
						point_cloud_data[i].z = nanf("");
					}
				}
			} 


			k4a_image_t color = k4a_capture_get_color_image(capture);
			// next, need to align this image to the depth image.
			if (K4A_RESULT_SUCCEEDED != k4a_transformation_color_image_to_depth_camera(transformation, depth, color, color_image)) {
				printf("failed to transform colour image\n");
			}
			k4a_image_release(color);
			

			k4a_image_release(depth);
			//k4a_image_release(ir);
			k4a_capture_release(capture);
			return true;
		}
		return false;
	}

	void stop() {
		// free calibration, transformation, images etc. and set to nulltpr
	}

	void close() {
		if (device) {
			if (transformation) k4a_transformation_destroy(transformation);
			transformation = nullptr;
			
			if (color_image) k4a_image_release(color_image);
			color_image = nullptr;
			k4a_device_close(device);
			device = nullptr;
		}
	}
};

#include "node-api-helpers.h"
#define KINECTAZURESTATE_MAGIC 'KnAz'

struct KinectAzureState {
	uint32_t magic = KINECTAZURESTATE_MAGIC;

	KinectAzure azure;


	napi_env env;


	static KinectAzureState * getFromPtr(void * ptr) {
		KinectAzureState * data = (KinectAzureState *)ptr;
		return (data->magic == KINECTAZURESTATE_MAGIC) ? data : nullptr;
	}

	static KinectAzureState * getFromNapi(napi_env env, napi_value arg) {
		KinectAzureState * data = nullptr;
		napi_valuetype type;
		return (napi_ok == napi_typeof(env, arg, &type)
		&& type == napi_external
		&& napi_ok == napi_get_value_external(env, arg, (void **)&data)
		&& data) ? getFromPtr(data) : nullptr;
	}

	static void napi_finalize_k4a_device(napi_env env, void* ptr, void* hint) {
		KinectAzureState * data = getFromPtr(ptr);
		if (!data) return;

		printf("closing kinect device %s\n", data->azure.serial);
		data->azure.close();
	}

};

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

	uint32_t index = 0;
	napi_get_value_uint32(env, argv[0], &index);
	
	KinectAzureState * state = (KinectAzureState *)calloc(sizeof(KinectAzureState), 1);
	if (state->azure.open(index)) {
		state->magic = KINECTAZURESTATE_MAGIC;
		state->env = env;

		napi_value ndevice;
		assert(napi_ok == napi_create_external(env, state, KinectAzureState::napi_finalize_k4a_device, nullptr, &ndevice));
		return ndevice;
	} else {
		free(state);
		napi_throw_error(env, nullptr, "Failed to open Kinect Azure device");
		return nullptr;
	}
}

napi_value device_get_serialnum(napi_env env, const napi_callback_info info) {
	napi_value result = nullptr;
	size_t argc = 1;
	napi_value argv[1];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get device:
	KinectAzureState * state = KinectAzureState::getFromNapi(env, argv[0]);
	if (state) {
		const char *serial = state->azure.serial;
		napi_create_string_utf8(env, serial, strlen(serial), &result);
	}
	return result;
}

napi_value device_set_matrix(napi_env env, const napi_callback_info info) {
	napi_value result = nullptr;
	size_t argc = 2;
	napi_value argv[2];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get device:
	KinectAzureState * state = KinectAzureState::getFromNapi(env, argv[0]);
	if (state) {
		float * value = glm::value_ptr(state->azure.cloud2world);
		assert(napi_ok == getTypedArray(env, argv[1], value));
		memcpy(glm::value_ptr(state->azure.cloud2world), value, sizeof(glm::mat4));
	}
	return result;
}


napi_value device_start_cameras(napi_env env, const napi_callback_info info) {
	napi_value nresult = nullptr;
	size_t argc = 1;
	napi_value argv[1];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get device:
	KinectAzureState * state = KinectAzureState::getFromNapi(env, argv[0]);
	if (state) {
		// Start the camera with the given configuration
		k4a_result_t result = k4a_device_start_cameras(state->azure.device, &state->azure.config);
		assert(napi_ok == napi_get_boolean(env, result, &nresult));

		state->azure.update_calibration();
	}
	return nresult;
}

napi_value device_start_imu(napi_env env, const napi_callback_info info) {
	napi_value nresult = nullptr;
	size_t argc = 1;
	napi_value argv[1];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get device:
	KinectAzureState * state = KinectAzureState::getFromNapi(env, argv[0]);
	if (state) {
		// Start the camera with the given configuration
		k4a_result_t result = k4a_device_start_imu(state->azure.device);
		assert(napi_ok == napi_get_boolean(env, result, &nresult));
	}
	return nresult;
}


napi_value device_capture(napi_env env, const napi_callback_info info) {
	napi_value nresult = nullptr;
	size_t argc = 1;
	napi_value argv[1];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	uint32_t timeout_ms = 0;
	napi_get_value_uint32(env, argv[0], &timeout_ms);

	// get device:
	KinectAzureState * state = KinectAzureState::getFromNapi(env, argv[0]);
	if (state) {
		bool result = state->azure.capture(timeout_ms);
		assert(napi_ok == napi_get_boolean(env, result, &nresult));
	}
	return nresult;
}

napi_value device_get_color(napi_env env, const napi_callback_info info) {
	napi_value nresult = nullptr;
	size_t argc = 1;
	napi_value argv[1];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get device:
	KinectAzureState * state = KinectAzureState::getFromNapi(env, argv[0]);
	if (state) {
		// return 
		k4a_image_t& image = state->azure.color_image;
		uint8_t * data = (uint8_t *)k4a_image_get_buffer(image);
		size_t length = k4a_image_get_width_pixels(image) * k4a_image_get_height_pixels(image) * 4;
		size_t size = k4a_image_get_stride_bytes(image) * k4a_image_get_height_pixels(image);

		napi_value ab;
		assert(napi_ok == napi_create_external_arraybuffer(env, data, size, nullptr, nullptr, &ab));

		assert(napi_ok == napi_create_typedarray(env, napi_uint8_array, length, ab, 0, &nresult));
	}
	return nresult;
}

napi_value device_get_cloud(napi_env env, const napi_callback_info info) {
	napi_value nresult = nullptr;
	size_t argc = 1;
	napi_value argv[1];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get device:
	KinectAzureState * state = KinectAzureState::getFromNapi(env, argv[0]);
	if (state) {
		k4a_image_t& point_cloud = state->azure.point_cloud;
		//printf("dim %d x %d\n", k4a_image_get_width_pixels(point_cloud), k4a_image_get_height_pixels(point_cloud));
		
		float * points = (float *)k4a_image_get_buffer(point_cloud);
		size_t length = k4a_image_get_width_pixels(point_cloud) * k4a_image_get_height_pixels(point_cloud) * 3;
		size_t size = k4a_image_get_stride_bytes(point_cloud) * k4a_image_get_height_pixels(point_cloud);

		napi_value ab;
		assert(napi_ok == napi_create_external_arraybuffer(env, points, size, nullptr, nullptr, &ab));

		assert(napi_ok == napi_create_typedarray(env, napi_float32_array, length, ab, 0, &nresult));
	}
	return nresult;
}



napi_value device_get_acc(napi_env env, const napi_callback_info info) {
	napi_value nresult = nullptr;
	size_t argc = 1;
	napi_value argv[1];
	assert(napi_ok == napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

	// get device:
	KinectAzureState * state = KinectAzureState::getFromNapi(env, argv[0]);
	if (state) {
		state->azure.get_imu(0);

		napi_value ab;
		assert(napi_ok == napi_create_external_arraybuffer(env, &state->azure.imu_sample.acc_sample.xyz.x, 3 * sizeof(float), nullptr, nullptr, &ab));
		assert(napi_ok == napi_create_typedarray(env, napi_float32_array, 3, ab, 0, &nresult));
	}
	return nresult;
}


#endif