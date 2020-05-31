/* THIS IS A GENERATED FILE -- DO NOT EDIT!! */

#include "node-k4a.h"

napi_value init(napi_env env, napi_value exports) {
	napi_status status;
	napi_property_descriptor properties[] = {
		{ "device_get_installed_count", 0, device_get_installed_count, 0, 0, 0, napi_default, 0 },
		{ "device_open", 0, device_open, 0, 0, 0, napi_default, 0 },
		{ "device_get_serialnum", 0, device_get_serialnum, 0, 0, 0, napi_default, 0 },
		{ "device_set_matrix", 0, device_set_matrix, 0, 0, 0, napi_default, 0 },
		{ "device_start_cameras", 0, device_start_cameras, 0, 0, 0, napi_default, 0 },
		{ "device_start_imu", 0, device_start_imu, 0, 0, 0, napi_default, 0 },
		{ "device_capture", 0, device_capture, 0, 0, 0, napi_default, 0 },
		{ "device_get_color", 0, device_get_color, 0, 0, 0, napi_default, 0 },
		{ "device_get_cloud", 0, device_get_cloud, 0, 0, 0, napi_default, 0 },
		{ "device_get_acc", 0, device_get_acc, 0, 0, 0, napi_default, 0 }
	};
	status = napi_define_properties(env, exports, 10, properties);
	//assert(status == napi_ok);
	return exports;
}
NAPI_MODULE(NODE_GYP_MODULE_NAME, init)