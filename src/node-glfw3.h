#include "node-api-helpers.h"
#include <GLFW/glfw3.h>

///////////////////////////////////////////////////////////////////////////
// now the GLFW bindings:
///////////////////////////////////////////////////////////////////////////



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


napi_value GetError(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// int glfwGetError(const char** description)
	int result = glfwGetError(nullptr);
	napi_value result_value;
	status = napi_create_int32(env, (int32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetErrorString(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	const char* description;
	// int glfwGetError(const char** description)
	int result = glfwGetError(&description);
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


napi_value GetMonitorContentScale(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);

	GLFWmonitor* monitor;
	napi_valuetype monitor_type;
	status = napi_typeof(env, args[0], &monitor_type);
	if (status != napi_ok || monitor_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&monitor);
	if (status != napi_ok) return nullptr;

	float x, y;
	// void glfwGetMonitorPos(GLFWmonitor* monitor, int* xpos, int* ypos)
	glfwGetMonitorContentScale(monitor, &x, &y);

	// return as array:
	napi_value result = nullptr;
	status = napi_create_array_with_length(env, 2, &result);
	if (status == napi_ok) {
		napi_value nx, ny;
		napi_create_double(env, x, &nx);
		napi_set_element(env, result, 0, nx);
		napi_create_double(env, y, &ny);
		napi_set_element(env, result, 1, ny);
	}
	return result;
}


#define GLFW_WINDOWSTATE_MAGIC 'glwn'

struct WindowState {
	uint32_t magic = GLFW_WINDOWSTATE_MAGIC;
	napi_env env;
	napi_ref windowRef;

	napi_ref onWindowPos = nullptr;
};

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


napi_value GetWindowContentScale(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);

	GLFWwindow* window;
	napi_valuetype window_type;
	status = napi_typeof(env, args[0], &window_type);
	if (status != napi_ok || window_type != napi_external) return nullptr;
	status = napi_get_value_external(env, args[0], (void **)&window);
	if (status != napi_ok) return nullptr;

	float x, y;
	glfwGetWindowContentScale(window, &x, &y);

	// return as array:
	napi_value result = nullptr;
	status = napi_create_array_with_length(env, 2, &result);
	if (status == napi_ok) {
			napi_value nx, ny;
			napi_create_double(env, x, &nx);
			napi_set_element(env, result, 0, nx);
			napi_create_double(env, y, &ny);
			napi_set_element(env, result, 1, ny);
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
	// get the userdata for this window:
	WindowState * data = (WindowState *)glfwGetWindowUserPointer(window);
	if (data->magic != GLFW_WINDOWSTATE_MAGIC || data->onWindowPos == nullptr) return;

	// retrieve the handler:
	napi_value callback; 
	napi_valuetype callback_type;
	if (napi_ok != napi_get_reference_value(data->env, data->onWindowPos, &callback)
	|| napi_ok != napi_typeof(data->env, callback, &callback_type)
	|| callback_type != napi_function) return;

	// call it: 
	int argc = 3;
	napi_value argv[3];
	napi_get_reference_value(data->env, data->windowRef, &argv[0]);
	napi_create_int32(data->env, x, &argv[1]);
	napi_create_int32(data->env, y, &argv[2]);

	napi_value result;
	napi_call_function(data->env, nullptr, callback, argc, argv, &result);
}

napi_value SetWindowPosCallback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);

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
	if (data->onWindowPos) napi_delete_reference(env, data->onWindowPos);
	data->onWindowPos = nullptr;
	// 2nd arg must be a callable function
	napi_value handler = args[1];
	napi_valuetype handler_type;
	status = napi_typeof(env, args[0], &handler_type);
	if (status == napi_ok && handler_type == napi_function) {
		// install new one:
		napi_create_reference(env, handler, 1, &data->onWindowPos);
	}

	// install the onWindowPos handler:
	// GLFWwindowposfun glfwSetWindowPosCallback(GLFWwindow* window, GLFWwindowposfun cbfun)
	glfwSetWindowPosCallback(window, windowposfun);

	return args[0];
}


//void Errorfun(int code, const char* msg) {}
// since there's no userdata here, the only way we could get this to work is to 
// stash a singleton user handler in the napi_env itself somehow
// I'm not sure how to do that yet.
napi_value glfwSetErrorCallback(napi_env env, napi_callback_info info) {
	return nullptr ;
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
