/* THIS IS A GENERATED FILE -- DO NOT EDIT!! */

#include "node-openvr.h"

napi_value init(napi_env env, napi_value exports) {
	napi_status status;
	napi_property_descriptor properties[] = {
		{ "connect", 0, connect, 0, 0, 0, napi_default, 0 },
		{ "getTextureWidth", 0, getTextureWidth, 0, 0, 0, napi_default, 0 },
		{ "getTextureHeight", 0, getTextureHeight, 0, 0, 0, napi_default, 0 },
		{ "getProjection", 0, getProjection, 0, 0, 0, napi_default, 0 },
		{ "getView", 0, getView, 0, 0, 0, napi_default, 0 },
		{ "inputSources", 0, inputSources, 0, 0, 0, napi_default, 0 },
		{ "update", 0, update, 0, 0, 0, napi_default, 0 },
		{ "submit", 0, submit, 0, 0, 0, napi_default, 0 }
	};
	status = napi_define_properties(env, exports, 8, properties);
	//assert(status == napi_ok);
	return exports;
}
NAPI_MODULE(NODE_GYP_MODULE_NAME, init)