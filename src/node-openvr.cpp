/* THIS IS A GENERATED FILE -- DO NOT EDIT!! */

#include "node-openvr.h"

napi_value init(napi_env env, napi_value exports) {
	napi_status status;
	napi_property_descriptor properties[] = {
		{ "vrConnect", 0, vrConnect, 0, 0, 0, napi_default, 0 },
		{ "vrGetTextureWidth", 0, vrGetTextureWidth, 0, 0, 0, napi_default, 0 },
		{ "vrGetProjection", 0, vrGetProjection, 0, 0, 0, napi_default, 0 },
		{ "vrGetTextureHeight", 0, vrGetTextureHeight, 0, 0, 0, napi_default, 0 },
		{ "vrUpdate", 0, vrUpdate, 0, 0, 0, napi_default, 0 },
		{ "vrSubmit", 0, vrSubmit, 0, 0, 0, napi_default, 0 }
	};
	status = napi_define_properties(env, exports, 6, properties);
	//assert(status == napi_ok);
	return exports;
}
NAPI_MODULE(NODE_GYP_MODULE_NAME, init)