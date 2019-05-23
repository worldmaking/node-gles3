#ifndef AL_GLFW_H
#define AL_GLFW_H

#include "al_platform.h"
#include "al_gl.h"

/*
	Windows, contexts, threads
	Docs say:
	- a glfw window has a context (inseparably linked)
	- a new window can share context with an earlier one (last arg of glfwCreateWindow)
	- shared contexts allow use of same texture, shader etc. in both windows
		- BUT apparently VAOs and FBOs are not shared?? CHeck this!

	- BUT different threads MUST have different contexts
	- a thread can ony have one GL context bound at a time
	- a context should only be 'current' on one window & one thread at a time

	-  a possible benefit of seconardy contexts is parallel data upload
	
	- so: each windows has a context, which could be shared with other windows, so long as they are on the same thread.
	
	- some GLFW calls can only be made on the primary thread, incl. pollevents

	- the context of a window must be made 'current' to use GL commands, via glfwMakeContextCurrent(window). glfwGetCurrentContext() returns the current window
	- in this single-thread, multi-window case:
		- make contex current for win1, draw, make context current for win2, draw, swapbufs on win1, swapbufs on win2, then pollevents
	- can multithread this a little, but context can only be active on one thread at a time, so there's not much point.

	Fullscreen
	- going fullscreen means creating a new window, and destroying the old
	- OR, use  glfwSetWindowMonitor to convert a window FS/noFS	
		- but most of the code I see online doesn't do this
	- see "Windowed full screen" for how to do this without changing video modes (http://www.glfw.org/docs/latest/window_guide.html#window_full_screen)
	- but there's a bug in current glfw that is fixed on master, would need a rebuild of the libs.
*/

#define _GLFW_USE_DWM_SWAP_INTERVAL 1
#include <GLFW/glfw3.h>

#include "al_console.h"

struct Window {

	struct Manager {

		Manager() {
			glfwInit();
			glfwSetErrorCallback(glfw_error_callback);

			detect_monitors();
		}

		~Manager() {
			// Terminates GLFW, clearing any resources allocated by GLFW.
			glfwTerminate();
		}

		void detect_monitors() {
			monitors = glfwGetMonitors(&monitor_count);
			console.log("%d monitor(s)", monitor_count);
		}
	
		static void glfw_error_callback(int err, const char* description) {
			console.error("GL Error (%d): %s", err, description);
		}

		int monitor_count = 0;
		GLFWmonitor** monitors = NULL;

		Window * firstWindow = NULL;
	};

	int width = 1920/2;
	int height = 1280/2;
	int x, y; // position
	int windowed_width, windowed_height; 
	GLFWwindow* pointer = 0; 
	GLFWmonitor * monitor = 0;
	bool isFullScreen = 0;
	
	Window() {
		
	}
	
	bool open() {
		if (pointer) return true;
		
		// GL context options:
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#ifdef AL_OSX
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
	#endif

		monitor = glfwGetPrimaryMonitor();
		
		// OS window options:
		if (0) {
			const GLFWvidmode * lastMonitorMode = glfwGetVideoMode(monitor);
			width = lastMonitorMode->width;
			height = lastMonitorMode->height;	

			
			glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
			glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
			glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);
			pointer = glfwCreateWindow(width, height, "alice", monitor, NULL);
			isFullScreen = 1;
		} else {

			glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
			glfwWindowHint(GLFW_MAXIMIZED, GL_FALSE);
			glfwWindowHint(GLFW_AUTO_ICONIFY, GL_TRUE);
			glfwWindowHint(GLFW_DECORATED, GL_TRUE);

			glfwWindowHint(GLFW_FOCUSED, GL_FALSE); // don't grab focus, it's annoying
			pointer = glfwCreateWindow(width, height, "alice", NULL, NULL);
		}
		
		// try to create it:
		
		if (pointer == NULL) {
			console.error("Failed to create GLFW window");
			return false;
		}
	
		glfwGetWindowPos(pointer, &x, &y);

		// now initialize:
		glfwMakeContextCurrent(pointer);
		glfwSetWindowUserPointer(pointer, this);	
		glfwSwapInterval(0); // turn off vsync	
		glfwGetFramebufferSize(pointer, &width, &height);
		glfw_framebuffer_size_callback(pointer, width, height);
		glfwSetFramebufferSizeCallback(pointer, glfw_framebuffer_size_callback);
	
		int version = gladLoadGLSimple((GLADsimpleloadproc) glfwGetProcAddress);
		if (version == 0) {
			console.error("Failed to initialize OpenGL context");
			return false;
		} else {
			console.log("Loaded OpenGL %d.%d\n", version / 10, version % 10);
		}

	
		// initialize OpenGL:
		
		/*
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			console.error("Failed to initialize OpenGL context");
			return false;
		} else if (GLVersion.major < 2) {
			console.error("Your system doesn't support OpenGL >= 2!");
			return false;
		}
		*/
		//console.log("GLAD initialized OpenGL %d.%d", GLVersion.major, GLVersion.minor);
		console.log("OpenGL %s, GLSL %s", glGetString(GL_VERSION),
			glGetString(GL_SHADING_LANGUAGE_VERSION));

		// TODO: set up user-level GL resources
		
		return true;
	}
	
	void close() {
		if (!pointer) return;		
		glfwDestroyWindow(pointer);
		// TODO: clean up user-level GL resources
		pointer = NULL;
	}

	void toggleFullScreen() { fullScreen(!isFullScreen); }

	void fullScreen(bool fs=true) {
		if (fs != isFullScreen) {
			if (fs) {
				glfwGetWindowPos(pointer, &x, &y);
				glfwGetWindowSize(pointer, &windowed_width, &windowed_height);
				
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);

				console.log("enter fs for %p at %d x %d @%dfps", monitor, mode->width, mode->height, mode->refreshRate);
				glfwSetWindowMonitor(pointer, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
				isFullScreen = true;
			} else {
				console.log("leave fs to %d x %d", windowed_width, windowed_height);
				glfwSetWindowMonitor(pointer, NULL, x, y, windowed_width, windowed_height, GLFW_DONT_CARE);
				isFullScreen = false;
			}
		}
	}

	void position(int x, int y) {
		this->x = x;
		this->y = y;
		glfwSetWindowPos(pointer, x, y);
	}
	
	static void glfw_framebuffer_size_callback(GLFWwindow * pointer, int w, int h) {
		console.log("New framebuffer resolution %dx%d", w, h); 
		auto self = (Window *)glfwGetWindowUserPointer(pointer);
		self->width = w;
		self->height = h;
	}
	
};

#endif // AL_GLFW_H
