#include <node_api.h> 
#include <napi.h>

#include <assert.h>
#include <stdio.h> 
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <vector>


#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API



 struct Pipeline : public Napi::ObjectWrap<Pipeline> {

	// Create a Pipeline - this serves as a top-level API for streaming and processing frames
	rs2::pipeline p;
	// https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1config.html
	rs2::config config;
	// Declare pointcloud object, for calculating pointclouds and texture mappings
	//https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1pointcloud.html
	rs2::pointcloud pc;
	// We want the points object to be persistent so we can display the last cloud when a frame drops
	// https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1points.html
	rs2::points points;

	// storage for the vertex xyz points
	Napi::ArrayBuffer vertices_ab;

//	Napi::TypedArrayOf<float> depth;
	Napi::TypedArrayOf<float> vertices;

// 	// .getWidth(), .getHeight(), .getResolution(), .getChannels()
// 	// .getDataType(), .getMemoryType() (CPU or GPU), .getPtr()
// 	// sl::Mat left;
// 	// sl::Mat depth;
// 	// The point cloud stores its data on 4 channels using 32-bit float for each channel. The last float is used to store color information, where R, G, B, and alpha channels (4 x 8-bit) are concatenated into a single 32-bit float. 
// 	sl::Mat cloud;
// 	// The output is a 4 channels 32-bit matrix (X,Y,Z,empty), where X,Y,Z values encode the direction of the normal vectors.
// 	sl::Mat normals;
// 	sl::Resolution capture_res;
// 	uint64_t ms = 0;

    Pipeline(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Pipeline>(info) {
		Napi::Env env = info.Env();

	//	if (info.Length()) open(info);
	}

	Napi::Value start(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();
		Napi::Object This = info.This().As<Napi::Object>();

// 		// expect some configuration here
 		const Napi::Object options = info.Length() ? info[0].ToObject() : Napi::Object::New(env);



		// Configure and start the pipeline
		p.start(config);

// 		sl::InitParameters init_parameters;
// 		init_parameters.depth_mode = sl::DEPTH_MODE::PERFORMANCE;
// 		init_parameters.Pipeline_resolution = sl::RESOLUTION::HD720;
// 		init_parameters.Pipeline_fps = 30;
// 		init_parameters.coordinate_units = sl::UNIT::METER;

// 		sl::RuntimeParameters runtime_parameters;
// 		//runtime_parameters.sensing_mode = SENSING_MODE::FILL;
		
// 		// from SVO file:
// 		//init_parameters.input.setFromSVOFile(input_path);
// 		// from IP stream:
// 		// ..
// 		if (options.Has("serial")) {
// 			// try to open device by serial
// 			uint32_t z = options.Get("serial").ToNumber().Uint32Value();
// 			init_parameters.input.setFromSerialNumber(z);
// 		} else if (options.Has("id")) {
// 			// try to open device by id
// 			uint32_t z = options.Get("id").ToNumber().Uint32Value();
// 			init_parameters.input.setFromPipelineID(z);
// 		} 

// 		sl::ERROR_CODE err = zed.open(init_parameters);
// 		if (err != sl::ERROR_CODE::SUCCESS) {
// 			 throw Napi::Error::New(env, sl::toString(err).get());
// 		}

// 		auto cam_info = zed.getPipelineInformation();
// 		//cout << cam_info.Pipeline_model << ", ID: " << z << ", SN: " << cam_info.serial_number << " Opened" << endl;

// 		This.Set("serial", cam_info.serial_number);
// 		This.Set("model", sl::toString(cam_info.Pipeline_model).get());
// 		This.Set("input_type", sl::toString(cam_info.input_type).get());

// 		// has calibration parameters, firmware, fps, resolution:
// 		//This.Set("Pipeline_configuration", sl::toString(cam_info.Pipeline_configuration).get());
// 		This.Set("firmware_version", cam_info.Pipeline_configuration.firmware_version);
// 		This.Set("fps", cam_info.Pipeline_configuration.fps);
		
// 		auto resolution = cam_info.Pipeline_configuration.resolution;
// 		This.Set("width", resolution.width);
// 		This.Set("height", resolution.height);

// 		size_t subdiv = 1;
// 		capture_res.width = resolution.width / subdiv;
// 		capture_res.height = resolution.height / subdiv;

// 		// CalibrationParameters calibration_params = zed.getPipelineInformation()->calibration_parameters;
// 		// // Focal length of the left eye in pixels
// 		// float focal_left_x = calibration_params.left_cam.fx;
// 		// // First radial distortion coefficient
// 		// float k1 = calibration_params.left_cam.disto[0];
// 		// // Translation between left and right eye on z-axis
// 		// float tz = calibration_params.T.z;
// 		// // Horizontal field of view of the left eye in degrees
// 		// float h_fov = calibration_params.left_cam.h_fov;

// 		// has firmware, imu, barometer, magnetometer etc.
// 		//This.Set("sensors_configuration", sl::toString(cam_info.sensors_configuration).get());

// 		// Pipeline settings:
// 		// Set exposure to 50% of Pipeline framerate
// 		// zed.setPipelineSettings(VIDEO_SETTINGS::EXPOSURE, 50);
// 		// // Set white balance to 4600K
// 		// zed.setPipelineSettings(VIDEO_SETTINGS::WHITE_BALANCE, 4600);
// 		// // Reset to auto exposure
// 		// zed.setPipelineSettings(VIDEO_SETTINGS::EXPOSURE, VIDEO_SETTINGS_VALUE_AUTO);

// 		// allocate some memory to store the Left/Depth images
// 		cloud.alloc(capture_res, sl::MAT_TYPE::F32_C4);
// 		This.Set("cloud", Napi::TypedArrayOf<float>::New(env, 
// 			4 * capture_res.width * capture_res.height,
// 			Napi::ArrayBuffer::New(env, cloud.getPtr<float>(), cloud.getStepBytes() * cloud.getHeight()),
// 			0,
// 			napi_float32_array)
// 		);

// 		normals.alloc(capture_res, sl::MAT_TYPE::F32_C4);
// 		This.Set("normals", Napi::TypedArrayOf<float>::New(env, 
// 			4 * capture_res.width * capture_res.height,
// 			Napi::ArrayBuffer::New(env, normals.getPtr<float>(), normals.getStepBytes() * normals.getHeight()),
// 			0,
// 			napi_float32_array)
// 		);

		return This;
	}

	~Pipeline() {
		//zed_close();
		printf("~Pipeline\n");
	}

// 	void zed_close() {
// 		// join thread
// 		// release memory

// 		zed.close();
// 	}

// 	Napi::Value close(const Napi::CallbackInfo& info) {
// 		zed_close();
// 		return info.This();
// 	}

// 	Napi::Value isOpened(const Napi::CallbackInfo& info) {
// 		Napi::Env env = info.Env();
// 		return Napi::Boolean::New(env, zed.isOpened());
// 	}

	Napi::Value grab(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();
		Napi::Object This = info.This().As<Napi::Object>();

		bool wait = info.Length() && info[0].As<Napi::Boolean>();

		rs2::frameset frames;
		if (wait) {
			// https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1frameset.html
			// Block program until frames arrive
			frames = p.wait_for_frames();
		} else {
			// non-blocking:
			if (!p.poll_for_frames(&frames)) return env.Null();
		}

		// Try to get a frame of a depth image
		// https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1depth__frame.html
		rs2::depth_frame depth = frames.get_depth_frame();

		
		// rs2::pose_frame pose_frame = frames.get_pose_frame();
		// rs2_pose pose = pose_frame.get_pose_data();
		// printf("accel %f %f %f\n", pose.acceleration.x, pose.acceleration.y, pose.acceleration.z);

		// frame depth.apply_filter (filter_interface &filter).as<rs2::depth_frame>();


		// Get the depth frame's dimensions
		int width = depth.get_width();
		int height = depth.get_height();
		// retrieve frame stride, meaning the actual line width in memory in bytes (not the logical image width)
		int stride = depth.get_stride_in_bytes ();
		int bitspp = depth.get_bits_per_pixel ();
		int bytespp = depth.get_bytes_per_pixel ();
		double t = depth.get_timestamp ();
		const int sz = depth.get_data_size ();
		const void * data = depth.get_data ();
		// Query the distance from the Pipeline to the object in the center of the image
		//float dist_to_center = depth.get_distance(width / 2, height / 2);
		// Print the distance
		// printf("The Pipeline is facing an object %f x %f meters away \n", depth.get_units(), dist_to_center);
		// printf("size %d points %d x %d = %d \n", sz, width, height, width*height);

		// {
		// 	const size_t num_vertices = width * height; //points.size();
		// 	const size_t num_floats = num_vertices;
		// 	const float units = depth.get_units();
		// 	const float * vertices = (float *)data; // (float *)depth.get_data();  // xyz

		// 	printf("depth count %d point %f\n", num_vertices, vertices[640*240 + 320] * units);

		// 	// const size_t num_bytes = num_floats * sizeof(float);
		// 	if (!this->depth || this->depth.ElementLength() != num_floats) {
		// 		// reallocate it:
		// 		printf("reallocating %d floats\n", num_floats);
		// 		this->depth = Napi::TypedArrayOf<float>::New(env, num_floats, napi_float32_array);
		// 		This.Set("depth", this->depth);
		// 	}

		// 	//memcpy(this->vertices.Data(), vertices, num_bytes);
		// }

		{
			// Generate the pointcloud and texture mappings
			points = pc.calculate(depth);
			//const rs2::vertex * vertices = points.get_vertices ();
			const float * vertices = (float *)points.get_vertices ();  // xyz
			//const rs2::texture_coordinate * texcoords = points.get_texture_coordinates (); // uv
			const float * texcoords = (float *)points.get_texture_coordinates (); // uv
			
			const size_t num_vertices = points.size();
			const size_t num_floats = num_vertices * 3;
			const size_t num_bytes = num_floats * sizeof(float);
			if (!this->vertices || this->vertices.ElementLength() != num_floats) {
				// reallocate it:
				printf("reallocating %d floats\n", num_floats);
				this->vertices = Napi::TypedArrayOf<float>::New(env, num_floats, napi_float32_array);
				This.Set("vertices", this->vertices);
			}

			memcpy(this->vertices.Data(), vertices, num_bytes);
		}


		// auto color = frames.get_color_frame();
		// // Tell pointcloud object to map to this color frame
		// pc.map_to(color);

		return This;
	}

	Napi::Value get_vertices(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();
		Napi::Object This = info.This().As<Napi::Object>();

		const size_t num_vertices = points.size();
		const size_t num_floats = num_vertices * 3;
		const size_t num_bytes = num_floats * sizeof(float);

		// if (!vertices_ab || vertices_ab.ByteLength() != num_bytes) {
		// 	// reallocate it:
		// 	vertices_ab = Napi::ArrayBuffer::New(env, num_bytes);
		// 	This.Set("vertices_arraybuffer", vertices_ab);

		vertices = Napi::TypedArrayOf<float>::New(env, num_vertices * 3, napi_float32_array);
		// }

		//Napi::TypedArrayOf<float> vertices_float32array = Napi::TypedArrayOf<float>::New(env, num_vertices * 3, vertices_ab, 0, napi_float32_array);

		return This;
	}

	Napi::Value get_active_profile(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();
		rs2::pipeline_profile profile = p.get_active_profile ();

		// https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1pipeline__profile.html
		if (1) {
			Napi::Object res = Napi::Object::New(env);

			const rs2::device dev = profile.get_device();
			res.Set("device", Napi::Object::New(env));
			//https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1device.html

			const std::vector<rs2::stream_profile> streams = profile.get_streams();
			res.Set("streams", Napi::Array::New(env, streams.size()));

			// populate `res` with details of device & streams
			for (int i=0; i<streams.size(); i++) {
				const rs2::stream_profile sp = streams[i];
				// https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1stream__profile.html

			}

			return res;
		}
	
		return info.This();
	}

};


class Module : public Napi::Addon<Module> {
public:

	// /*
	// 	Returns array
	// */
	// Napi::Value devices(const Napi::CallbackInfo& info) {
	// 	Napi::Env env = info.Env();
	// 	//Napi::Object devices = Napi::Array::New(env);

	// 	// std::vector<sl::DeviceProperties> devList = sl::Pipeline::getDeviceList();
	// 	// for (int i = 0; i < devList.size(); i++) {
	// 	// 	Napi::Object device = Napi::Object::New(env);
	// 	// 	device.Set("id", devList[i].id);
	// 	// 	device.Set("serial", devList[i].serial_number);
	// 	// 	device.Set("model", sl::toString(devList[i].Pipeline_model).get());
	// 	// 	device.Set("state", sl::toString(devList[i].Pipeline_state).get());
	// 	// 	devices[i] = device;
	// 	// }
	// 	// return devices;
	// }

	// Napi::Value open(const Napi::CallbackInfo& info) {
	// 	Napi::Env env = info.Env();
	// }
	
	Module(Napi::Env env, Napi::Object exports) {
		// See https://github.com/nodejs/node-addon-api/blob/main/doc/class_property_descriptor.md
		DefineAddon(exports, {
			// InstanceMethod("start", &Module::start),
			// InstanceMethod("end", &Module::end),
			// //InstanceMethod("test", &Module::test),

			// // InstanceValue
			// // InstanceAccessor
//			InstanceAccessor<&Module::devices>("devices"),
			// InstanceAccessor<&Module::Gett>("t"),
			// InstanceAccessor<&Module::GetSamplerate>("samplerate"),
		});
		
		// This method is used to hook the accessor and method callbacks
		Napi::Function ctor = Pipeline::DefineClass(env, "Pipeline", {
			Pipeline::InstanceMethod<&Pipeline::start>("start"),
		// 	Pipeline::InstanceMethod<&Pipeline::close>("close"),
		// 	Pipeline::InstanceMethod<&Pipeline::isOpened>("isOpened"),
			Pipeline::InstanceMethod<&Pipeline::grab>("grab"),
			Pipeline::InstanceMethod<&Pipeline::grab>("get_active_profile"),
		});

		// Create a persistent reference to the class constructor. This will allow
		// a function called on a class prototype and a function
		// called on instance of a class to be distinguished from each other.
		Napi::FunctionReference* constructor = new Napi::FunctionReference();
		*constructor = Napi::Persistent(ctor);
		exports.Set("Pipeline", ctor);
		// Store the constructor as the add-on instance data. This will allow this
		// add-on to support multiple instances of itself running on multiple worker
		// threads, as well as multiple instances of itself running in different
		// contexts on the same thread.
		// By default, the value set on the environment here will be destroyed when
		// the add-on is unloaded using the `delete` operator, but it is also
		// possible to supply a custom deleter.
		env.SetInstanceData<Napi::FunctionReference>(constructor);
	}
};

NODE_API_ADDON(Module)