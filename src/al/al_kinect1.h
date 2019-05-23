#ifndef AL_KIENCT1_H
#define AL_KIENCT1_H

#include "al_glm.h"
#include "al_xyzrgb.h"
#include "arrays.h"
#include "support.h"

#define KINECT_DEPTH_WIDTH 640
#define KINECT_DEPTH_HEIGHT 480

#ifdef  _MSC_VER

//#include "Windows.h"
#include <combaseapi.h>
#include "NuiApi.h"
#include <Ole2.h>
#include <algorithm>
#include <new>
typedef OLECHAR* WinStr;

#pragma pack(push, 1)
struct BGRA {
	unsigned char b, g, r, a;
};
struct ARGB {
	unsigned char a, r, g, b;
};
struct RGB {
	unsigned char r, g, b;
};
#pragma pack(pop)

struct Kinect1 {
	
	XYZRGB cloud;
	
	// Current Kinect
	INuiSensor* device;
	WinStr name;
	HANDLE colorStreamHandle;
	HANDLE depthStreamHandle;
	NUI_SKELETON_FRAME skeleton_back;
	std::string serial;
	//t_systhread capture_thread;
	//t_systhread_mutex depth_mutex;
	int capturing;
	int device_count;

	// calibration
	int hasColorMap;
	long* colorCoordinates;
	uint16_t* mappedDepthTmp;
	uint16_t* unmappedDepthTmp;

	glm::mat4 cloudTransform;

	int new_depth_data, new_rgb_data;
	int usecolor;
	int timeout;

	Array2DSized < glm::vec3, KINECT_DEPTH_WIDTH, KINECT_DEPTH_HEIGHT> rgb;
	Array2DSized < uint32_t, KINECT_DEPTH_WIDTH, KINECT_DEPTH_HEIGHT> depth_mat;
	Array2DSized < glm::vec3, KINECT_DEPTH_WIDTH, KINECT_DEPTH_HEIGHT> cloud_mat;

	Kinect1() {
		usecolor = 1;
		/*
		unique = 1;
		usecolor = 1;
		uselock = 1;
		align_depth_to_color = 1;
		player = 0;
		skeleton = 0;
		skeleton_smoothing = 1;
		seated = 0;
		near_mode = 0;
		*/

		timeout = 30;
		
		device = 0;
		colorStreamHandle = 0;
		capturing = 0;
		//systhread_mutex_new(&depth_mutex, 0);


		new_depth_data = 0;
		new_rgb_data = 0;
		hasColorMap = 0;

		cloudTransform = glm::mat4(1.f);

		HRESULT result = NuiGetSensorCount(&device_count);
		if (result != S_OK) console_error("Kinect: failed to get sensor count");
		else console_log("Kinect: %d devices", device_count);


		colorCoordinates = new long[KINECT_DEPTH_WIDTH*KINECT_DEPTH_HEIGHT * 2];
		mappedDepthTmp = new uint16_t[KINECT_DEPTH_WIDTH*KINECT_DEPTH_HEIGHT];
		unmappedDepthTmp = new uint16_t[KINECT_DEPTH_WIDTH*KINECT_DEPTH_HEIGHT];
	}

	~Kinect1() {
		close();
	}

	void close() {
		if (device) {
			device->NuiShutdown();
			device->Release();
			device = 0;
		}
	}


	glm::vec3 realWorldToDepth(const glm::vec3& p) {
		const Vector4 v = { p.x, p.y, p.z, 1.f };
		LONG x = 0;
		LONG y = 0;
		USHORT d = 0;
		NuiTransformSkeletonToDepthImage(v, &x, &y, &d, NUI_IMAGE_RESOLUTION_640x480);
		d >>= 3;
		return glm::vec3(x, y, d * 0.001f);
	}

	glm::vec3 depthToRealWorld(const glm::vec3& p) {
		Vector4 v = NuiTransformDepthImageToSkeleton(
			LONG(p.x),
			LONG(p.y),
			USHORT(p.z),
			NUI_IMAGE_RESOLUTION_640x480
			);

		return glm::vec3(v.x, v.y, v.z);
	}

	void open(std::string name, int index = 0) {
		if (device) {
			console_log("device already opened");
			return;
		}
		INuiSensor* dev;
		HRESULT result = 0;
		OLECHAR instanceName[100];
		const char * s = name.c_str();
		mbstowcs(instanceName, s, strlen(s) + 1);
		result = NuiCreateSensorById(instanceName, &dev);
		if (result != S_OK) {
			console_error("Kinect couldn't open by ID: %s", s);
			open(index);
		}
		else {
			open(dev);
		}
	}

	void open(int index = 0) {

		if (device) {
			console_log("device already opened");
			return;
		}

		INuiSensor* dev;
		HRESULT result = 0;

		result = NuiCreateSensorByIndex(index, &dev);

		if (result != S_OK) {
			if (E_NUI_DEVICE_IN_USE == result) {
				console_error("Kinect for Windows already in use.");
			}
			else if (E_NUI_NOTGENUINE == result) {
				console_error("Kinect for Windows is not genuine.");
			}
			else if (E_NUI_INSUFFICIENTBANDWIDTH == result) {
				console_error("Insufficient bandwidth.");
			}
			else if (E_NUI_NOTSUPPORTED == result) {
				console_error("Kinect for Windows device not supported.");
			}
			else if (E_NUI_NOTCONNECTED == result) {
				console_error("Kinect for Windows is not connected.");
			}
			else if (E_NUI_NOTREADY == result) {
				console_error("Kinect for Windows is not ready.");
			}
			else if (E_NUI_NOTPOWERED == result) {
				console_error("Kinect for Windows is not powered.");
			}
			else if (E_NUI_DATABASE_NOT_FOUND == result) {
				console_error("Kinect for Windows database not found.");
			}
			else if (E_NUI_DATABASE_VERSION_MISMATCH == result) {
				console_error("Kinect for Windows database version mismatch.");
			}
			else {
				console_error("Kinect for Windows could not initialize.");
			}
			return;
		}
		open(dev);
	}
	void open(INuiSensor* dev) {
		HRESULT result = dev->NuiStatus();
		switch (result) {
		case S_OK:
			break;
		case S_NUI_INITIALIZING:
			console_log("the device is connected, but still initializing"); return;
		case E_NUI_NOTCONNECTED:
			console_error("the device is not connected"); return;
		case E_NUI_NOTGENUINE:
			console_log("the device is not a valid kinect"); break;
		case E_NUI_NOTSUPPORTED:
			console_log("the device is not a supported model"); break;
		case E_NUI_INSUFFICIENTBANDWIDTH:
			console_error("the device is connected to a hub without the necessary bandwidth requirements."); return;
		case E_NUI_NOTPOWERED:
			console_log("the device is connected, but unpowered."); return;
		default:
			console_log("the device has some unspecified error"); return;
		}

		WinStr wstr = dev->NuiDeviceConnectionId();
		std::mbstate_t state = std::mbstate_t();
		int len = 1 + std::wcsrtombs((char *)nullptr, (const wchar_t **)&wstr, 0, &state);
		char outname[128];
		std::wcsrtombs(outname, (const wchar_t **)&wstr, len, &state);
		serial = (outname);

		device = dev;
		console_log("Kinect: init device %s", outname);
		
		hasColorMap = 0;
		long priority = 10; // maybe increase?
		
	}

	// must be called from run thread:
	void start() {
		HRESULT result = 0;
		DWORD dwImageFrameFlags;
		DWORD initFlags = 0;

		initFlags = 0;
		if (usecolor) initFlags |= NUI_INITIALIZE_FLAG_USES_COLOR;
		/*if (player) {
			initFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX;
		}
		else {*/
			initFlags |= NUI_INITIALIZE_FLAG_USES_DEPTH;
		/*		if (skeleton) {
			initFlags |= NUI_INITIALIZE_FLAG_USES_SKELETON;
		}
		if (audio) {
			initFlags |= NUI_INITIALIZE_FLAG_USES_AUDIO;
		}
		if (high_quality_color) {
			initFlags |= NUI_INITIALIZE_FLAG_USES_HIGH_QUALITY_COLOR;
		}*/


		result = device->NuiInitialize(initFlags);

		if (result != S_OK) {
			console_error("failed to initialize sensor");
			goto done;
		}
		/*
		if (skeleton) {
			if (seated) {
				NuiSkeletonTrackingEnable(NULL, NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT);
			}
			else {
				NuiSkeletonTrackingEnable(NULL, 0);
			}
		}
		*/
		console_log("Kinect device initialized");

		if (usecolor) {
			dwImageFrameFlags = 0;
			//if (near_mode) dwImageFrameFlags |= NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE;
			result = device->NuiImageStreamOpen(
				NUI_IMAGE_TYPE_COLOR, //NUI_IMAGE_TYPE eImageType,
				NUI_IMAGE_RESOLUTION_640x480, // NUI_IMAGE_RESOLUTION eResolution,
				dwImageFrameFlags,
				2, //DWORD dwFrameLimit,
				0,
				&colorStreamHandle);
			if (result != S_OK) {
				console_error("failed to open color stream");
				goto done;
			}
			console_log("opened color stream");
		}

		dwImageFrameFlags = 0;
		dwImageFrameFlags |= NUI_IMAGE_STREAM_FLAG_DISTINCT_OVERFLOW_DEPTH_VALUES;
		//if (near_mode) dwImageFrameFlags |= NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE;
		NUI_IMAGE_TYPE eImageType = NUI_IMAGE_TYPE_DEPTH;
		/*if (player) {
			eImageType = NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX;
		}*/
		result = device->NuiImageStreamOpen(
			eImageType,
			NUI_IMAGE_RESOLUTION_640x480, // NUI_IMAGE_RESOLUTION eResolution,
			dwImageFrameFlags,
			2, //DWORD dwFrameLimit,
			0,
			&depthStreamHandle);
		if (result != S_OK) {
			console_error("failed to open depth stream");
			goto done;
		}
		console_log("opened depth stream");

		//estimateCalibration();
		return;

	done:
		close();
	}

	// call in run thread:
	void step() {
		if (usecolor) processColor();
		processDepth();
		//if (skeleton) pollSkeleton();
	}


	void processColor() {
		if (!device) return;
		DWORD dwMillisecondsToWait = timeout;
		NUI_IMAGE_FRAME imageFrame;

		HRESULT result = device->NuiImageStreamGetNextFrame(colorStreamHandle, dwMillisecondsToWait, &imageFrame);
		if (result == E_NUI_FRAME_NO_DATA) {
			// timeout with no data. bail or continue?
			return;
		}
		else if (FAILED(result)) {
			switch (result) {
				case E_INVALIDARG:
					console_error("arg stream error"); break;
				case E_OUTOFMEMORY:
					console_error("Ran out of memory"); break;
				case E_NOINTERFACE:
					console_error("unsupported"); break;
				case E_ABORT:
					console_error("Operation aborted"); break;
				case E_ACCESSDENIED:
					console_error("General access denied error"); break;
				case E_POINTER:
					console_error("pointer stream error"); break;
				case E_HANDLE:
					console_error("invalid handle"); break;
				case E_PENDING:
					console_error("The data necessary to complete this operation is not yet available."); break;
				case S_FALSE:
					console_error("timeout"); break;
				default:
					console_error("stream error"); break;
			}
				return;
		}

		int newframe = 0;

		// got data; now turn it into jitter 
		//console_log("frame %d", imageFrame.dwFrameNumber);
		//outlet_int(outlet_msg, imageFrame.dwFrameNumber);
		INuiFrameTexture * imageTexture = imageFrame.pFrameTexture;
		NUI_LOCKED_RECT LockedRect;

		// Lock the frame data so the Kinect knows not to modify it while we're reading it
		imageTexture->LockRect(0, &LockedRect, NULL, 0);

		// Make sure we've received valid data
		if (LockedRect.Pitch != 0) {
			//console_log("pitch %d size %d", LockedRect.Pitch, LockedRect.size);
			//static_cast<BYTE *>(LockedRect.pBits), LockedRect.size

			//sysmem_copyptr(LockedRect.pBits, rgb_back, LockedRect.size);

			// convert to Jitter-friendly RGB layout:


			const BGRA * src = (const BGRA *)LockedRect.pBits;
			glm::vec3 * dst = rgb.data;
			int cells = KINECT_DEPTH_HEIGHT * KINECT_DEPTH_WIDTH;
			/*
			if (align_depth_to_color) {
				for (int i = 0; i < cells; ++i) {
					dst[i].r = src[i].r;
					dst[i].g = src[i].g;
					dst[i].b = src[i].b;
				}
			}
			else {*/
				// align color to depth:
				//std::fill(dst, dst + cells, RGB(0, 0, 0));
				for (int i = 0; i < cells; ++i) {
					int c = colorCoordinates[i * 2];
					int r = colorCoordinates[i * 2 + 1];
					if (c >= 0 && c < KINECT_DEPTH_WIDTH
						&& r >= 0 && r < KINECT_DEPTH_HEIGHT) {
						// valid location: depth value:
						int idx = r*KINECT_DEPTH_WIDTH + c;
						dst[i].r = src[idx].r / 255.;
						dst[i].g = src[idx].g / 255.;
						dst[i].b = src[idx].b / 255.;
					}
				}
			//}
			newframe = 1;
		}

		// We're done with the texture so unlock it
		imageTexture->UnlockRect(0);

		//	ReleaseFrame:
		// Release the frame
		device->NuiImageStreamReleaseFrame(colorStreamHandle, &imageFrame);

		//if (newframe) cloud_rgb_process();

		new_rgb_data = 1;
	}


	void processDepth() {
		if (!device) return;
		DWORD dwMillisecondsToWait = timeout;
		NUI_IMAGE_FRAME imageFrame;

		HRESULT result = device->NuiImageStreamGetNextFrame(depthStreamHandle, dwMillisecondsToWait, &imageFrame);
		if (result == E_NUI_FRAME_NO_DATA) {
			// timeout with no data. bail or continue?
			Sleep(30);
			return;
		}
		else if (FAILED(result)) {
			switch (result) {
			case E_INVALIDARG:
				console_error("arg stream error"); break;
			case E_OUTOFMEMORY:
				console_error("Ran out of memory"); break;
			case E_NOINTERFACE:
				console_error("unsupported"); break;
			case E_ABORT:
				console_error("Operation aborted"); break;
			case E_ACCESSDENIED:
				console_error("General access denied error"); break;
			case E_POINTER:
				console_error("pointer stream error"); break;
			case E_HANDLE:
				console_error("invalid handle"); break;
			case E_PENDING:
				console_error("The data necessary to complete this operation is not yet available."); break;
			case S_FALSE:
				console_error("timeout"); break;

			default:
				console_error("stream error %x"); break;
			}
			Sleep(30);
			return;
		}

		INuiFrameTexture * imageTexture = NULL;
		BOOL bNearMode = false; // near_mode;

		result = device->NuiImageFrameGetDepthImagePixelFrameTexture(depthStreamHandle, &imageFrame, &bNearMode, &imageTexture);
		//imageTexture = imageFrame.pFrameTexture;

		// got data; now turn it into jitter 
		if (!imageTexture) {
			console_log("no data");
			goto ReleaseFrame;
		}
		NUI_LOCKED_RECT LockedRect;

		// Lock the frame data so the Kinect knows not to modify it while we're reading it
		imageTexture->LockRect(0, &LockedRect, NULL, 0);

		// Make sure we've received valid data
		if (LockedRect.Pitch != 0) {

			// convert to Jitter-friendly RGB layout:
			//const uint16_t * src = (const uint16_t *)LockedRect.pBits;
			NUI_DEPTH_IMAGE_PIXEL * src = reinterpret_cast<NUI_DEPTH_IMAGE_PIXEL*>(LockedRect.pBits);
			uint32_t * dst = depth_mat.data;
			//char * dstp = player_mat.back;
			static const int cells = KINECT_DEPTH_HEIGHT * KINECT_DEPTH_WIDTH;

			// First generate packed depth values from extended depth values, which include near pixels.
			for (int i = 0; i<cells; i++) {
				unmappedDepthTmp[i] = src[i].depth << NUI_IMAGE_PLAYER_INDEX_SHIFT;
			}

			if (!hasColorMap) {
				// use it to generate the color map:
				device->NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution(
					NUI_IMAGE_RESOLUTION_640x480, //colorResolution,
					NUI_IMAGE_RESOLUTION_640x480, //depthResolution,
					cells,
					unmappedDepthTmp, // depth_d16
					cells * 2,
					colorCoordinates
					);
				//console_log("generated color map");
				hasColorMap = 1;
			}

			//if (uselock) systhread_mutex_lock(depth_mutex);

			glm::vec2 dim(KINECT_DEPTH_WIDTH, KINECT_DEPTH_HEIGHT - 1);
			glm::vec2 inv_dim_1 = 1.f / (dim - 1.f);

			/*if (align_depth_to_color)
			{
				//QWriteLocker locker(&that->m_lock);
				//uint16_t* depth_buf = that->m_current_image.rawDepth16bitsRef().ptr<uint16_t>();
				//mapDepthFrameToRgbFrame(src, depth_buf);

				// clean the output buffers:
				std::fill(dst, dst + cells, 0);
				std::fill(dstp, dstp + cells, 0);
				//memset(cloud_mat.back, 0, sizeof(vec3) * cells);

				float inv_rgb_focal_x = 1.f / rgb_focal.x;
				float inv_rgb_focal_y = 1.f / rgb_focal.y;
				vec2 inv_rgb_focal = 1.f / rgb_focal;

				// read through all the color coordinates
				for (int i = 0; i < cells; ++i) {
					// get the corresponding RGB image coordinate for depth pixel i:
					int c = colorCoordinates[i * 2];
					int r = colorCoordinates[i * 2 + 1];
					// idx is for rgb image:
					int idx = r*KINECT_DEPTH_WIDTH + c;

					if (c >= 0 && c < KINECT_DEPTH_WIDTH
						&& r >= 0 && r < KINECT_DEPTH_HEIGHT) {
						// valid location: get the depth value:			
						uint16_t depth_in_mm = src[i].depth;
						// the output depth/cloud images will be aligned to the rgb image:
						vec3& out = cloud_mat.back[idx];
						if (depth_in_mm > 0) {
							// set the output depth value:
							dst[idx] = depth_in_mm;
							//Player index
							dstp[idx] = (char)src[i].playerIndex;

							// derive the output 3D position
							//out = depthToRealWorld(vec3(c, r, depth_in_mm << 3));

							// first, unproject from pixel plane to idealized focal plane (in meters):
							vec2 xy = (vec2(c, r) - rgb_center) * inv_rgb_focal;
							//float xf = (c - rgb_center.x) * inv_rgb_focal_x;
							//float yf = (r - rgb_center.y) * inv_rgb_focal_y;
							float zf = depth_in_mm * 0.001f;

							// TODO: remove RGB lens distortion effects? (rgb_radial, rgb_tangential)
							// for this we would need an undistortion map to be generated
							// apply rectify:
							xy += rectify_mat.sample(xy);

							// project into 3D space by depth:
							vec3 v(xy.x * zf, -xy.y * zf, zf);

							// apply extrinsic:
							out = cloudTransform(v);

						}
						else {
							// invalid depth: fill zero:
							dst[idx] = 0;
							vec3 v(0, 0, 0);
							out = cloudTransform(v);
						}
					}

				}

				/*
				for (int y=0; y<ydim; y++) {
				for (int x=0; x<xdim; x++) {
				int idx1 = x + y*xdim;

				float m = world.mask[idx1];
				land_prev[idx1] *= m;
				}
				}
			}
			else
			{*/

				/*QWriteLocker locker(&that->m_lock);
				uint16_t* depth_buf = that->m_current_image.rawDepth16bitsRef().ptr<uint16_t>();
				cv::Vec2w* depth_to_color_coords = that->m_current_image.depthToRgbCoordsRef().ptr<cv::Vec2w>();
				extractDepthAndColorCoords (src, depth_buf, depth_to_color_coords);
				*/

				// write cells into depth and cloud matrices:
				for (int i = 0, y = 0; y<KINECT_DEPTH_HEIGHT; y++) {
					for (int x = 0; x<KINECT_DEPTH_WIDTH; x++, i++) {
						uint32_t d = src[i].depth;
						dst[i] = d;
						//dstp[i] = (char)src[i].playerIndex;
						glm::vec3 v(0, 0, 0);

						if (d > 0) {
							v = depthToRealWorld(glm::vec3(x, y, d << 3));
						}
						v = mat4_transform(cloudTransform, v);
						v *= glm::vec3(1.f, -1.f, -1.f);
						cloud_mat.data[i] = v;
					}
				}

				for (int i = 0, y = 0; y<KINECT_DEPTH_HEIGHT; y++) {
					for (int x = 0; x<KINECT_DEPTH_WIDTH; x++, i++) {
						if (dst[i] > 0) {
							glm::vec3 out = cloud_mat.data[i];
						}
					}
				}

			//}

			//if (uselock) systhread_mutex_unlock(depth_mutex);
			new_depth_data = 1;
		}


		// We're done with the texture so unlock it
		imageTexture->UnlockRect(0);

		//cloud_process();
		//local_cloud_process();

	ReleaseFrame:
		// Release the frame
		device->NuiImageStreamReleaseFrame(depthStreamHandle, &imageFrame);

	}
};

#else 

// TODO: fake on OSX

struct Kinect1 {

	Array2DSized < glm::vec3, KINECT_DEPTH_WIDTH, KINECT_DEPTH_HEIGHT> cloud_mat;

};

#endif

#endif
