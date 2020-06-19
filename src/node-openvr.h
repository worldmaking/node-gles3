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

	if (session && session->hmd.connected) {
		session->hmd.update();

		// populate the session inputSources:
		napi_value inputSources;
		napi_get_reference_value(env, session->inputSourcesRef, &inputSources);

		/*
		https://developer.mozilla.org/en-US/docs/Web/API/XRInputSource

		.gamepad for buttons & axes
		.gripSpace XRSpace
		.targetRaySpace XRSpace
		.handedness "left" or "right" or "none"
		.targetRayMode "gaze" or "tracked-pointer" or "screen"
		.profiles array of strings for preferred visualization

		*/
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
