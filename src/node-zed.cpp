#include <node_api.h> 
#include <napi.h>

#include <assert.h>
#include <stdio.h> 
#include <stdlib.h>
#include <chrono>
#include <thread>

#include <sl/Camera.hpp>

/* 
	Built against ZED SDK 3.5.0
*/

class Module : public Napi::Addon<Module> {
public:
	
	Module(Napi::Env env, Napi::Object exports) {

		using namespace sl;

		Camera zed;

		ERROR_CODE returned_state = zed.open();
		if (returned_state != ERROR_CODE::SUCCESS) {
			throw Napi::Error::New(env, "Error opening camera");
		}
		// Get camera information (ZED serial number)
		auto camera_infos = zed.getCameraInformation();
		printf("Hello! This is my serial number: %d\n", camera_infos.serial_number);
		zed.close();
		
		// if (ma_context_init(NULL, 0, NULL, &state.context) != MA_SUCCESS) {
		// 	// Error.
		// 	throw Napi::Error::New(env, "Audio Init exception");
		// }
		
		// See https://github.com/nodejs/node-addon-api/blob/main/doc/class_property_descriptor.md
		DefineAddon(exports, {
			// InstanceMethod("start", &Module::start),
			// InstanceMethod("end", &Module::end),


			// //InstanceMethod("test", &Module::test),

			// // InstanceValue
			// // InstanceAccessor
			// InstanceAccessor<&Module::devices>("devices"),
			// InstanceAccessor<&Module::Gett>("t"),
			// InstanceAccessor<&Module::GetSamplerate>("samplerate"),
		});
	}
};

NODE_API_ADDON(Module)