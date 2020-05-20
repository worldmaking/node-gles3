// removed temporarily from node-glfw3 because the symbols are missing from the dll

// napi_value GetError(napi_env env, napi_callback_info info) {
// 	napi_status status = napi_ok;
// 	// int glfwGetError(const char** description)
// 	int result = glfwGetError(nullptr);
// 	napi_value result_value;
// 	status = napi_create_int32(env, (int32_t)result, &result_value);
// 	return (status == napi_ok) ? result_value : nullptr;
// }

// napi_value GetErrorString(napi_env env, napi_callback_info info) {
// 	napi_status status = napi_ok;
// 	const char* description;
// 	// int glfwGetError(const char** description)
// 	int result = glfwGetError(&description);
// 	napi_value result_value;
// 	status = napi_create_string_utf8(env, description, strlen(description), &result_value);
// 	return (status == napi_ok) ? result_value : nullptr;
// }


// napi_value GetWindowContentScale(napi_env env, napi_callback_info info) {
// 	napi_status status = napi_ok;
// 	napi_value args[1];
// 	size_t argc = checkArgCount(env, info, args, 1, 1);

// 	GLFWwindow* window;
// 	napi_valuetype window_type;
// 	status = napi_typeof(env, args[0], &window_type);
// 	if (status != napi_ok || window_type != napi_external) return nullptr;
// 	status = napi_get_value_external(env, args[0], (void **)&window);
// 	if (status != napi_ok) return nullptr;

// 	float x, y;
// 	glfwGetWindowContentScale(window, &x, &y);

// 	// return as array:
// 	napi_value result = nullptr;
// 	status = napi_create_array_with_length(env, 2, &result);
// 	if (status == napi_ok) {
// 			napi_value nx, ny;
// 			napi_create_double(env, x, &nx);
// 			napi_set_element(env, result, 0, nx);
// 			napi_create_double(env, y, &ny);
// 			napi_set_element(env, result, 1, ny);
// 	}
// 	return result;
// }


// napi_value SetWindowMaximizeCallback(napi_env env, napi_callback_info info) {
// 	napi_status status = napi_ok;
// 	napi_value args[2];
// 	size_t argc = checkArgCount(env, info, args, 2, 2);
// 	// get window
// 	GLFWwindow* window = nullptr;
// 	napi_valuetype window_type;
// 	status = napi_typeof(env, args[0], &window_type);
// 	if (status != napi_ok || window_type != napi_external) return nullptr;
// 	status = napi_get_value_external(env, args[0], (void **)&window);
// 	if (status != napi_ok) return nullptr;
// 	// get the userdata for this window:
// 	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
// 	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
// 	// clear out old handler:
// 	if (data->onMaximize ) napi_delete_reference(env, data->onMaximize );
// 	data->onMaximize  = nullptr;
// 	// 2nd arg must be a callable function
// 	napi_value handler = args[1];
// 	napi_valuetype handler_type;
// 	status = napi_typeof(env, args[1], &handler_type);
// 	if (status == napi_ok && handler_type == napi_function) {
// 		// install new one:
// 		napi_create_reference(env, handler, 1, &data->onMaximize );
// 	} 
// 	glfwSetWindowMaximizeCallback(window, windowmaximizefun);
// 	return args[0];
// }


// napi_value SetWindowContentScaleCallback(napi_env env, napi_callback_info info) {
// 	napi_status status = napi_ok;
// 	napi_value args[2];
// 	size_t argc = checkArgCount(env, info, args, 2, 2);
// 	// get window
// 	GLFWwindow* window = nullptr;
// 	napi_valuetype window_type;
// 	status = napi_typeof(env, args[0], &window_type);
// 	if (status != napi_ok || window_type != napi_external) return nullptr;
// 	status = napi_get_value_external(env, args[0], (void **)&window);
// 	if (status != napi_ok) return nullptr;
// 	// get the userdata for this window:
// 	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
// 	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
// 	// clear out old handler:
// 	if (data->onContentScale ) napi_delete_reference(env, data->onContentScale );
// 	data->onContentScale  = nullptr;
// 	// 2nd arg must be a callable function
// 	napi_value handler = args[1];
// 	napi_valuetype handler_type;
// 	status = napi_typeof(env, args[1], &handler_type);
// 	if (status == napi_ok && handler_type == napi_function) {
// 		// install new one:
// 		napi_create_reference(env, handler, 1, &data->onContentScale );
// 	} 
// 	glfwSetWindowContentScaleCallback(window, windowcontentscalefun);
// 	return args[0];
// }


// napi_value GetMonitorContentScale(napi_env env, napi_callback_info info) {
// 	napi_status status = napi_ok;
// 	napi_value args[1];
// 	size_t argc = checkArgCount(env, info, args, 1, 1);

// 	GLFWmonitor* monitor;
// 	napi_valuetype monitor_type;
// 	status = napi_typeof(env, args[0], &monitor_type);
// 	if (status != napi_ok || monitor_type != napi_external) return nullptr;
// 	status = napi_get_value_external(env, args[0], (void **)&monitor);
// 	if (status != napi_ok) return nullptr;

// 	float x, y;
// 	// void glfwGetMonitorPos(GLFWmonitor* monitor, int* xpos, int* ypos)
// 	glfwGetMonitorContentScale(monitor, &x, &y);

// 	// return as array:
// 	napi_value result = nullptr;
// 	status = napi_create_array_with_length(env, 2, &result);
// 	if (status == napi_ok) {
// 		napi_value nx, ny;
// 		napi_create_double(env, x, &nx);
// 		napi_set_element(env, result, 0, nx);
// 		napi_create_double(env, y, &ny);
// 		napi_set_element(env, result, 1, ny);
// 	}
// 	return result;
// }