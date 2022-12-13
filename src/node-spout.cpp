#define NAPI_DISABLE_CPP_EXCEPTIONS 1

#include <node_api.h> 
#include <napi.h>

#include <assert.h>
#include <stdio.h> 
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <vector>

#include "SpoutGL\SpoutSender.h"

struct Sender : public Napi::ObjectWrap<Sender> {

    SpoutSender sender;    // Spout sender object

    Sender(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Sender>(info) {
		Napi::Env env = info.Env();
		Napi::Object This = info.This().As<Napi::Object>();



		// accel = Napi::TypedArrayOf<float>::New(env, 3, napi_float32_array);
		// This.Set("accel", accel);
		// accel[0] = 0;
		// accel[1] = 0;
		// accel[2] = -10;

	//	if (info.Length()) open(info);
	}

    Napi::Value setName(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();
		Napi::Object This = info.This().As<Napi::Object>();
        if (info.Length() > 0 && info[0].IsString()) {

            sender.SetSenderName(info[0].ToString().Utf8Value().c_str());
        }
        return This;
    }

    Napi::Value sendFBO(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();
		Napi::Object This = info.This().As<Napi::Object>();
        if (info.Length() > 2 
            && info[0].IsNumber()
            && info[1].IsNumber()
            && info[2].IsNumber()) {
            //   The fbo must be bound for read.
            //   The invert option is false because the fbo is already flipped in y.
            //   The fbo size can be different to the application window for a fixed sender size.
            //   Disable the code in WindowResized() if you want this.
            //GLuint fboID, unsigned int width, height, bool invert;
            sender.SendFbo(info[0].ToNumber().Uint32Value(), info[1].ToNumber().Uint32Value(), info[2].ToNumber().Uint32Value(), info[3].ToBoolean().Value());
        }
        return This;
    }
};

// #include <sl/Camera.hpp>

// /* 
// 	Built against ZED SDK 3.5.0
// 	+ CUDA 11.0
// */


// 		// sl::Camera zed;

// 		// ERROR_CODE returned_state = zed.open();
// 		// if (returned_state != ERROR_CODE::SUCCESS) {
// 		// 	throw Napi::Error::New(env, "Error opening camera");
// 		// }
// 		// // Get camera information (ZED serial number)
// 		// auto camera_infos = zed.getCameraInformation();
// 		// printf("Hello! This is my serial number: %d\n", camera_infos.serial_number);
// 		// zed.close();

// 		// if (ma_context_init(NULL, 0, NULL, &state.context) != MA_SUCCESS) {
// 		// 	// Error.
// 		// 	throw Napi::Error::New(env, "Audio Init exception");
// 		// }

// struct Camera : public Napi::ObjectWrap<Camera> {

// 	sl::Camera zed;
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

//     Camera(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Camera>(info) {
// 		Napi::Env env = info.Env();

// 		if (info.Length()) open(info);
// 	}

// 	Napi::Value open(const Napi::CallbackInfo& info) {
// 		Napi::Env env = info.Env();

// 		// expect some configuration here
// 		const Napi::Object options = info.Length() ? info[0].ToObject() : Napi::Object::New(env);

// 		sl::InitParameters init_parameters;
// 		init_parameters.depth_mode = sl::DEPTH_MODE::PERFORMANCE;
// 		init_parameters.camera_resolution = sl::RESOLUTION::HD720;
// 		init_parameters.camera_fps = 30;
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
// 			init_parameters.input.setFromCameraID(z);
// 		} 

// 		sl::ERROR_CODE err = zed.open(init_parameters);
// 		if (err != sl::ERROR_CODE::SUCCESS) {
// 			 throw Napi::Error::New(env, sl::toString(err).get());
// 		}

// 		auto cam_info = zed.getCameraInformation();
// 		//cout << cam_info.camera_model << ", ID: " << z << ", SN: " << cam_info.serial_number << " Opened" << endl;

// 		Napi::Object This = info.This().As<Napi::Object>();
// 		This.Set("serial", cam_info.serial_number);
// 		This.Set("model", sl::toString(cam_info.camera_model).get());
// 		This.Set("input_type", sl::toString(cam_info.input_type).get());

// 		// has calibration parameters, firmware, fps, resolution:
// 		//This.Set("camera_configuration", sl::toString(cam_info.camera_configuration).get());
// 		This.Set("firmware_version", cam_info.camera_configuration.firmware_version);
// 		This.Set("fps", cam_info.camera_configuration.fps);
		
// 		auto resolution = cam_info.camera_configuration.resolution;
// 		This.Set("width", resolution.width);
// 		This.Set("height", resolution.height);

// 		size_t subdiv = 1;
// 		capture_res.width = resolution.width / subdiv;
// 		capture_res.height = resolution.height / subdiv;

// 		// CalibrationParameters calibration_params = zed.getCameraInformation()->calibration_parameters;
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

// 		// camera settings:
// 		// Set exposure to 50% of camera framerate
// 		// zed.setCameraSettings(VIDEO_SETTINGS::EXPOSURE, 50);
// 		// // Set white balance to 4600K
// 		// zed.setCameraSettings(VIDEO_SETTINGS::WHITE_BALANCE, 4600);
// 		// // Reset to auto exposure
// 		// zed.setCameraSettings(VIDEO_SETTINGS::EXPOSURE, VIDEO_SETTINGS_VALUE_AUTO);

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

// 		// create a thread for each capture device

// 		return This;
// 	}

// 	~Camera() {
// 		zed_close();
// 		std::cout << "~MyObject"  << std::endl;
// 	}

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

// 	// convert to a thread fun
// 	Napi::Value grab(const Napi::CallbackInfo& info) {
// 		Napi::Env env = info.Env();
// 		if (!zed.isOpened()) return env.Null();

// 		auto err = zed.grab();
// 		if (err == sl::ERROR_CODE::END_OF_SVOFILE_REACHED) zed.setSVOPosition(0);
// 		if (err != sl::ERROR_CODE::SUCCESS) return info.Env().Null();

// 		// could go straight to GPU here
// 		// optional resolution argument
// 		//zed.retrieveImage(left, sl::VIEW::LEFT); //sl::VIEW::DEPTH);
// 		//zed.retrieveMeasure(depth, sl::MEASURE::DEPTH); 
// 		zed.retrieveMeasure(cloud, sl::MEASURE::XYZRGBA, sl::MEM::CPU, capture_res);
// 		zed.retrieveMeasure(normals, sl::MEASURE::NORMALS, sl::MEM::CPU, capture_res);
// 		ms = zed.getTimestamp(sl::TIME_REFERENCE::IMAGE).getMilliseconds();

// 		// sl::Mat confidence_map;
// 		// zed.retrieveMeasure(confidence_map,MEASURE_CONFIDENCE);

// 		// printf("left %d x %d of %s\n", left.getWidth(), left.getHeight(), sl::toString(left.getDataType()).get());
// 		// printf("depth %d x %d of %s\n", depth.getWidth(), depth.getHeight(), sl::toString(depth.getDataType()).get());
// 		// printf("cloud %d x %d of %s step %d\n", cloud.getWidth(), cloud.getHeight(), sl::toString(cloud.getDataType()).get(), cloud.getStepBytes());
// 		// printf("normals %d x %d of %s step %d\n", normals.getWidth(), normals.getHeight(), sl::toString(normals.getDataType()).get(), normals.getStepBytes());

// 		return info.This();
// 	}

// };


class Module : public Napi::Addon<Module> {
public:

	// /*
	// 	Returns array
	// */
	// Napi::Value devices(const Napi::CallbackInfo& info) {
	// 	Napi::Env env = info.Env();
	// 	Napi::Object devices = Napi::Array::New(env);

	// 	std::vector<sl::DeviceProperties> devList = sl::Camera::getDeviceList();
	// 	for (int i = 0; i < devList.size(); i++) {
	// 		Napi::Object device = Napi::Object::New(env);
	// 		device.Set("id", devList[i].id);
	// 		device.Set("serial", devList[i].serial_number);
	// 		device.Set("model", sl::toString(devList[i].camera_model).get());
	// 		device.Set("state", sl::toString(devList[i].camera_state).get());
	// 		devices[i] = device;
	// 	}
	// 	return devices;
	// }

	// Napi::Value open(const Napi::CallbackInfo& info) {
	// 	Napi::Env env = info.Env();
	// }
	
	Module(Napi::Env env, Napi::Object exports) {
		// See https://github.com/nodejs/node-addon-api/blob/main/doc/class_property_descriptor.md
		DefineAddon(exports, {
		// 	// InstanceMethod("start", &Module::start),
		// 	// InstanceMethod("end", &Module::end),
		// 	// //InstanceMethod("test", &Module::test),

		// 	// // InstanceValue
		// 	// // InstanceAccessor
		// 	InstanceAccessor<&Module::devices>("devices"),
		// 	// InstanceAccessor<&Module::Gett>("t"),
		// 	// InstanceAccessor<&Module::GetSamplerate>("samplerate"),
		});
		
		// This method is used to hook the accessor and method callbacks
		Napi::Function sender_ctor = Sender::DefineClass(env, "Sender", {
			
			Sender::InstanceMethod<&Sender::setName>("setName"),
			Sender::InstanceMethod<&Sender::sendFBO>("sendFBO"),
			// Camera::InstanceMethod<&Camera::close>("close"),
			// Camera::InstanceMethod<&Camera::isOpened>("isOpened"),
			// Camera::InstanceMethod<&Camera::grab>("grab"),
		});

		// Create a persistent reference to the class constructor. This will allow
		// a function called on a class prototype and a function
		// called on instance of a class to be distinguished from each other.
		Napi::FunctionReference* constructor = new Napi::FunctionReference();
		*constructor = Napi::Persistent(sender_ctor);
		exports.Set("Sender", sender_ctor);
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