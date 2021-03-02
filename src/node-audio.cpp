#include <node_api.h> 
#include <napi.h>
#include <assert.h>
#include <stdio.h> 
#include <stdlib.h>
#include <chrono>
#include <thread>

#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

struct AudioCallbackState {
	ma_context context;
	ma_device_config deviceConfig;
	ma_device device;

	float * playbackRingBuffer;
	float * captureRingBuffer;
	uint32_t frames;
	int t = 0;
};

// Note that frameCount can be somewhat unpredictable in size from callback to callback
// it can even be an odd number, or even just 1
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {

    // MA_ASSERT(pDevice->playback.channels == DEVICE_CHANNELS);
    AudioCallbackState& state = *(AudioCallbackState *)pDevice->pUserData;

	ma_uint32 inch = state.device.capture.channels;
	ma_uint32 outch = state.device.playback.channels;
	float* outs = (float*)pOutput;
	float* ins = (float*)pInput;
	// this has to be frame-by-frame because `frameCount` can vary from callback to callback; it could even be a single sample.
	for (ma_uint32 i=0; i<frameCount; i++) {
		// copy input:
		memcpy(&state.captureRingBuffer[state.t*inch], &ins[i*inch], sizeof(float)*inch);
		// copy output:
		memcpy(&outs[i*outch], &state.playbackRingBuffer[state.t*outch], sizeof(float)*outch);
		// burn after reading
		memset(&state.playbackRingBuffer[state.t*outch], 0, sizeof(float)*outch);
		// advance
		state.t = (state.t + 1) % state.frames;
	}
}

class Module : public Napi::Addon<Module> {
public:

	AudioCallbackState state;

	/*
		Returns: {
			playback: [],
			capture: []
		}
	*/
	Napi::Value devices(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();

		Napi::Object devices = Napi::Object::New(env);
		Napi::Array playbackDevices = Napi::Array::New(env);
		Napi::Array captureDevices = Napi::Array::New(env);
		devices.Set("playback", playbackDevices);
		devices.Set("capture", captureDevices);

		ma_device_info* pPlaybackInfos;
		ma_uint32 playbackCount;
		ma_device_info* pCaptureInfos;
		ma_uint32 captureCount;
		if (ma_context_get_devices(&state.context, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount) != MA_SUCCESS) {
			throw Napi::Error::New(env, "Audio Get Devices exception");
			return env.Null();

		}
		// output devices:
		for (ma_uint32 i = 0; i < playbackCount; i++) {
			ma_device_info& info = pPlaybackInfos[i];
			ma_context_get_device_info(&state.context, ma_device_type_playback, &info.id, ma_share_mode_shared, &info);
			
			Napi::Object device = Napi::Object::New(env);
			device.Set("id", i);
			device.Set("name", info.name);
			device.Set("minChannels", info.minChannels);
			device.Set("maxChannels", info.maxChannels);
			device.Set("minSampleRate", info.minSampleRate);
			device.Set("maxSampleRate", info.maxSampleRate);
			playbackDevices[i] = device;
		}

		// input devices:
		for (ma_uint32 i = 0; i < captureCount; i++) {
			ma_device_info& info = pCaptureInfos[i];
			ma_context_get_device_info(&state.context, ma_device_type_capture, &info.id, ma_share_mode_shared, &info);
			
			Napi::Object device = Napi::Object::New(env);
			device.Set("id", i);
			device.Set("name", info.name);
			device.Set("minChannels", info.minChannels);
			device.Set("maxChannels", info.maxChannels);
			device.Set("minSampleRate", info.minSampleRate);
			device.Set("maxSampleRate", info.maxSampleRate);
			captureDevices[i] = device;
		}

		return devices;
	}

	/*
		audio.start()  ->  starts playback with default device, stereo, 44kHz

		audio.start({
			samplerate: 48000,
			indevcice: 0,
			inchannels: 2, // can be 0
			outdevice: 0,
			outchannels: 2, // can be 0
		})
		
	*/
	Napi::Value start(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();
		const Napi::Object options = info.Length() ? info[0].ToObject() : Napi::Object::New(env);

		ma_uint32 sampleRate = options.Has("samplerate") ? options.Get("samplerate").ToNumber().Uint32Value() : 48000;
		ma_uint32 outChannels = options.Has("outchannels") ? options.Get("outchannels").ToNumber().Uint32Value() : 2;
		ma_uint32 inChannels = options.Has("inchannels") ? options.Get("inchannels").ToNumber().Uint32Value() : 2;

		//ma_uint32 frames = options.Has("frames") ? options.Get("frames").ToNumber().Uint32Value() : 512;

		if (outChannels == 0 && inChannels == 0) {
			throw Napi::Error::New(env, "Cannot start audio with no input or output channels.");
			return env.Null();
		}

		ma_device_id * captureId = nullptr;
		ma_device_id * playbackId = nullptr;

		ma_device_info* pPlaybackInfos;
		ma_device_info* pCaptureInfos;
		ma_uint32 playbackCount;
		ma_uint32 captureCount;			
		if (options.Has("indevice") || options.Has("outdevice")) {
			if (ma_context_get_devices(&state.context, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount) != MA_SUCCESS) {
				throw Napi::Error::New(env, "Audio Get Devices exception");
				return env.Null();
			}

			if (options.Has("indevice")) {
				int i = options.Get("indevice").ToNumber().Uint32Value();
				if (i < captureCount) {
					captureId = &pCaptureInfos[i].id;
					//ma_device_info& info = pCaptureInfos[i];
					//ma_context_get_device_info(&state.context, ma_device_type_capture, &info.id, ma_share_mode_shared, &info);
				}
			}

			if (options.Has("outdevice")) {
				int i = options.Get("outdevice").ToNumber().Uint32Value();
				if (i < captureCount) {
					playbackId = &pPlaybackInfos[i].id;
					//ma_device_info& info = pCaptureInfos[i];
					//ma_context_get_device_info(&state.context, ma_device_type_capture, &info.id, ma_share_mode_shared, &info);
				}
			}
		}

		state.deviceConfig = ma_device_config_init(
			inChannels > 0 && outChannels > 0 ? ma_device_type_duplex : 
			inChannels > 0 ? ma_device_type_capture :
			ma_device_type_playback);
		state.deviceConfig.sampleRate        = sampleRate;
		
		//state.deviceConfig.periodSizeInFrames = frames;
		//state.deviceConfig.noClip
		//state.deviceConfig.noPreZeroedOutputBuffer

		state.deviceConfig.playback.pDeviceID = playbackId; // pointer to ma_device_id
		state.deviceConfig.playback.format   = ma_format_f32;
		state.deviceConfig.playback.channels = outChannels;

		state.deviceConfig.capture.pDeviceID = captureId;
		state.deviceConfig.capture.format    = ma_format_f32;
		state.deviceConfig.capture.channels  = inChannels;

		state.deviceConfig.pUserData         = &state;
		state.deviceConfig.dataCallback      = data_callback;
		//state.deviceConfig.stopCallback

		if (ma_device_init(NULL, &state.deviceConfig, &state.device) != MA_SUCCESS) {
			throw Napi::Error::New(env, "Failed to open audio device.");
			return env.Null();
		}
		
		// printf("Out SR: %d\n", state.device.sampleRate);
		// //printf("Out Device Name: %s\n", state.device.playback.pDeviceID->);
		// //printf("Out channels: %d\n", state.device.latency);
		// // printf("Out channels: %s\n", state.device.periodSizeInFrames);
		// // printf("Out channels: %s\n", state.device.periodSizeInMilliseconds);
		// // printf("Out channels: %s\n", state.device.periodSizeInMilliseconds);
		printf("Out period: %d x %d\n", state.device.playback.internalPeriodSizeInFrames, state.device.playback.internalPeriods);
		printf("In period: %d x %d\n", state.device.capture.internalPeriodSizeInFrames, state.device.capture.internalPeriods);
		
		uint32_t inlen = state.device.capture.internalPeriodSizeInFrames * state.device.capture.internalPeriods;
		uint32_t outlen = state.device.playback.internalPeriodSizeInFrames * state.device.playback.internalPeriods;
		uint32_t maxlen = inlen < outlen ? outlen : inlen;
		state.frames = maxlen;

		// allocate the ringbuffer
		// doing this via JS to make it easier:
		Napi::Float32Array tab1 = Napi::Float32Array::New(env, state.frames * state.device.playback.channels);
		state.playbackRingBuffer = tab1.Data();
		Napi::Float32Array tab2 = Napi::Float32Array::New(env, state.frames * state.device.capture.channels);
		state.captureRingBuffer = tab2.Data();
		// store this in our object to prevent it being GC'd:
		info.This().ToObject()["outbuffer"] = tab1;
		info.This().ToObject()["inbuffer"] = tab2;

		info.This().ToObject().Set("outchannels", state.device.playback.channels);
		info.This().ToObject().Set("inchannels", state.device.capture.channels);
		info.This().ToObject().Set("outname", state.device.playback.name);
		info.This().ToObject().Set("inname", state.device.capture.name);
		info.This().ToObject().Set("frames", state.frames);
		info.This().ToObject().Set("latency", state.frames / (float)state.device.sampleRate);
		// recommended poll interval:
		info.This().ToObject().Set("pollms", 500.f * state.frames / (float)state.device.sampleRate);

		if (ma_device_start(&state.device) != MA_SUCCESS) {
			ma_device_uninit(&state.device);
			throw Napi::Error::New(env, "Failed to start playback device.");
			return env.Null();
		}

		return info.This();
	}

	Napi::Value end(const Napi::CallbackInfo& info) {
		ma_device_uninit(&state.device);
		return info.This();
	}

	Napi::Value Gett(const Napi::CallbackInfo &info) {
		return Napi::Number::New(info.Env(), state.t);
	}

	Napi::Value GetSamplerate(const Napi::CallbackInfo &info) {
		return Napi::Number::New(info.Env(), state.device.sampleRate);
	}


	// /*
	// 	Want to move most of the heavy lifting to a secondary thread (worker)
	// 	This thread could itself be the audio callback thread


	// */

	// struct ThreadState {

	// 	// Native thread
  	// 	std::thread nativeThread;

	// 	// callback
	// 	Napi::ThreadSafeFunction tsfn;
	// };

	// // The thread-safe function finalizer callback. This callback executes
	// // at destruction of thread-safe function, taking as arguments the finalizer
	// // data and threadsafe-function context.
	// static void FinalizerCallback(Napi::Env env, void* finalizeData, ThreadState* context) {
	// 	printf("FinalizerCallback, %p\n", context);
	// }

	// static void threadEntry(ThreadState* context) {
	// 	for (int i=0; i<10; i++) {
	// 		printf("thread loop %d\n", i);
	// 		// call back into JS:
	// 		napi_status status = context->tsfn.BlockingCall(&context->ints[index], callback);

	// 		// Sleep for some time.
    // 		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	// 	}

	// 	// allow finalization:
	// 	context->tsfn.Release();
	// }

	// Napi::Value test(const Napi::CallbackInfo &info) {
	// 	Napi::Env env = info.Env();

	// 	// create C++ object to track state in the other thread:
	// 	auto testData = new ThreadState;

	// 	// Create a new ThreadSafeFunction.
	// 	testData->tsfn = Napi::ThreadSafeFunction::New(
	// 		env,                           // Environment
	// 		info[0].As<Napi::Function>(),  // JS function from caller
	// 		"TSFN",                        // Resource name
	// 		0,                             // Max queue size (0 = unlimited).
	// 		1,                             // Initial thread count
	// 		testData,                      // Context,
	// 		FinalizerCallback,             // Finalizer
	// 		(void*)nullptr                 // Finalizer data
	// 	);

	// 	testData->nativeThread = std::thread(threadEntry, testData);

	// 	return env.Null();
	// }

	Module(Napi::Env env, Napi::Object exports) {

		if (ma_context_init(NULL, 0, NULL, &state.context) != MA_SUCCESS) {
			// Error.
			throw Napi::Error::New(env, "Audio Init exception");
		}
		
		// See https://github.com/nodejs/node-addon-api/blob/main/doc/class_property_descriptor.md
		DefineAddon(exports, {
			InstanceMethod("start", &Module::start),
			InstanceMethod("end", &Module::end),


			//InstanceMethod("test", &Module::test),

			// InstanceValue
			// InstanceAccessor
			InstanceAccessor<&Module::devices>("devices"),
			InstanceAccessor<&Module::Gett>("t"),
			InstanceAccessor<&Module::GetSamplerate>("samplerate"),
		});
	}
};

NODE_API_ADDON(Module)