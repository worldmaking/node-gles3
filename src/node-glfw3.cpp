/* THIS IS A GENERATED FILE -- DO NOT EDIT!! */

#include "node-glfw3.h"

napi_value Init(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// int glfwInit(void)
	int result = glfwInit();
	napi_value result_value = nullptr;
	status = napi_create_int32(env, (int32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value Terminate(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// void glfwTerminate(void)
	glfwTerminate();
	return NULL;
}

napi_value GetMonitorWorkarea(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLFWmonitor* monitor = nullptr;
	napi_valuetype monitor_type;
	status = napi_typeof(env, args[0], &monitor_type);
	if (status != napi_ok || monitor_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&monitor);
	if (status != napi_ok) return nullptr;
	int* xpos;
	int* ypos;
	int* width;
	int* height;
	// void glfwGetMonitorWorkarea(GLFWmonitor* monitor, int* xpos, int* ypos, int* width, int* height)
	glfwGetMonitorWorkarea(monitor, xpos, ypos, width, height);
	return NULL;
}

napi_value SetGamma(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLFWmonitor* monitor = nullptr;
	napi_valuetype monitor_type;
	status = napi_typeof(env, args[0], &monitor_type);
	if (status != napi_ok || monitor_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&monitor);
	if (status != napi_ok) return nullptr;
	float gamma = getDouble(env, args[1]);
	// void glfwSetGamma(GLFWmonitor* monitor, float gamma)
	glfwSetGamma(monitor, gamma);
	return NULL;
}

napi_value SetGammaRamp(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLFWmonitor* monitor = nullptr;
	napi_valuetype monitor_type;
	status = napi_typeof(env, args[0], &monitor_type);
	if (status != napi_ok || monitor_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&monitor);
	if (status != napi_ok) return nullptr;
	GLFWgammaramp* ramp;
	// void glfwSetGammaRamp(GLFWmonitor* monitor, const GLFWgammaramp* ramp)
	glfwSetGammaRamp(monitor, ramp);
	return NULL;
}

napi_value DefaultWindowHints(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// void glfwDefaultWindowHints(void)
	glfwDefaultWindowHints();
	return NULL;
}

napi_value WindowHint(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	int hint = getInt32(env, args[0]);
	int value = getInt32(env, args[1]);
	// void glfwWindowHint(int hint, int value)
	glfwWindowHint(hint, value);
	return NULL;
}

napi_value DestroyWindow(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// void glfwDestroyWindow(GLFWwindow* window)
	glfwDestroyWindow(window);
	return NULL;
}

napi_value WindowShouldClose(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// int glfwWindowShouldClose(GLFWwindow* window)
	int result = glfwWindowShouldClose(window);
	napi_value result_value = nullptr;
	status = napi_create_int32(env, (int32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value SetWindowShouldClose(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	int value = getInt32(env, args[1]);
	// void glfwSetWindowShouldClose(GLFWwindow* window, int value)
	glfwSetWindowShouldClose(window, value);
	return NULL;
}

napi_value SetWindowTitle(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	char* title = nullptr;
	status = getCharacterArray(env, args[1], title);
	// void glfwSetWindowTitle(GLFWwindow* window, const char* title)
	glfwSetWindowTitle(window, title);
	return NULL;
}

napi_value SetWindowIcon(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	int count = getInt32(env, args[1]);
	GLFWimage* images;
	// void glfwSetWindowIcon(GLFWwindow* window, int count, const GLFWimage* images)
	glfwSetWindowIcon(window, count, images);
	return NULL;
}

napi_value SetWindowPos(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	int xpos = getInt32(env, args[1]);
	int ypos = getInt32(env, args[2]);
	// void glfwSetWindowPos(GLFWwindow* window, int xpos, int ypos)
	glfwSetWindowPos(window, xpos, ypos);
	return NULL;
}

napi_value SetWindowSizeLimits(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	int minwidth = getInt32(env, args[1]);
	int minheight = getInt32(env, args[2]);
	int maxwidth = getInt32(env, args[3]);
	int maxheight = getInt32(env, args[4]);
	// void glfwSetWindowSizeLimits(GLFWwindow* window, int minwidth, int minheight, int maxwidth, int maxheight)
	glfwSetWindowSizeLimits(window, minwidth, minheight, maxwidth, maxheight);
	return NULL;
}

napi_value SetWindowAspectRatio(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	int numer = getInt32(env, args[1]);
	int denom = getInt32(env, args[2]);
	// void glfwSetWindowAspectRatio(GLFWwindow* window, int numer, int denom)
	glfwSetWindowAspectRatio(window, numer, denom);
	return NULL;
}

napi_value SetWindowSize(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	int width = getInt32(env, args[1]);
	int height = getInt32(env, args[2]);
	// void glfwSetWindowSize(GLFWwindow* window, int width, int height)
	glfwSetWindowSize(window, width, height);
	return NULL;
}

napi_value IconifyWindow(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// void glfwIconifyWindow(GLFWwindow* window)
	glfwIconifyWindow(window);
	return NULL;
}

napi_value RestoreWindow(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// void glfwRestoreWindow(GLFWwindow* window)
	glfwRestoreWindow(window);
	return NULL;
}

napi_value MaximizeWindow(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// void glfwMaximizeWindow(GLFWwindow* window)
	glfwMaximizeWindow(window);
	return NULL;
}

napi_value ShowWindow(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// void glfwShowWindow(GLFWwindow* window)
	glfwShowWindow(window);
	return NULL;
}

napi_value HideWindow(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// void glfwHideWindow(GLFWwindow* window)
	glfwHideWindow(window);
	return NULL;
}

napi_value FocusWindow(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// void glfwFocusWindow(GLFWwindow* window)
	glfwFocusWindow(window);
	return NULL;
}

napi_value GetWindowMonitor(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// GLFWmonitor* glfwGetWindowMonitor(GLFWwindow* window)
	GLFWmonitor* result = glfwGetWindowMonitor(window);
	napi_value result_value = nullptr;
	status = napi_create_external(env, (void *)result, NULL, NULL, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetWindowAttrib(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	int attrib = getInt32(env, args[1]);
	// int glfwGetWindowAttrib(GLFWwindow* window, int attrib)
	int result = glfwGetWindowAttrib(window, attrib);
	napi_value result_value = nullptr;
	status = napi_create_int32(env, (int32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value PollEvents(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// void glfwPollEvents(void)
	glfwPollEvents();
	return NULL;
}

napi_value WaitEvents(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// void glfwWaitEvents(void)
	glfwWaitEvents();
	return NULL;
}

napi_value WaitEventsTimeout(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	double timeout;
	// void glfwWaitEventsTimeout(double timeout)
	glfwWaitEventsTimeout(timeout);
	return NULL;
}

napi_value PostEmptyEvent(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// void glfwPostEmptyEvent(void)
	glfwPostEmptyEvent();
	return NULL;
}

napi_value GetInputMode(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	int mode = getInt32(env, args[1]);
	// int glfwGetInputMode(GLFWwindow* window, int mode)
	int result = glfwGetInputMode(window, mode);
	napi_value result_value = nullptr;
	status = napi_create_int32(env, (int32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value SetInputMode(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	int mode = getInt32(env, args[1]);
	int value = getInt32(env, args[2]);
	// void glfwSetInputMode(GLFWwindow* window, int mode, int value)
	glfwSetInputMode(window, mode, value);
	return NULL;
}

napi_value RawMouseMotionSupported(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// int glfwRawMouseMotionSupported(void)
	int result = glfwRawMouseMotionSupported();
	napi_value result_value = nullptr;
	status = napi_create_int32(env, (int32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetKey(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	int key = getInt32(env, args[1]);
	// int glfwGetKey(GLFWwindow* window, int key)
	int result = glfwGetKey(window, key);
	napi_value result_value = nullptr;
	status = napi_create_int32(env, (int32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetMouseButton(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	int button = getInt32(env, args[1]);
	// int glfwGetMouseButton(GLFWwindow* window, int button)
	int result = glfwGetMouseButton(window, button);
	napi_value result_value = nullptr;
	status = napi_create_int32(env, (int32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetCursorPos(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	double* xpos;
	double* ypos;
	// void glfwGetCursorPos(GLFWwindow* window, double* xpos, double* ypos)
	glfwGetCursorPos(window, xpos, ypos);
	return NULL;
}

napi_value SetCursorPos(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	double xpos;
	double ypos;
	// void glfwSetCursorPos(GLFWwindow* window, double xpos, double ypos)
	glfwSetCursorPos(window, xpos, ypos);
	return NULL;
}

napi_value SetDropCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	GLFWdropfun callback;
	// GLFWdropfun glfwSetDropCallback(GLFWwindow* window, GLFWdropfun callback)
	GLFWdropfun result = glfwSetDropCallback(window, callback);
}

napi_value JoystickPresent(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	int jid = getInt32(env, args[0]);
	// int glfwJoystickPresent(int jid)
	int result = glfwJoystickPresent(jid);
	napi_value result_value = nullptr;
	status = napi_create_int32(env, (int32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value SetClipboardString(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	char* string = nullptr;
	status = getCharacterArray(env, args[1], string);
	// void glfwSetClipboardString(GLFWwindow* window, const char* string)
	glfwSetClipboardString(window, string);
	return NULL;
}

napi_value GetTime(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// double glfwGetTime(void)
	double result = glfwGetTime();
	napi_value result_value = nullptr;
	status = napi_create_double(env, (double)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value SetTime(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	double time;
	// void glfwSetTime(double time)
	glfwSetTime(time);
	return NULL;
}

napi_value MakeContextCurrent(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// void glfwMakeContextCurrent(GLFWwindow* window)
	glfwMakeContextCurrent(window);
	return NULL;
}

napi_value SwapBuffers(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// void glfwSwapBuffers(GLFWwindow* window)
	glfwSwapBuffers(window);
	return NULL;
}

napi_value SwapInterval(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	int interval = getInt32(env, args[0]);
	// void glfwSwapInterval(int interval)
	glfwSwapInterval(interval);
	return NULL;
}

napi_value ExtensionSupported(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	char* extension = nullptr;
	status = getCharacterArray(env, args[0], extension);
	// int glfwExtensionSupported(const char* extension)
	int result = glfwExtensionSupported(extension);
	napi_value result_value = nullptr;
	status = napi_create_int32(env, (int32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value init(napi_env env, napi_value exports) {
	napi_status status;
	napi_property_descriptor properties[] = {
		{ "getVersion", 0, GetVersion, 0, 0, 0, napi_default, 0 },
		{ "getVersionString", 0, GetVersionString, 0, 0, 0, napi_default, 0 },
		{ "getMonitors", 0, GetMonitors, 0, 0, 0, napi_default, 0 },
		{ "getPrimaryMonitor", 0, GetPrimaryMonitor, 0, 0, 0, napi_default, 0 },
		{ "getMonitorPos", 0, GetMonitorPos, 0, 0, 0, napi_default, 0 },
		{ "getMonitorPhysicalSize", 0, GetMonitorPhysicalSize, 0, 0, 0, napi_default, 0 },
		{ "getVideoMode", 0, GetVideoMode, 0, 0, 0, napi_default, 0 },
		{ "createWindow", 0, CreateWindow, 0, 0, 0, napi_default, 0 },
		{ "getCurrentContext", 0, GetCurrentContext, 0, 0, 0, napi_default, 0 },
		{ "getWindowPos", 0, GetWindowPos, 0, 0, 0, napi_default, 0 },
		{ "getWindowSize", 0, GetWindowSize, 0, 0, 0, napi_default, 0 },
		{ "getWindowContentScale", 0, GetWindowContentScale, 0, 0, 0, napi_default, 0 },
		{ "getFramebufferSize", 0, GetFramebufferSize, 0, 0, 0, napi_default, 0 },
		{ "getWindowFrameSize", 0, GetWindowFrameSize, 0, 0, 0, napi_default, 0 },
		{ "setWindowMonitor", 0, SetWindowMonitor, 0, 0, 0, napi_default, 0 },
		{ "setWindowAttrib", 0, SetWindowAttrib, 0, 0, 0, napi_default, 0 },
		{ "setWindowPosCallback", 0, SetWindowPosCallback, 0, 0, 0, napi_default, 0 },
		{ "setWindowSizeCallback", 0, SetWindowSizeCallback, 0, 0, 0, napi_default, 0 },
		{ "setWindowCloseCallback", 0, SetWindowCloseCallback, 0, 0, 0, napi_default, 0 },
		{ "setWindowRefreshCallback", 0, SetWindowRefreshCallback, 0, 0, 0, napi_default, 0 },
		{ "setWindowFocusCallback", 0, SetWindowFocusCallback, 0, 0, 0, napi_default, 0 },
		{ "setWindowIconifyCallback", 0, SetWindowIconifyCallback, 0, 0, 0, napi_default, 0 },
		{ "setFramebufferSizeCallback", 0, SetFramebufferSizeCallback, 0, 0, 0, napi_default, 0 },
		{ "setWindowContentScaleCallback", 0, SetWindowContentScaleCallback, 0, 0, 0, napi_default, 0 },
		{ "setMouseButtonCallback", 0, SetMouseButtonCallback, 0, 0, 0, napi_default, 0 },
		{ "setCursorPosCallback", 0, SetCursorPosCallback, 0, 0, 0, napi_default, 0 },
		{ "setCursorEnterCallback", 0, SetCursorEnterCallback, 0, 0, 0, napi_default, 0 },
		{ "setScrollCallback", 0, SetScrollCallback, 0, 0, 0, napi_default, 0 },
		{ "setKeyCallback", 0, SetKeyCallback, 0, 0, 0, napi_default, 0 },
		{ "setCharCallback", 0, SetCharCallback, 0, 0, 0, napi_default, 0 },
		{ "setCharModsCallback", 0, SetCharModsCallback, 0, 0, 0, napi_default, 0 },
		{ "setErrorCallback", 0, SetErrorCallback, 0, 0, 0, napi_default, 0 },
		{ "setJoystickCallback", 0, SetJoystickCallback, 0, 0, 0, napi_default, 0 },
		{ "setMonitorUserPointer", 0, SetMonitorUserPointer, 0, 0, 0, napi_default, 0 },
		{ "getMonitorUserPointer", 0, GetMonitorUserPointer, 0, 0, 0, napi_default, 0 },
		{ "setWindowUserPointer", 0, SetWindowUserPointer, 0, 0, 0, napi_default, 0 },
		{ "getWindowUserPointer", 0, GetWindowUserPointer, 0, 0, 0, napi_default, 0 },
		{ "setMonitorCallback", 0, SetMonitorCallback, 0, 0, 0, napi_default, 0 },
		{ "vulkanSupported", 0, VulkanSupported, 0, 0, 0, napi_default, 0 },
		{ "getInstanceProcAddress", 0, GetInstanceProcAddress, 0, 0, 0, napi_default, 0 },
		{ "getPhysicalDevicePresentationSupport", 0, GetPhysicalDevicePresentationSupport, 0, 0, 0, napi_default, 0 },
		{ "createWindowSurface", 0, CreateWindowSurface, 0, 0, 0, napi_default, 0 },
		{ "createCursor", 0, CreateCursor, 0, 0, 0, napi_default, 0 },
		{ "createStandardCursor", 0, CreateStandardCursor, 0, 0, 0, napi_default, 0 },
		{ "destroyCursor", 0, DestroyCursor, 0, 0, 0, napi_default, 0 },
		{ "setCursor", 0, SetCursor, 0, 0, 0, napi_default, 0 },
		{ "getProcAddress", 0, GetProcAddress, 0, 0, 0, napi_default, 0 },
		{ "init", 0, Init, 0, 0, 0, napi_default, 0 },
		{ "terminate", 0, Terminate, 0, 0, 0, napi_default, 0 },
		{ "getMonitorWorkarea", 0, GetMonitorWorkarea, 0, 0, 0, napi_default, 0 },
		{ "setGamma", 0, SetGamma, 0, 0, 0, napi_default, 0 },
		{ "setGammaRamp", 0, SetGammaRamp, 0, 0, 0, napi_default, 0 },
		{ "defaultWindowHints", 0, DefaultWindowHints, 0, 0, 0, napi_default, 0 },
		{ "windowHint", 0, WindowHint, 0, 0, 0, napi_default, 0 },
		{ "destroyWindow", 0, DestroyWindow, 0, 0, 0, napi_default, 0 },
		{ "windowShouldClose", 0, WindowShouldClose, 0, 0, 0, napi_default, 0 },
		{ "setWindowShouldClose", 0, SetWindowShouldClose, 0, 0, 0, napi_default, 0 },
		{ "setWindowTitle", 0, SetWindowTitle, 0, 0, 0, napi_default, 0 },
		{ "setWindowIcon", 0, SetWindowIcon, 0, 0, 0, napi_default, 0 },
		{ "setWindowPos", 0, SetWindowPos, 0, 0, 0, napi_default, 0 },
		{ "setWindowSizeLimits", 0, SetWindowSizeLimits, 0, 0, 0, napi_default, 0 },
		{ "setWindowAspectRatio", 0, SetWindowAspectRatio, 0, 0, 0, napi_default, 0 },
		{ "setWindowSize", 0, SetWindowSize, 0, 0, 0, napi_default, 0 },
		{ "iconifyWindow", 0, IconifyWindow, 0, 0, 0, napi_default, 0 },
		{ "restoreWindow", 0, RestoreWindow, 0, 0, 0, napi_default, 0 },
		{ "maximizeWindow", 0, MaximizeWindow, 0, 0, 0, napi_default, 0 },
		{ "showWindow", 0, ShowWindow, 0, 0, 0, napi_default, 0 },
		{ "hideWindow", 0, HideWindow, 0, 0, 0, napi_default, 0 },
		{ "focusWindow", 0, FocusWindow, 0, 0, 0, napi_default, 0 },
		{ "getWindowMonitor", 0, GetWindowMonitor, 0, 0, 0, napi_default, 0 },
		{ "getWindowAttrib", 0, GetWindowAttrib, 0, 0, 0, napi_default, 0 },
		{ "pollEvents", 0, PollEvents, 0, 0, 0, napi_default, 0 },
		{ "waitEvents", 0, WaitEvents, 0, 0, 0, napi_default, 0 },
		{ "waitEventsTimeout", 0, WaitEventsTimeout, 0, 0, 0, napi_default, 0 },
		{ "postEmptyEvent", 0, PostEmptyEvent, 0, 0, 0, napi_default, 0 },
		{ "getInputMode", 0, GetInputMode, 0, 0, 0, napi_default, 0 },
		{ "setInputMode", 0, SetInputMode, 0, 0, 0, napi_default, 0 },
		{ "rawMouseMotionSupported", 0, RawMouseMotionSupported, 0, 0, 0, napi_default, 0 },
		{ "getKey", 0, GetKey, 0, 0, 0, napi_default, 0 },
		{ "getMouseButton", 0, GetMouseButton, 0, 0, 0, napi_default, 0 },
		{ "getCursorPos", 0, GetCursorPos, 0, 0, 0, napi_default, 0 },
		{ "setCursorPos", 0, SetCursorPos, 0, 0, 0, napi_default, 0 },
		{ "setDropCallback", 0, SetDropCallback, 0, 0, 0, napi_default, 0 },
		{ "joystickPresent", 0, JoystickPresent, 0, 0, 0, napi_default, 0 },
		{ "setClipboardString", 0, SetClipboardString, 0, 0, 0, napi_default, 0 },
		{ "getTime", 0, GetTime, 0, 0, 0, napi_default, 0 },
		{ "setTime", 0, SetTime, 0, 0, 0, napi_default, 0 },
		{ "makeContextCurrent", 0, MakeContextCurrent, 0, 0, 0, napi_default, 0 },
		{ "swapBuffers", 0, SwapBuffers, 0, 0, 0, napi_default, 0 },
		{ "swapInterval", 0, SwapInterval, 0, 0, 0, napi_default, 0 },
		{ "extensionSupported", 0, ExtensionSupported, 0, 0, 0, napi_default, 0 }
	};
	status = napi_define_properties(env, exports, 91, properties);
	//assert(status == napi_ok);
	return exports;
}
NAPI_MODULE(NODE_GYP_MODULE_NAME, init)