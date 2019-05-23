#ifndef AL_KINECT2_H
#define AL_KINECT2_H

#include "al_math.h"
#include "al_thread.h"
#include "al_time.h"
#include "al_platform.h"

#include <vector>
#include <algorithm>
#include <signal.h>

#define KINECT_FRAME_BUFFERS 4
#define KINECT_MAX_DEVICES 2



#ifdef AL_WIN
#define AL_USE_FREENECT2_SDK 1
#ifdef AL_USE_FREENECT2_SDK
#define LIBFREENECT2_WITH_CUDA_SUPPORT

#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>

#else 
#include <Ole2.h>
typedef OLECHAR* WinStr;
#define AL_USE_KINECT2_SDK 1
#include "kinect.h"

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

#endif
#endif

/*
    We're going to want a cloud source
    this could be from a device stream, or from a disk stream

	one way of doing this is to mmap a hunk of space for streaming the frames into, as a kind of ringbuffer

*/

struct CloudPoint {
    glm::vec3 location;
    glm::vec3 color;
    glm::vec2 texCoord; // the location within the original image
};

struct ColorPoint {
	uint8_t b, g, r;
};

typedef uint16_t DepthPoint;

static const int cDepthWidth = 512;
static const int cDepthHeight = 424;
static const int cColorWidth = 1920;
static const int cColorHeight = 1080;

/*
	Colour and depth frames are stored separately, as they can be received at different rates
	And moreover, the depth data tends to come in at a lower latency than colour (by 0-3 frames)

	Maybe it would be nice to provide an interface to synchronize them, particularly when recording data to disk
	Even for realtime, sometimes it would be preferable to admit more latency to ensure depth/image data correspond
	(That means, picking an older depth frame to match the closest colour frame timestamp)
*/

struct ColourFrame {
	// RGB colour image at full resolution
	ColorPoint color[cColorWidth*cColorHeight];

	int64_t timeStamp;
};

struct CloudFrame {
	// depth data, in mm. bad points will be marked with zeroes.
	uint16_t depth[cDepthWidth*cDepthHeight];
	// depth points mapped to 3D space
	// coordinate system: right-handed (like common OpenGL use)
	// scale: meters
	// origin: the Kinect IR sensor
	// orientation: +Z in front of kinect, +Y above it, and +X away from the RGB camera
	// i.e. RHS oriented looking toward the camera.
	// to re-orient the space from the camera POV, flip sign on X and Z axes. 
	glm::vec3 xyz[cDepthWidth*cDepthHeight];
	// uv texture coordinates for each depth point to pick from the color image 
	glm::vec2 uv[cDepthWidth*cDepthHeight]; 
	// color for this point
	glm::vec3 rgb[cDepthWidth*cDepthHeight];

	//uint64_t pointCount; 
	int64_t timeStamp;
};

struct CloudDevice {
    int use_colour = 1;
	int use_uv = 1;
    int capturing = 0;

	std::string serial;
	int id = 0;

	glm::mat4 cloudTransform = glm::mat4(1.);

	std::thread kinect_thread;
	std::vector<CloudFrame> cloudFrames = std::vector<CloudFrame>(KINECT_FRAME_BUFFERS);
	std::vector<ColourFrame> colourFrames = std::vector<ColourFrame>(KINECT_FRAME_BUFFERS);
	int lastCloudFrame = 0;
	int lastColourFrame = 0;

	FILE * recordFD;
	
	// the most recently completed frame:
	const CloudFrame& cloudFrame() const {
		return cloudFrames[lastCloudFrame];
	}
	// the most recently completed frame:
	const ColourFrame& colourFrame() const {
		return colourFrames[lastColourFrame];
	}

#ifdef AL_USE_FREENECT2_SDK

	struct ColourPacket {
		uint8_t r, g, b, x;
	};

	libfreenect2::Freenect2Device * dev = 0;
	libfreenect2::PacketPipeline *pipeline = 0;

	FPS fps;

 	int kinect_thread_fun() {

		printf("hello from freenect thread for device %s\n", dev->getSerialNumber().c_str());
		libfreenect2::Freenect2Device::Config config;
		config.MinDepth = 0.5f;
		config.MaxDepth = 6.f;
		// Remove pixels on edges because ToF cameras produce noisy edges.
		config.EnableEdgeAwareFilter = true;
		///< Remove some "flying pixels".
		config.EnableBilateralFilter = true;
		dev->setConfiguration(config);

		int types = libfreenect2::Frame::Ir 
				  | libfreenect2::Frame::Depth;
		if (use_colour) types |= libfreenect2::Frame::Color;
		libfreenect2::SyncMultiFrameListener listener(types);
		libfreenect2::FrameMap frames;
		dev->setColorFrameListener(&listener);
		dev->setIrAndDepthFrameListener(&listener);

		if (use_colour) {
			if (!dev->start()) return -1;
		} else {
			if (!dev->startStreams(use_colour, true)) return -1;
		}

		libfreenect2::Registration* registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
		libfreenect2::Frame undistorted(cDepthWidth, cDepthHeight, 4);
		libfreenect2::Frame  registered(cDepthWidth, cDepthHeight, 4);

		printf("freenect ready\n");

		size_t framecount = 0;
		while(capturing) {
		
			if (!listener.waitForNewFrame(frames, 10*1000)) { // 10 sconds
				std::cout << "timeout!" << std::endl;
				break;
			}
			
			libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
			libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
			libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];

			// do the registration to rectify the depth (and map to colour, if used):
			if (use_colour) {
				registration->apply(rgb, depth, &undistorted, &registered);

				// copy colur image
				int nextColourFrame = (lastColourFrame + 1) % colourFrames.size();
				ColourFrame& colourFrame = colourFrames[nextColourFrame];
				ColorPoint * dst = colourFrame.color;
				ColourPacket * src = (ColourPacket *)rgb->data;

				static const int nCells = cColorWidth * cColorHeight;
				for (int i = 0; i < nCells; ++i) {
					dst[i].r = src[i].r;
					dst[i].g = src[i].g;
					dst[i].b = src[i].b;
				}

				// we finished writing, we can now share this as the next frame to read:
				//colourFrame.timeStamp = currentColorFrameTime;
				lastColourFrame = nextColourFrame;

			} else {
				registration->undistortDepth(depth, &undistorted);
			}

			// get the next frame to write into:
			int nextCloudFrame = (lastCloudFrame + 1) % cloudFrames.size();
			CloudFrame& cloudFrame = cloudFrames[nextCloudFrame];

			// undistorted frame contains depth in mm, as float
			const float * mmptr = (float *)undistorted.data;
			uint16_t * dptr = cloudFrame.depth;
			glm::vec3 * xyzptr = cloudFrame.xyz;
			glm::vec3 * rgbptr = cloudFrame.rgb;
			glm::vec2 * uvptr = cloudFrame.uv;
			int i = 0;

			// TODO dim or dim-1? 
			//glm::vec2 uvscale = glm::vec2(1.f / cDepthWidth, 1.f / cDepthHeight);
			glm::vec2 uvscale = glm::vec2(1.f / cColorWidth, 1.f / cColorHeight);

			// copy to captureFrame:
			for (int r=0; r<cDepthHeight; r++) {

				for (int c=0; c<cDepthWidth; c++) {
					float mm = mmptr[i];
					dptr[i] = mm;

					glm::vec3 pt;
					float rgb = 0.f;
					//registration->getPointXYZ(&undistorted, r, c, pt.x, pt.y, pt.z);
					registration->getPointXYZRGB (&undistorted, &registered, r, c, pt.x, pt.y, pt.z, rgb);
					pt = -al_fixnan(pt);
					xyzptr[i] = transform(cloudTransform, pt);

					const uint8_t *cp = reinterpret_cast<uint8_t*>(&rgb);
					rgbptr[i] = glm::vec3(cp[2]/255.f, cp[1]/255.f, cp[0]/255.f);

					// this could be wrong?
					glm::vec2 uv;
					registration->apply(c, r, mm, uv.x, uv.y);
					uv *= uvscale;
					//uv = glm::vec2(c, r) * uvscale;
					uvptr[i] = uv;

					i++;
					
					//if (r == 211 && c == 253) printf("depth mm %f point %f %f %f uv %f %f\n", mmptr[i], pt.x, pt.y, pt.z, uv.x, uv.y);
				}
			}

			// we finished writing, we can now share this as the next frame to read:
			//cloudFrame.timeStamp = currentDepthFrameTime;
			//printf("at %d depth\n", currentDepthFrameTime);
			lastCloudFrame = nextCloudFrame;

			listener.release(frames);

			if (fps.measure()) {
				printf("freenect %d fps %f\n", id, fps.fps);
			}
		
		}

		printf("bye from freenect thread for device %s\n", dev->getSerialNumber().c_str());
		dev->stop();
		dev->close();
		return 0;
	}

#endif
#ifdef AL_USE_KINECT2_SDK
    IKinectSensor * device;
	IMultiSourceFrameReader* m_reader;   // Kinect data source
	IDepthFrameReader* m_pDepthFrameReader;
    IColorFrameReader* m_pColorFrameReader;
	int64_t currentDepthFrameTime = 0;
	int64_t currentColorFrameTime = 0;
	int64_t timestampDiff = 0;

	ICoordinateMapper* m_mapper;         // Converts between depth, color, and 3d coordinates
	WAITABLE_HANDLE m_coordinateMappingChangedEvent;

	RGBQUAD m_rgb_buffer[cColorWidth * cColorHeight]; // used for internal processing;
	CameraIntrinsics intrinsics = {};
	/* {
    float FocalLengthX;
    float FocalLengthY;
    float PrincipalPointX;
    float PrincipalPointY;
    float RadialDistortionSecondOrder;
    float RadialDistortionFourthOrder;
    float RadialDistortionSixthOrder;
    }*/
#endif


	bool isRecording() {
		return recordFD;
	}

	// TODO: work in progress
	bool record(bool enable) {
		if (enable) {
			if (recordFD) return true; // already recording

			std::string filename = "kinect2.bin";
			if (filename.empty()) return false;

			recordFD = fopen(filename.c_str(), "wb");
			return true;

		} else {
			// I guess this might be bad if currently writing a frame?
			if (recordFD) fclose(recordFD);
			recordFD = 0;
			return true;
		}
	}


    bool start() {
		printf("opening cloud device %s\n", serial.c_str());
#ifdef AL_USE_FREENECT2_SDK

		
		
		if (dev == 0) {
			printf("couldn't acquire cloud device\n");
			return false;
		} else {
			capturing = 1;
			kinect_thread = std::thread(&CloudDevice::kinect_thread_fun, this);
			return true;
		}
#endif
#ifdef AL_USE_KINECT2_SDK
        HRESULT result = 0;

		result = GetDefaultKinectSensor(&device);
		if (result != S_OK) {
			// TODO: get meaningful error string from error code
			printf("Kinect for Windows could not initialize.\n");
			return false;
		} else {
			HRESULT hr = device->Open();
			if (!SUCCEEDED(hr)) {
				printf("failed to open device");
				SafeRelease(device);
				return false;
			}
			printf("got RGBD device\n");

			long priority = 10; // maybe increase?

			kinect_thread = std::thread(&CloudDevice::run, this);
			return true;
		}
#endif
		return false;
    }

#ifdef AL_USE_KINECT2_SDK
	void run() {
		printf("ready to capture with RGBD device\n");
		HRESULT hr = S_OK;
		{
			IDepthFrameSource* pDepthFrameSource = NULL;
			IColorFrameSource* pColorFrameSource = NULL;
			{
				hr = device->get_DepthFrameSource(&pDepthFrameSource);
				if (FAILED(hr)) { printf("KINECT failed\n"); return; }
				hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
				if (FAILED(hr)) { printf("KINECT failed\n"); return; }
			
				hr = device->get_CoordinateMapper(&m_mapper);
				if (FAILED(hr)) { printf("KINECT failed\n"); return; }
				hr = m_mapper->SubscribeCoordinateMappingChanged(&m_coordinateMappingChangedEvent); // TODO
				if (FAILED(hr)) { printf("KINECT failed\n"); return; }
			}
			if (use_colour) {
				hr = device->get_ColorFrameSource(&pColorFrameSource);
				if (FAILED(hr)) { printf("KINECT failed\n"); return; }
				hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
				if (FAILED(hr)) { printf("KINECT failed\n"); return; }
				
			}
			SafeRelease(pDepthFrameSource);
			SafeRelease(pColorFrameSource);
		}

		printf("starting capture with RGBD device\n");
		capturing = 1;
		FPS fps;
		while (capturing) {
			HRESULT hr = S_OK;
		
			if (m_coordinateMappingChangedEvent != NULL &&
            	WAIT_OBJECT_0 == WaitForSingleObject((HANDLE)m_coordinateMappingChangedEvent, 0)) {
				printf("EVENT: coordinate mapping changed\n");
				
				m_mapper->GetDepthCameraIntrinsics(&intrinsics);
				float focalLengthX = intrinsics.FocalLengthX / cDepthWidth;
				float focalLengthY = intrinsics.FocalLengthY / cDepthHeight;
				float principalPointX = intrinsics.PrincipalPointX / cDepthWidth;
				float principalPointY = intrinsics.PrincipalPointY / cDepthHeight;
				
				ResetEvent((HANDLE)m_coordinateMappingChangedEvent);
			}

			// try to grab colour first, since it tends to be laggier
			int nextColourFrame = (lastColourFrame + 1) % colourFrames.size();
			ColourFrame& colourFrame = colourFrames[nextColourFrame];
			if (use_colour) {
				IColorFrame* pColorFrame;
				hr = m_pColorFrameReader->AcquireLatestFrame(&pColorFrame);
				if (SUCCEEDED(hr)) {
					static const int nCells = cColorWidth * cColorHeight;
					RGBQUAD *src = m_rgb_buffer;
					HRESULT hr = pColorFrame->CopyConvertedFrameDataToArray(nCells * sizeof(RGBQUAD), reinterpret_cast<BYTE*>(src), ColorImageFormat_Bgra);
					if (SUCCEEDED(hr)) {
						ColorPoint * dst = colourFrame.color;
						for (int i = 0; i < nCells; ++i) {
							dst[i].r = src[i].rgbRed;
							dst[i].g = src[i].rgbGreen;
							dst[i].b = src[i].rgbBlue;
							//dst[i].a = 255;
							//new_rgb_data = 1;
						}
						hr = pColorFrame->get_RelativeTime(&currentColorFrameTime);
						currentColorFrameTime /= 10000;
						
						// we finished writing, we can now share this as the next frame to read:
						colourFrame.timeStamp = currentColorFrameTime;
						lastColourFrame = nextColourFrame;

						if (isRecording()) {
							fwrite(&colourFrame, sizeof(ColourFrame), 1, recordFD);
						}
						//printf("at %d colour\n", currentColorFrameTime);
					}
					SafeRelease(pColorFrame);
				}
			}

			// identify which is the next frame to write into:
			int nextCloudFrame = (lastCloudFrame + 1) % cloudFrames.size();
			CloudFrame& cloudFrame = cloudFrames[nextCloudFrame];
			bool isNewDepthFrame = false;
			{
				IDepthFrame* pDepthFrame = NULL;
				hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);
				if (FAILED(hr)) {
					SafeRelease(pDepthFrame);
					//printf("failed to acquire depth frame\n");
					continue; 
				}
				
				UINT capacity;
				UINT16 * src; // depth in mm
				DepthPoint * dst = cloudFrame.depth;
				hr = pDepthFrame->AccessUnderlyingBuffer(&capacity, &src);
				if (SUCCEEDED(hr)) {
					// copy to captureFrame:
					//for (UINT i = 0; i < capacity; i++) { dst[i] = src[i]; }
					memcpy(dst, src, sizeof(uint16_t) * capacity);

					isNewDepthFrame = true;
					pDepthFrame->get_RelativeTime(&currentDepthFrameTime);
					currentDepthFrameTime /= 10000;
				}
				SafeRelease(pDepthFrame);
			}
			if (isNewDepthFrame) {
				// map captureFrame.depth into XYZ space
				hr = m_mapper->MapDepthFrameToCameraSpace(
					cDepthWidth*cDepthHeight, (UINT16 *)cloudFrame.depth,        // Depth frame data and size of depth frame
					cDepthWidth*cDepthHeight, (CameraSpacePoint *)cloudFrame.xyz); // Output CameraSpacePoint array and size
				if (SUCCEEDED(hr)) {
					// figure out the UVs:
					if (use_uv) {
						// TODO dim or dim-1? 
						glm::vec2 uvscale = glm::vec2(1.f / cColorWidth, 1.f / cColorHeight);
						//uint64_t pts = 0;
						// iterate the points to get UVs
						for (UINT i = 0, y = 0; y < cDepthHeight; y++) {
							for (UINT x = 0; x < cDepthWidth; x++, i++) {
								// TODO: add 0.5 for center of pixel?
								//DepthSpacePoint dp = { (float)x, (float)y };
								// TODO: this could be baked, and avoid the double for loop
								DepthSpacePoint dp = { (float)x + 0.5f, (float)y + 0.5f };
								UINT16 depth_mm = cloudFrame.depth[i];
								glm::vec2 uvpt;
								m_mapper->MapDepthPointToColorSpace(dp, depth_mm, (ColorSpacePoint *)(&uvpt));
								cloudFrame.uv[i] = uvpt * uvscale;
								// TODO create an indices table?
								//if (depth_mm > 0) pts++;
							}
						}
						//pointCount = pts;
						//new_uv_data = 1;
					}

				}

				// TODO: create a quick tri mesh here?

				// we finished writing, we can now share this as the next frame to read:
				cloudFrame.timeStamp = currentDepthFrameTime;
				//printf("at %d depth\n", currentDepthFrameTime);
				lastCloudFrame = nextCloudFrame;

				if (isRecording()) {
					fwrite(&cloudFrame, sizeof(CloudFrame), 1, recordFD);
				}
			}
			
			if (fps.measure()) {
				printf("kinect fps %f\n", fps.fps);
			}
		}
		printf("ending capture with RGBD device\n");
	}
#endif

    void close() {
#ifdef AL_USE_FREENECT2_SDK

		if (capturing) {
			capturing = 0;
			kinect_thread.join();
		}
		
#endif
#ifdef AL_USE_KINECT2_SDK
        if (capturing) {
            capturing = 0;
			kinect_thread.join();
        }
        if (device) {
			device->Close();
			SafeRelease(device);
		}
#endif
    }
};



struct CloudDeviceManager {
	CloudDevice devices[KINECT_MAX_DEVICES];
	int numDevices = 0; 

#ifdef AL_USE_FREENECT2_SDK

	libfreenect2::Freenect2 freenect2;
	
	void reset() {
		for (int i=0; i<KINECT_MAX_DEVICES; i++) {
			devices[i].close();
		}

		

		numDevices = freenect2.enumerateDevices();
		printf("found %d freenect devices\n", numDevices);
		
		// sort by serial number, to make it deterministic
		std::vector<std::string> serialList;
		for (int i=0; i<numDevices; i++) {
			serialList.push_back(freenect2.getDeviceSerialNumber(i));
		}
		std::sort(std::begin(serialList), std::end(serialList));
		for (int i=0; i<numDevices; i++) {
			devices[i].id = i;
			devices[i].serial = serialList[i].c_str();
			printf("device %d serial %s\n", devices[i].id, serialList[i].c_str());
		}
	}

	bool open(int i=0) {
		if (i >= numDevices) {
			printf("cannot open device %d, not found\n", i);
			return false;
		}
		
		//libfreenect2::Freenect2Device * dev;
		CloudDevice& device = devices[i];

		if (!device.pipeline) {
			//device.pipeline = new libfreenect2::CpuPacketPipeline();
			//device.pipeline = new libfreenect2::OpenGLPacketPipeline();
			device.pipeline = new libfreenect2::CudaPacketPipeline();
		}
		if (device.pipeline) {
			device.dev = freenect2.openDevice(device.serial, device.pipeline);
		} else {
			device.dev = freenect2.openDevice(device.serial);
		}

		device.start();
	}

	

#else 
	void reset() {}
	bool open(int i=0) { return false; }
#endif

	void open_all() {
		for (int i=0; i<KINECT_MAX_DEVICES; i++) {
			open (i);
		}
	}

	void close(int i=0) {
		devices[i].close();
	}

	void close_all() {
		for (int i=0; i<KINECT_MAX_DEVICES; i++) {
			close (i);
		}
	}
};

#endif // AL_KINECT2_H