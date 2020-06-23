#include "node-api-helpers.h"
#include <GLFW/glfw3.h>

///////////////////////////////////////////////////////////////////////////
// now the GLFW bindings:
///////////////////////////////////////////////////////////////////////////

#define GLFW_WINDOWSTATE_MAGIC 'glwn'

struct WindowState {
	uint32_t magic = GLFW_WINDOWSTATE_MAGIC;
	napi_env env;
	napi_ref windowRef;

	napi_ref onPos  = nullptr;
	napi_ref onSize = nullptr;
	napi_ref onClose = nullptr;
	napi_ref onRefresh = nullptr;
	napi_ref onFocus = nullptr;
	napi_ref onIconify = nullptr;
	napi_ref onMaximize  = nullptr;
	napi_ref onFramebufferSize  = nullptr;
	napi_ref onContentScale = nullptr;
	napi_ref onMouseButton = nullptr;
	napi_ref onCursorPos = nullptr;
	napi_ref onCursorEnter = nullptr;
	napi_ref onScroll = nullptr;
	napi_ref onDrop = nullptr;
	napi_ref onChar  = nullptr;
	napi_ref onKey = nullptr;
	napi_ref onCharMods = nullptr;
};


napi_value GetVersion(napi_env env, napi_callback_info info) {
	napi_value result_value = nullptr;
	napi_status status = napi_ok;
	int maj, min, rev;
	glfwGetVersion(&maj, &min, &rev);
	// return object
	status = napi_create_object(env, &result_value);
	if (status == napi_ok) {
		napi_value nmaj, nmin, nrev;
		napi_create_int32(env, maj, &nmaj);
		napi_create_int32(env, min, &nmin);
		napi_create_int32(env, rev, &nrev);
		napi_set_named_property(env, result_value, "major", nmaj);
		napi_set_named_property(env, result_value, "minor", nmin);
		napi_set_named_property(env, result_value, "rev", nrev);
	}
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetVersionString(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	const char * description = glfwGetVersionString();

	napi_value result_value;
	status = napi_create_string_utf8(env, description, strlen(description), &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}


// treating GLFWmonitor* here as an "external" 
napi_value GetMonitors(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	int count;
	// GLFWmonitor** glfwGetMonitors(int* count)
	GLFWmonitor** monitors = glfwGetMonitors(&count);

	// return an array of pointers?
	napi_value result = nullptr;
	status = napi_create_array_with_length(env, count, &result);
	if (status == napi_ok) {
		for (int i=0; i<count; i++) {
			napi_value val;
			napi_create_external(env, monitors[i], nullptr, nullptr, &val);
			napi_set_element(env, result, i, val);
		}
	}
	return result;
}

napi_value GetPrimaryMonitor(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	// GLFWmonitor* glfwGetPrimaryMonitor(void)
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	napi_value result_value = nullptr;
	status = napi_create_external(env, monitor, nullptr, nullptr, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetMonitorPos(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);

	GLFWmonitor* monitor;
	napi_valuetype monitor_type;
	status = napi_typeof(env, args[0], &monitor_type);
	if (status != napi_ok || monitor_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&monitor);
	if (status != napi_ok) return nullptr;

	int xpos;
	int ypos;
	// void glfwGetMonitorPos(GLFWmonitor* monitor, int* xpos, int* ypos)
	glfwGetMonitorPos(monitor, &xpos, &ypos);

	// return as array:
	napi_value result = nullptr;
	status = napi_create_array_with_length(env, 2, &result);
	if (status == napi_ok) {
		napi_value nx, ny;
		napi_create_int32(env, xpos, &nx);
		napi_set_element(env, result, 0, nx);
		napi_create_int32(env, ypos, &ny);
		napi_set_element(env, result, 1, ny);
	}
	return result;
}

napi_value GetMonitorPhysicalSize(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);

	GLFWmonitor* monitor;
	napi_valuetype monitor_type;
	status = napi_typeof(env, args[0], &monitor_type);
	if (status != napi_ok || monitor_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&monitor);
	if (status != napi_ok) return nullptr;

	int xpos;
	int ypos;
	// void glfwGetMonitorPos(GLFWmonitor* monitor, int* xpos, int* ypos)
	glfwGetMonitorPhysicalSize(monitor, &xpos, &ypos);

	// return as array:
	napi_value result = nullptr;
	status = napi_create_array_with_length(env, 2, &result);
	if (status == napi_ok) {
		napi_value nx, ny;
		napi_create_int32(env, xpos, &nx);
		napi_set_element(env, result, 0, nx);
		napi_create_int32(env, ypos, &ny);
		napi_set_element(env, result, 1, ny);
	}
	return result;
}



// width, height, title, [monitor], [shared context window]
napi_value CreateWindow(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 3);
	int width = getInt32(env, args[0]);
	int height = getInt32(env, args[1]);

	static const size_t maxlen = 256;
	char title[maxlen];
	size_t len;
	status = napi_get_value_string_utf8(env, args[2], title, maxlen, &len);
	
	// optional monitor to fullscreen onto:
	GLFWmonitor* monitor = nullptr;
	napi_valuetype monitor_type;
	status = napi_typeof(env, args[3], &monitor_type);
	if (status == napi_ok && monitor_type == napi_external) {
		napi_get_value_external(env, args[3], (void **)&monitor);
	}

	// optional window to share context with:
	GLFWwindow* share = nullptr;
	napi_valuetype share_type;
	status = napi_typeof(env, args[4], &share_type);
	if (status == napi_ok && share_type == napi_external) {
		napi_get_value_external(env, args[4], (void **)&share);
	}
	
	// GLFWwindow* glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
	GLFWwindow * win = glfwCreateWindow(width, height, title, monitor, share);
	napi_value result_value;
	status = napi_create_external(env, win, nullptr, nullptr, &result_value);

	// stash a state pointer in the window:
	WindowState * ws = (WindowState *)calloc(1, sizeof(WindowState));
	ws->magic = GLFW_WINDOWSTATE_MAGIC;
	ws->env = env;
	napi_create_reference(env, result_value, 1, &ws->windowRef); 
	glfwSetWindowUserPointer(win, ws);

	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetCurrentContext(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// GLFWwindow* glfwGetCurrentContext(void)
	GLFWwindow* win = glfwGetCurrentContext();
	napi_value result_value;
	status = napi_create_external(env, win, nullptr, nullptr, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}


napi_value GetWindowPos(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);

	GLFWwindow* win;
	napi_valuetype win_type;
	status = napi_typeof(env, args[0], &win_type);
	if (status != napi_ok || win_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&win);
	if (status != napi_ok) return nullptr;

	int xpos;
	int ypos;
	// void glfwGetMonitorPos(GLFWmonitor* monitor, int* xpos, int* ypos)
	glfwGetWindowPos(win, &xpos, &ypos);

	// return as array:
	napi_value result = nullptr;
	status = napi_create_array_with_length(env, 2, &result);
	if (status == napi_ok) {
		napi_value nx, ny;
		napi_create_int32(env, xpos, &nx);
		napi_set_element(env, result, 0, nx);
		napi_create_int32(env, ypos, &ny);
		napi_set_element(env, result, 1, ny);
	}
	return result;
}

napi_value GetWindowSize(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);

	GLFWwindow* win;
	napi_valuetype win_type;
	status = napi_typeof(env, args[0], &win_type);
	if (status != napi_ok || win_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&win);
	if (status != napi_ok) return nullptr;

	int xpos;
	int ypos;
	// void glfwGetMonitorPos(GLFWmonitor* monitor, int* xpos, int* ypos)
	glfwGetWindowSize(win, &xpos, &ypos);

	// return as array:
	napi_value result = nullptr;
	status = napi_create_array_with_length(env, 2, &result);
	if (status == napi_ok) {
			napi_value nx, ny;
			napi_create_int32(env, xpos, &nx);
			napi_set_element(env, result, 0, nx);
			napi_create_int32(env, ypos, &ny);
			napi_set_element(env, result, 1, ny);
	}
	return result;
}

napi_value GetWindowContentScale(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value result = nullptr;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);

	GLFWwindow* win;
	napi_valuetype win_type;
	status = napi_typeof(env, args[0], &win_type);
	if (status != napi_ok || win_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&win);
	if (status != napi_ok) return nullptr;

	float x;
	float y;
	
	if (win) {
		glfwGetWindowContentScale(win, &x, &y);

		// return as array:
		status = napi_create_array_with_length(env, 2, &result);
		if (status == napi_ok) {
				napi_value nx, ny;
				napi_create_double(env, x, &nx);
				napi_set_element(env, result, 0, nx);
				napi_create_double(env, y, &ny);
				napi_set_element(env, result, 1, ny);
		}
	}
	return result;
}

napi_value GetFramebufferSize(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);

	GLFWwindow* win;
	napi_valuetype win_type;
	status = napi_typeof(env, args[0], &win_type);
	if (status != napi_ok || win_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&win);
	if (status != napi_ok) return nullptr;

	int xpos;
	int ypos;
	// void glfwGetMonitorPos(GLFWmonitor* monitor, int* xpos, int* ypos)
	glfwGetFramebufferSize(win, &xpos, &ypos);

	// return as array:
	napi_value result = nullptr;
	status = napi_create_array_with_length(env, 2, &result);
	if (status == napi_ok) {
			napi_value nx, ny;
			napi_create_int32(env, xpos, &nx);
			napi_set_element(env, result, 0, nx);
			napi_create_int32(env, ypos, &ny);
			napi_set_element(env, result, 1, ny);
	}
	return result;
}


napi_value GetWindowFrameSize(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);

	GLFWwindow* win;
	napi_valuetype win_type;
	status = napi_typeof(env, args[0], &win_type);
	if (status != napi_ok || win_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&win);
	if (status != napi_ok) return nullptr;

	int l, t, r, b;
	// void glfwGetWindowFrameSize(GLFWwindow* window, int* left, int* top, int* right, int* bottom)
	glfwGetWindowFrameSize(win, &l, &t, &r, &b);

	// return as array:
	napi_value result = nullptr;
	status = napi_create_array_with_length(env, 2, &result);
	if (status == napi_ok) {
			napi_value nl, nt, nr, nb;
			napi_create_int32(env, l, &nl);
			napi_set_element(env, result, 0, nl);
			napi_create_int32(env, t, &nt);
			napi_set_element(env, result, 1, nt);
			napi_create_int32(env, r, &nr);
			napi_set_element(env, result, 0, nr);
			napi_create_int32(env, b, &nb);
			napi_set_element(env, result, 1, nb);
	}
	return result;
}



napi_value SetWindowMonitor(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[7];
	size_t argc = checkArgCount(env, info, args, 7, 7);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	GLFWmonitor* monitor = nullptr;
	napi_valuetype monitor_type;
	status = napi_typeof(env, args[1], &monitor_type);
	if (status == napi_ok && monitor_type == napi_external) {
		status = napi_get_value_external(env, args[1], (void **)&monitor);
	}
	int xpos = getInt32(env, args[2]);
	int ypos = getInt32(env, args[3]);
	int width = getInt32(env, args[4]);
	int height = getInt32(env, args[5]);
	int refreshRate = getInt32(env, args[6]);
	// void glfwSetWindowMonitor(GLFWwindow* window, GLFWmonitor* monitor, int xpos, int ypos, int width, int height, int refreshRate)
	glfwSetWindowMonitor(window, monitor, xpos, ypos, width, height, refreshRate);
	return NULL;
}


void windowposfun(GLFWwindow* window, int x, int y) {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onPos  == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onPos , &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 3;
	napi_value argv[3];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	napi_create_int32(env, x, &argv[1]);
	napi_create_int32(env, y, &argv[2]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}

napi_value SetWindowPosCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onPos ) napi_delete_reference(env, data->onPos );
	data->onPos  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onPos );
	} 
	glfwSetWindowPosCallback(window, windowposfun);
	return args[0];
}

void windowsizefun(GLFWwindow* window, int x, int y) {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onSize  == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onSize , &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 3;
	napi_value argv[3];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	napi_create_int32(env, x, &argv[1]);
	napi_create_int32(env, y, &argv[2]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}

napi_value SetWindowSizeCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onSize ) napi_delete_reference(env, data->onSize );
	data->onSize  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onSize );
	} 
	glfwSetWindowSizeCallback(window, windowsizefun);
	return args[0];
}

void windowclosefun(GLFWwindow* window) {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onClose  == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onClose , &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 1;
	napi_value argv[1];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}

napi_value SetWindowCloseCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onClose ) napi_delete_reference(env, data->onClose );
	data->onClose  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onClose );
	} 
	glfwSetWindowCloseCallback(window, windowclosefun);
	return args[0];
}

void windowrefreshfun(GLFWwindow* window) {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onRefresh  == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onRefresh, &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 1;
	napi_value argv[1];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}

napi_value SetWindowRefreshCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onRefresh ) napi_delete_reference(env, data->onRefresh );
	data->onRefresh  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onRefresh );
	} 
	glfwSetWindowRefreshCallback(window, windowrefreshfun);
	return args[0];
}

void windowfocusfun(GLFWwindow* window,int x) {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onFocus  == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onFocus , &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 2;
	napi_value argv[2];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	napi_create_int32(env, x, &argv[1]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}


napi_value SetWindowFocusCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onFocus ) napi_delete_reference(env, data->onFocus );
	data->onFocus  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onFocus );
	} 
	glfwSetWindowFocusCallback(window, windowfocusfun);
	return args[0];
}

void windowiconifyfun(GLFWwindow* window,int x) {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onIconify  == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onIconify , &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 2;
	napi_value argv[2];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	napi_create_int32(env, x, &argv[1]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}


napi_value SetWindowIconifyCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onIconify ) napi_delete_reference(env, data->onIconify );
	data->onIconify  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onIconify );
	} 
	glfwSetWindowIconifyCallback(window, windowiconifyfun);
	return args[0];
}


void windowmaximizefun(GLFWwindow* window,int x) {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onMaximize  == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onMaximize , &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 2;
	napi_value argv[2];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	napi_create_int32(env, x, &argv[1]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}


void windowframebuffersizefun(GLFWwindow* window,int x,int y){
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onFramebufferSize  == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onFramebufferSize , &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 3;
	napi_value argv[3];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	napi_create_int32(env, x, &argv[1]);
	napi_create_int32(env, y, &argv[2]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}


napi_value SetFramebufferSizeCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onFramebufferSize ) napi_delete_reference(env, data->onFramebufferSize );
	data->onFramebufferSize  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onFramebufferSize );
	} 
	glfwSetFramebufferSizeCallback(window, windowframebuffersizefun);
	return args[0];
}

void windowcontentscalefun(GLFWwindow* window,float x,float y) {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onContentScale  == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onContentScale , &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 3;
	napi_value argv[3];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	napi_create_double(env, x, &argv[1]);
	napi_create_double(env, y, &argv[2]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}

napi_value SetWindowContentScaleCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onContentScale ) napi_delete_reference(env, data->onContentScale );
	data->onContentScale  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onContentScale );
	} 
	glfwSetWindowContentScaleCallback(window, windowcontentscalefun);
	return args[0];
}

void windowmousebuttonfun(GLFWwindow* window,int x,int y,int z) {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onMouseButton  == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onMouseButton , &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 4;
	napi_value argv[4];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	napi_create_int32(env, x, &argv[1]);
	napi_create_int32(env, y, &argv[2]);
	napi_create_int32(env, z, &argv[3]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}

napi_value SetMouseButtonCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onMouseButton ) napi_delete_reference(env, data->onMouseButton );
	data->onMouseButton  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onMouseButton );
	} 
	glfwSetMouseButtonCallback(window, windowmousebuttonfun);
	return args[0];
}

void windowcursorposfun(GLFWwindow* window,double x,double y) {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onCursorPos  == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onCursorPos , &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 3;
	napi_value argv[3];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	napi_create_double(env, x, &argv[1]);
	napi_create_double(env, y, &argv[2]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}

napi_value SetCursorPosCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onCursorPos ) napi_delete_reference(env, data->onCursorPos );
	data->onCursorPos  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onCursorPos );
	} 
	glfwSetCursorPosCallback(window, windowcursorposfun);
	return args[0];
}

void windowcursorenterfun(GLFWwindow* window,int x) {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onCursorEnter  == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onCursorEnter , &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 2;
	napi_value argv[2];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	napi_create_int32(env, x, &argv[1]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}

napi_value SetCursorEnterCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onCursorEnter ) napi_delete_reference(env, data->onCursorEnter );
	data->onCursorEnter  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onCursorEnter );
	} 
	glfwSetCursorEnterCallback(window, windowcursorenterfun);
	return args[0];
}

void windowscrollfun(GLFWwindow* window,double x,double y)  {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onScroll  == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onScroll , &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 3;
	napi_value argv[3];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	napi_create_double(env, x, &argv[1]);
	napi_create_double(env, y, &argv[2]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}

napi_value SetScrollCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onScroll ) napi_delete_reference(env, data->onScroll );
	data->onScroll  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onScroll );
	} 
	glfwSetScrollCallback(window, windowscrollfun);
	return args[0];
}

void windowkeyfun(GLFWwindow* window, int x,int y,int z,int w) {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onKey == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onKey, &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 5;
	napi_value argv[5];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	napi_create_int32(env, x, &argv[1]);
	napi_create_int32(env, y, &argv[2]);
	napi_create_int32(env, z, &argv[3]);
	napi_create_int32(env, w, &argv[4]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}


napi_value SetKeyCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onKey ) napi_delete_reference(env, data->onKey );
	data->onKey  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onKey );
	} 
	glfwSetKeyCallback(window, windowkeyfun);
	return args[0];
}

void windowcharfun(GLFWwindow* window, unsigned int x) {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onPos  == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onPos , &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 2;
	napi_value argv[2];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	napi_create_uint32(env, x, &argv[1]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}

napi_value SetCharCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onChar ) napi_delete_reference(env, data->onChar );
	data->onChar  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onChar );
	} 
	glfwSetCharCallback(window, windowcharfun);
	return args[0];
}

void windowcharmodsfun(GLFWwindow* window,unsigned int x,int y) {
	napi_status status = napi_ok;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onCharMods == nullptr) return;
	napi_env env = data->env;
	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(env, data->onCharMods, &callback)
	|| napi_ok != napi_typeof(env, callback, &callback_type)
	|| callback_type != napi_function) return;
	// prepare the args:
	int argc = 3;
	napi_value argv[3];
	napi_get_reference_value(env, data->windowRef, &argv[0]);
	napi_create_uint32(env, x, &argv[1]);
	napi_create_int32(env, y, &argv[2]);
	// use global namespace as the `this`:
	napi_value global;
	status = napi_get_global(env, &global);
	// now call it:
	napi_value result;
	status = napi_call_function(env, global, callback, argc, argv, &result);
}


napi_value SetCharModsCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	// get window
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC) return nullptr;
	// clear out old handler:
	if (data->onCharMods ) napi_delete_reference(env, data->onCharMods );
	data->onCharMods  = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[1], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onCharMods );
	} 
	glfwSetCharModsCallback(window, windowcharmodsfun);
	return args[0];
}


// TODO:
// void windowdropfun(GLFWwindow* window,int x,const char** y) {
// 	napi_status status = napi_ok;
// 	// get the userdata for this window:
// 	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
// 	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onDrop == nullptr) return;
// 	napi_env env = data->env;
// 	// retrieve the handler:
// 	napi_value callback; 
// 	napi_valuetype callback_type;
// 	if (napi_ok != napi_get_reference_value(env, data->onDrop, &callback)
// 	|| napi_ok != napi_typeof(env, callback, &callback_type)
// 	|| callback_type != napi_function) return;
// 	// prepare the args:
// 	int argc = 3;
// 	napi_value argv[3];
// 	napi_get_reference_value(env, data->windowRef, &argv[0]);
// 	napi_create_int32(env, x, &argv[1]);
// 	napi_create_string_utf8(env, y, strlen(y), &argv[2]);
// 	// use global namespace as the `this`:
// 	napi_value global;
// 	status = napi_get_global(env, &global);
// 	// now call it:
// 	napi_value result;
// 	status = napi_call_function(env, global, callback, argc, argv, &result);
// }

//void Errorfun(int code, const char* msg) {}
// since there's no userdata here, the only way we could get this to work is to 
// stash a singleton user handler in the napi_env itself somehow
// I'm not sure how to do that yet.
napi_value SetErrorCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLFWerrorfun cbfun;
	// GLFWerrorfun glfwSetErrorCallback(GLFWerrorfun cbfun)
	return nullptr;
}


napi_value SetJoystickCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	//size_t argc = checkArgCount(env, info, args, 1, 1);
	//GLFWjoystickfun cbfun;
	// GLFWjoystickfun glfwSetJoystickCallback(GLFWjoystickfun cbfun)
	//GLFWjoystickfun result = glfwSetJoystickCallback(cbfun);
	return nullptr;
}


// with these functions we can cache a userdata pointer in the monitor
// might be useful for a closer binding, but for now not needed

napi_value SetMonitorUserPointer(napi_env env, napi_callback_info info) {
	return NULL;
}

napi_value GetMonitorUserPointer(napi_env env, napi_callback_info info) {
	return nullptr ;
}


// disabled from JS interface, as we need it for the module
napi_value SetWindowUserPointer(napi_env env, napi_callback_info info) {
	return nullptr;
}
napi_value GetWindowUserPointer(napi_env env, napi_callback_info info) {
	return nullptr;
}

/*
Don't know how to deal with this yet
It is called when monitors are added or removed
There would need to be some kind of reference cached globally to retrieve the napi_env
in order to call a js-level function
*/
// void (* GLFWmonitorfun)(GLFWmonitor*,int);
napi_value SetMonitorCallback(napi_env env, napi_callback_info info) {
	return nullptr;
}

// TODO: Vulkan stuff
napi_value VulkanSupported(napi_env env, napi_callback_info info) {
	return nullptr;
}

napi_value GetInstanceProcAddress(napi_env env, napi_callback_info info) {
	return nullptr;
}

napi_value GetPhysicalDevicePresentationSupport(napi_env env, napi_callback_info info) {
	return nullptr;
}

napi_value CreateWindowSurface(napi_env env, napi_callback_info info) {
	return nullptr;
}


napi_value CreateCursor(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLFWimage* image;
	int xhot = getInt32(env, args[1]);
	int yhot = getInt32(env, args[2]);
	// GLFWcursor* glfwCreateCursor(const GLFWimage* image, int xhot, int yhot)
	GLFWcursor* result = glfwCreateCursor(image, xhot, yhot);
	return nullptr;
}

napi_value CreateStandardCursor(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	int shape = getInt32(env, args[0]);
	// GLFWcursor* glfwCreateStandardCursor(int shape)
	GLFWcursor* result = glfwCreateStandardCursor(shape);
	return nullptr;
}

napi_value DestroyCursor(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLFWcursor* cursor;
	// void glfwDestroyCursor(GLFWcursor* cursor)
	glfwDestroyCursor(cursor);
	return NULL;
}

napi_value SetCursor(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLFWwindow* window = nullptr;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;
	GLFWcursor* cursor;
	// void glfwSetCursor(GLFWwindow* window, GLFWcursor* cursor)
	glfwSetCursor(window, cursor);
	return NULL;
}


napi_value GetProcAddress(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	char* procname = nullptr;
	status = getCharacterArray(env, args[0], procname);
	// GLFWglproc glfwGetProcAddress(const char* procname)
	GLFWglproc result = glfwGetProcAddress(procname);
	return nullptr;
}
