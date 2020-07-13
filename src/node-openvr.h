#include "node-api-helpers.h"
#include <GL/glew.h>
#include "vr.h"

/*
Would be nice to mirror webxr interface where possible

*/

#define XR_SESSIONSTATE_MAGIC 'XRsh'

struct XRSession {
	uint32_t magic = XR_SESSIONSTATE_MAGIC;
	napi_env env;

	napi_ref inputSourcesRef;

	Hmd hmd;

	static XRSession * create(napi_env env) {
		XRSession * self = new XRSession;
		self->magic = XR_SESSIONSTATE_MAGIC;
		self->env = env;

		napi_value inputSources;
		assert(napi_ok == napi_create_array(env, &inputSources));
		napi_create_reference(env, inputSources, 1, &self->inputSourcesRef); 

		return self;
	}

	static void destroy(XRSession ** self) {
		delete *self;
		*self = nullptr;
	}
};

XRSession * session = nullptr;

// attrLoc, components, type, normalize, stride, offset
napi_value connect(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 0);

	bool enable = getBool(env, args[0]);

	if (!session) session = XRSession::create(env);

	bool ok = true;
	if (enable) {
		ok = session->hmd.connect();
	} else {
		session->hmd.disconnect();
		session = nullptr;
	}
	napi_value result_value;
	status = napi_create_uint32(env, ok ? 1 : 0, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}


napi_value getTextureWidth(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 0);

	uint32_t v = 0;
	if (session->hmd.connected) {
		v = session->hmd.width;
	}
	napi_value result_value;
	status = napi_create_uint32(env, v, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value getTextureHeight(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 0);

	uint32_t v = 0;
	if (session->hmd.connected) {
		v = session->hmd.height;
	}
	napi_value result_value;
	status = napi_create_uint32(env, v, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value getProjection(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	int eye = getInt32(env, args[0]);

	bool isArray, isTypedArray;
	status = napi_is_array(env, args[1], &isArray);
	status = napi_is_typedarray(env, args[1], &isTypedArray);

	if (isArray || isTypedArray) {
		glm::mat4& mat = session->hmd.mProjMatEye[eye];
		napi_value value;

		for (int i=0; i<16; i++) {
			status = napi_create_double(env, mat[i / 4][i % 4], &value);
			status = napi_set_element(env, args[1], i, value);
		}
	} else {
		napi_throw_type_error(env, nullptr, "needs an array to fill");
	}

	return args[1];
}

// actually for viewmatrix need the inverse of what is given:
napi_value getView(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	int eye = getInt32(env, args[0]);

	bool isArray, isTypedArray;
	status = napi_is_array(env, args[1], &isArray);
	status = napi_is_typedarray(env, args[1], &isTypedArray);

	if (isArray || isTypedArray) {
		glm::mat4& mat = session->hmd.mModelViewEye[eye];
		mat = glm::inverse(mat);

		napi_value value;

		for (int i=0; i<16; i++) {
			status = napi_create_double(env, mat[i / 4][i % 4], &value);
			status = napi_set_element(env, args[1], i, value);
		}
	} else {
		napi_throw_type_error(env, nullptr, "needs an array to fill");
	}

	return args[1];
}

napi_value inputSources(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value nresult = nullptr;

	if (session && session->hmd.connected) {
		napi_get_reference_value(env, session->inputSourcesRef, &nresult);
	}

	return nresult;
}

napi_value update(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	//napi_value args[1];
	//size_t argc = checkArgCount(env, info, args, 1, 0);

		/*
		https://developer.mozilla.org/en-US/docs/Web/API/XRSession/inputSources
		An XRInputSourceArray object listing all of the currently-connected input controllers which are linked specifically to the XR device currently in use. The returned object is live; as devices are connected to and removed from the user's system, the list's contents update to reflect the changes.

		https://developer.mozilla.org/en-US/docs/Web/API/XRInputSource

		.gamepad for buttons & axes
		.gripSpace XRSpace
		.targetRaySpace XRSpace
		.handedness "left" or "right" or "none"
		.targetRayMode "gaze" or "tracked-pointer" or "screen"
		.profiles array of strings for preferred visualization

		*/
	if (session && session->hmd.connected) {
		session->hmd.update();

		// populate the session inputSources:
		napi_value inputSources;
		napi_get_reference_value(env, session->inputSourcesRef, &inputSources);

		// check each device: 
		int idx=0;
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			vr::ETrackedDeviceClass deviceClass = session->hmd.mHMD->GetTrackedDeviceClass(i);
			const vr::TrackedDevicePose_t& trackedDevicePose = session->hmd.pRenderPoseArray[i];
			if (trackedDevicePose.bDeviceIsConnected) {

				napi_value targetRayMode = nullptr;
				napi_value handedness = nullptr;
						
				switch (deviceClass) {
					case vr::TrackedDeviceClass_HMD: {
						assert(napi_ok == napi_create_string_utf8(env, "gaze", NAPI_AUTO_LENGTH, &targetRayMode));
						assert(napi_ok == napi_create_string_utf8(env, "none", NAPI_AUTO_LENGTH, &handedness));
					} break;
						
					case vr::TrackedDeviceClass_Controller: {
						vr::ETrackedControllerRole role = session->hmd.mHMD->GetControllerRoleForTrackedDeviceIndex(i);
						assert(napi_ok == napi_create_string_utf8(env, "tracked-pointer", NAPI_AUTO_LENGTH, &targetRayMode));
						assert(napi_ok == napi_create_string_utf8(env, (role == vr::TrackedControllerRole_RightHand) ? "right" : "left", NAPI_AUTO_LENGTH, &handedness));
					} break;
					// TODO: TrackedDeviceClass_GenericTracker, TrackedDeviceClass_TrackingReference, TrackedDeviceClass_DisplayRedirect
					default: {
						// skip this type
						continue;
					} break;
				}

				// get/create 
				napi_value input;
				assert(napi_ok == napi_get_element(env, inputSources, idx, &input));
				{
					napi_valuetype type;
					assert(napi_ok == napi_typeof(env, input, &type));
					if (type != napi_object) {
						// create object:
						assert(napi_ok == napi_create_object(env, &input));
						assert(napi_ok == napi_set_element(env, inputSources, idx, input));
					}
				}
				
				assert(napi_ok == napi_set_named_property(env, input, "targetRayMode", targetRayMode));
				assert(napi_ok == napi_set_named_property(env, input, "handedness", handedness));

				if (trackedDevicePose.bPoseIsValid) {
					// this is the view matrix relative to the 'chaperone' space origin
					// (the center of the floor space in the real world)
					napi_value ab, ta;
					assert(napi_ok == napi_create_external_arraybuffer(env, (void *)&session->hmd.mDevicePose[i], sizeof(glm::mat4), nullptr, nullptr, &ab));
					assert(napi_ok == napi_create_typedarray(env, napi_float32_array, 16, ab, 0, &ta));
					assert(napi_ok == napi_set_named_property(env, input, "targetRaySpace", ta));
				}

				idx++;
			}
			// set length of array:
			napi_value len;
			assert(napi_ok == napi_create_int32(env, idx, &len));
			assert(napi_ok == napi_set_named_property(env, inputSources, "length", len));
		}

	}

	return nullptr;
}

napi_value submit(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 0);

	uint32_t texid = getUint32(env, args[0]);
	if (session->hmd.connected) {
		session->hmd.submit(texid);
	}

	bool ok = true;
	// TODO
	napi_value result_value;
	status = napi_create_uint32(env, ok ? 1 : 0, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}
