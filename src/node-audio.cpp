#include <node_api.h> 
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

ma_context context;
ma_device_config deviceConfig;
ma_device device;

float ringBuffer[ARRAYBUFFER_LENGTH];
int t = 0;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {

	float* out = (float*)pOutput;
	
	for (ma_uint32 i=0; i<frameCount; i++) {
		//float s = sin(t * M_PI*2.f * 300.f/DEVICE_SAMPLE_RATE);
		float s = ringBuffer[t];
		out[i*2] = s;
		out[i*2+1] = s;
		// burn after reading
		ringBuffer[t] = 0.f;

		t++;
		if (t >= ARRAYBUFFER_LENGTH) t = 0;
	}


    // ma_waveform* pSineWave;

    // MA_ASSERT(pDevice->playback.channels == DEVICE_CHANNELS);

    // pSineWave = (ma_waveform*)pDevice->pUserData;
    // MA_ASSERT(pSineWave != NULL);

    // ma_waveform_read_pcm_frames(pSineWave, pOutput, frameCount);


	// printf("frame rate: %d\n", frameCount);

    // (void)pInput;   /* Unused. */
}

napi_value start(napi_env env, const napi_callback_info info) {
	napi_value result = nullptr;

	ma_device_info* pPlaybackInfos;
    ma_uint32 playbackCount;
    ma_device_info* pCaptureInfos;
    ma_uint32 captureCount;
    if (ma_context_get_devices(&context, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount) != MA_SUCCESS) {
        return nullptr;

    }
	// output devices:
	for (ma_uint32 iDevice = 0; iDevice < playbackCount; iDevice += 1) {
		ma_device_info& info = pPlaybackInfos[iDevice];
        ma_context_get_device_info(&context, ma_device_type_playback, &info.id, ma_share_mode_shared, &info);
		printf("playback: %d - %s; %d-%d chans, %d-%dHz\n", iDevice, info.name, info.minChannels, info.maxChannels, info.minSampleRate, info.maxSampleRate);
    }

	// input devices:
	for (ma_uint32 iDevice = 0; iDevice < captureCount; iDevice += 1) {
        ma_device_info& info = pCaptureInfos[iDevice];
        ma_context_get_device_info(&context, ma_device_type_capture, &info.id, ma_share_mode_shared, &info);
		printf("capture: %d - %s; %d-%d chans, %d-%dHz\n", iDevice, info.name, info.minChannels, info.maxChannels, info.minSampleRate, info.maxSampleRate);
    }


	// sineWaveConfig = ma_waveform_config_init(DEVICE_FORMAT, DEVICE_CHANNELS, DEVICE_SAMPLE_RATE, ma_waveform_type_sine, 0.2, 220);
    // ma_waveform_init(&sineWaveConfig, &sineWave);

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = DEVICE_FORMAT;
    deviceConfig.playback.channels = DEVICE_CHANNELS;
    deviceConfig.sampleRate        = DEVICE_SAMPLE_RATE;
    deviceConfig.dataCallback      = data_callback;
    //deviceConfig.pUserData         = &sineWave;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        return result;
    }

    printf("Device Name: %s\n", device.playback.name);

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        return result;
    }

	// return our audio OLA buffer:
	napi_value arraybuffer;
	if (napi_ok == napi_create_external_arraybuffer(env, ringBuffer, sizeof(ringBuffer), nullptr, nullptr, &arraybuffer)) {
		if (napi_ok == napi_create_typedarray(env, napi_float32_array, ARRAYBUFFER_LENGTH, arraybuffer, 0, &result)) {
			return result;
		}
	}

    return result;
}

napi_value index(napi_env env, const napi_callback_info info) {
	napi_value result;
	napi_create_int32(env, t, &result);
	return result;
}

napi_value end(napi_env env, const napi_callback_info info) {
    ma_device_uninit(&device);
    return nullptr;
}

napi_value Init(napi_env env, napi_value exports) {
	if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
        // Error.
		return nullptr;
    }

    napi_property_descriptor export_properties[] = {
	{
		"start", nullptr, start,
		nullptr, nullptr, nullptr, napi_default, nullptr
	},
	{
		"index", nullptr, index,
		nullptr, nullptr, nullptr, napi_default, nullptr
	},
	{
		"end", nullptr, end,
		nullptr, nullptr, nullptr, napi_default, nullptr
	},
    };
    assert(napi_define_properties(env, exports, 
	sizeof(export_properties) / sizeof(export_properties[0]), export_properties) == napi_ok);
    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)