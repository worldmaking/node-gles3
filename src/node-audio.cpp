#include <node_api.h> 
#include <napi.h>
#include <assert.h>
#include <stdio.h> 
#include <stdlib.h>

#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

#define DEVICE_FORMAT       ma_format_f32
#define DEVICE_CHANNELS     2
#define DEVICE_SAMPLE_RATE  48000

#define ARRAYBUFFER_LENGTH  (4096*10)

struct AudioCallbackState {
	ma_context context;
	ma_device_config deviceConfig;
	ma_device device;

	float * ringBuffer;
	int t = 0;
};

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {

    // MA_ASSERT(pDevice->playback.channels == DEVICE_CHANNELS);
    AudioCallbackState& state = *(AudioCallbackState *)pDevice->pUserData;

	float* out = (float*)pOutput;
	for (ma_uint32 i=0; i<frameCount; i++) {
		//float s = sin(t * M_PI*2.f * 300.f/DEVICE_SAMPLE_RATE);
		float s = state.ringBuffer[state.t];
		out[i*2] = s;
		out[i*2+1] = s;
		// burn after reading
		state.ringBuffer[state.t] = 0.f;

		state.t++;
		if (state.t >= ARRAYBUFFER_LENGTH) state.t = 0;
	}
}

class Module : public Napi::Addon<Module> {
public:

	AudioCallbackState state;

	Napi::Value start(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();

		ma_device_info* pPlaybackInfos;
		ma_uint32 playbackCount;
		ma_device_info* pCaptureInfos;
		ma_uint32 captureCount;
		if (ma_context_get_devices(&state.context, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount) != MA_SUCCESS) {
			throw Napi::Error::New(env, "Audio Get Devices exception");
			return env.Null();

		}
		// output devices:
		for (ma_uint32 iDevice = 0; iDevice < playbackCount; iDevice += 1) {
			ma_device_info& info = pPlaybackInfos[iDevice];
			ma_context_get_device_info(&state.context, ma_device_type_playback, &info.id, ma_share_mode_shared, &info);
			printf("playback: %d - %s; %d-%d chans, %d-%dHz\n", iDevice, info.name, info.minChannels, info.maxChannels, info.minSampleRate, info.maxSampleRate);
		}

		// input devices:
		for (ma_uint32 iDevice = 0; iDevice < captureCount; iDevice += 1) {
			ma_device_info& info = pCaptureInfos[iDevice];
			ma_context_get_device_info(&state.context, ma_device_type_capture, &info.id, ma_share_mode_shared, &info);
			printf("capture: %d - %s; %d-%d chans, %d-%dHz\n", iDevice, info.name, info.minChannels, info.maxChannels, info.minSampleRate, info.maxSampleRate);
		}

		state.deviceConfig = ma_device_config_init(ma_device_type_playback);
		state.deviceConfig.playback.format   = DEVICE_FORMAT;
		state.deviceConfig.playback.channels = DEVICE_CHANNELS;
		state.deviceConfig.sampleRate        = DEVICE_SAMPLE_RATE;
		state.deviceConfig.dataCallback      = data_callback;
		state.deviceConfig.pUserData         		 = &state;

		if (ma_device_init(NULL, &state.deviceConfig, &state.device) != MA_SUCCESS) {
			throw Napi::Error::New(env, "Failed to open playback device.\n");
			return env.Null();
		}

		printf("Device Name: %s\n", state.device.playback.name);

		// allocate the ringbuffer
		// doing this via JS to make it easier:
		Napi::Float32Array tab = Napi::Float32Array::New(env, ARRAYBUFFER_LENGTH);
		state.ringBuffer = tab.Data();
		// store this in our object to prevent it being GC'd:
		info.This().ToObject().Set("outbuffer", tab);

		if (ma_device_start(&state.device) != MA_SUCCESS) {
			ma_device_uninit(&state.device);
			throw Napi::Error::New(env, "Failed to start playback device.\n");
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

	Module(Napi::Env env, Napi::Object exports) {

		if (ma_context_init(NULL, 0, NULL, &state.context) != MA_SUCCESS) {
			// Error.
			throw Napi::Error::New(env, "Audio Init exception");
		}
		
		// See https://github.com/nodejs/node-addon-api/blob/main/doc/class_property_descriptor.md
		DefineAddon(exports, {
			InstanceMethod("start", &Module::start),
			InstanceMethod("end", &Module::end),

			// InstanceValue
			// InstanceAccessor
			InstanceAccessor<&Module::Gett>("t"),
		});
	}
};

NODE_API_ADDON(Module)