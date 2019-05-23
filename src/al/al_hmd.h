#ifndef AL_HMD_H
#define AL_HMD_H

#include "al_console.h"
#include "al_math.h"
#include "al_gl.h"


#ifdef AL_WIN
#define USE_STEAM_DRIVER 1
//#define USE_OCULUS_DRIVER 1
#endif

#define VR_DEBUG_POST(fmt, ...) do { console.log("debug line %d:%s(): " fmt, __LINE__, __func__, __VA_ARGS__); } while (0)

#ifdef USE_STEAM_DRIVER
// The OpenVR SDK:
#ifdef AL_IMPLEMENTATION
#define VR_API_EXPORT
#endif
#include "openvr/openvr.h"

static std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL)
{
    std::string sResult = "<unknown>";

    uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
    if (unRequiredBufferLen == 0) return sResult;

    char *pchBuffer = new char[unRequiredBufferLen];
    unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
    sResult = (pchBuffer);
    delete[] pchBuffer;
    return sResult;
}

glm::mat4 mat4_from_openvr(const vr::HmdMatrix34_t &m) {
	return glm::mat4(
		m.m[0][0], m.m[1][0], m.m[2][0], 0.0,
		m.m[0][1], m.m[1][1], m.m[2][1], 0.0,
		m.m[0][2], m.m[1][2], m.m[2][2], 0.0,
		m.m[0][3], m.m[1][3], m.m[2][3], 1.0f
	);
}

glm::mat4 mat4_from_openvr(const vr::HmdMatrix44_t &m) {
	return glm::mat4(
		m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
		m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
		m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
		m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]
	);
}

#endif

struct Hmd {
    struct Frustum {
		float l, r, b, t, n, f;
	};

    // attrs:
	float near_clip = 0.15f;
	float far_clip = 50.f;
    Frustum frustum[2];

	int32_t steam_available = 0;
    int32_t attached = 0;
	int32_t connected = 0;

    glm::mat4 mHMDPose = glm::mat4(1.f);
    // the view matrix for each eye:
	glm::mat4 m_mat4viewEye[2];
	glm::mat4 mProjMatEye[2];
    glm::vec3 mTrackedPosition;
	glm::vec3 mTrackedVelocity, mTrackedAngularVelocity;
	glm::vec3 mWorldPosition;
	glm::quat mTrackedQuat;
	glm::quat mWorldQuat;
    
    glm::mat4 mModelViewEye[2];
	glm::vec3 mWorldPositionEye[2];
	glm::quat mWorldQuatEye[2];
    
	int mHandControllerDeviceIndex[2];
	glm::vec3 mHandTrackedPosition[2];
	glm::quat mHandTrackedQuat[2];

	// the fbo/texture to draw into when submitting frames:
	SimpleFBO fbo;

#ifdef USE_STEAM_DRIVER
    vr::IVRSystem *	mHMD = 0;
    vr::TrackedDevicePose_t pRenderPoseArray[vr::k_unMaxTrackedDeviceCount];
	glm::mat4 mDevicePose[vr::k_unMaxTrackedDeviceCount];
#endif

    // attempt to connect to the Vive runtime, creating a session:
	bool connect(bool force_reconnect=false) {
#ifdef USE_STEAM_DRIVER
        if (mHMD) {
            if (force_reconnect) {
                disconnect(); // i.e. reconnect
            } else {
                return true; // we're already connected
            }
        }

        vr::EVRInitError eError = vr::VRInitError_None;
		mHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);
        if (eError != vr::VRInitError_None) {
			mHMD = 0;
			console.error("Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
			return false;
		}
        if (!vr::VRCompositor()) {
            mHMD = 0;
			console.error("Compositor initialization failed.");
			return false;
		}
        if (mHMD) {
            configure();

            connected = 1;
            return true;
        }

#endif
        return false;
    }

    void configure() { 
#ifdef USE_STEAM_DRIVER
		if (!mHMD) return;

        std::string mDisplay = GetTrackedDeviceString(mHMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
		std::string mDriver = GetTrackedDeviceString(mHMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);
		console.log("VR display %s driver %s", mDisplay.c_str(), mDriver.c_str());

		uint32_t w, h;
		mHMD->GetRecommendedRenderTargetSize(&w, &h);
		w *= 2; // because we are sending side-by-side images in a single texture

		fbo.dim.x = w;
		fbo.dim.y = h;
		console.log("VR texture recommendation %d x %d", fbo.dim.x, fbo.dim.y);
		
		// trash any existing resources:
		dest_closing();
		
        
#endif   
    }


    void update(glm::vec3 position = glm::vec3(), glm::quat orientation = glm::quat()) {

#ifdef USE_STEAM_DRIVER
		if (!mHMD) return;

        // get desired model matrix (for navigation)
		glm::mat4 modelview_mat = glm::translate(glm::mat4(1.0f), position) * mat4_cast(orientation);

		// setup cameras:
		for (int i = 0; i < 2; i++) {

			vr::HmdMatrix34_t matEye = mHMD->GetEyeToHeadTransform((vr::Hmd_Eye)i);
			
			/*glm::mat4 matrixObj(
				matEye.m[0][0], matEye.m[1][0], matEye.m[2][0], 0.0,
				matEye.m[0][1], matEye.m[1][1], matEye.m[2][1], 0.0,
				matEye.m[0][2], matEye.m[1][2], matEye.m[2][2], 0.0,
				matEye.m[0][3], matEye.m[1][3], matEye.m[2][3], 1.0f
			);*/

			m_mat4viewEye[i] = mat4_from_openvr(matEye);//matrixObj;
			mProjMatEye[i] = mat4_from_openvr(mHMD->GetProjectionMatrix((vr::Hmd_Eye)i, near_clip, far_clip));

			// for some reason, this isn't producing the same results with glm::frustum as mProjMatEye
			// so I don't trust it.
			float l, r, t, b;
			mHMD->GetProjectionRaw((vr::Hmd_Eye)i, &l, &r, &t, &b);
			frustum[i].l = l*near_clip;
			frustum[i].r = r*near_clip;
			frustum[i].b = -b*near_clip;//-b*near_clip;
			frustum[i].t = -t*near_clip;//-t*near_clip;
			frustum[i].n = near_clip;
			frustum[i].f = far_clip;
		}

        vr::VREvent_t event;
		while (mHMD->PollNextEvent(&event, sizeof(event))) {
			switch (event.eventType) {
			case vr::VREvent_TrackedDeviceActivated:
			{
				attached = 1;
				//setupRenderModelForTrackedDevice(event.trackedDeviceIndex);
			}
			break;
			case vr::VREvent_TrackedDeviceDeactivated:
			{
				attached = 0;
			}
			break;
			case vr::VREvent_TrackedDeviceUpdated:
			{
			}
			break;
			//default: console.log("htcvive event %d", event.eventType);
			}
		}

        // get the tracking data here
		vr::EVRCompositorError err = vr::VRCompositor()->WaitGetPoses(pRenderPoseArray, vr::k_unMaxTrackedDeviceCount, NULL, 0);
		if (err != vr::VRCompositorError_None) {
			console.error("WaitGetPoses error");
			return;
		}

        // TODO: should we ignore button presses etc. if so?
		bool inputCapturedByAnotherProcess = mHMD->IsInputFocusCapturedByAnotherProcess();

        // check each device:
		for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++) {
			const vr::TrackedDevicePose_t& trackedDevicePose = pRenderPoseArray[i];

			if (trackedDevicePose.bDeviceIsConnected) {
				if (trackedDevicePose.bPoseIsValid) {
					// this is the view matrix relative to the 'chaperone' space origin
					// (the center of the floor space in the real world)
					// do we need to transform this by the scene?
					mDevicePose[i] = mat4_from_openvr(trackedDevicePose.mDeviceToAbsoluteTracking);
				}

				switch (mHMD->GetTrackedDeviceClass(i)) {
				case vr::TrackedDeviceClass_HMD: {
					if (trackedDevicePose.bPoseIsValid) {
						mHMDPose = mDevicePose[i];
						glm::mat4 world_pose = modelview_mat * mHMDPose;

						// probably want to output this for navigation etc. use
						glm::vec3 p = glm::vec3(mHMDPose[3]); // the translation component
						mTrackedPosition = p;

						glm::quat q = glm::quat_cast(mHMDPose);
						//q = glm::normalize(q);
						mTrackedQuat = q;

						p = glm::vec3(world_pose[3]); // the translation component
						mWorldPosition = p;

						q = glm::quat_cast(world_pose);
						mWorldQuat = q;

						mTrackedVelocity = glm::vec3(trackedDevicePose.vVelocity.v[0], trackedDevicePose.vVelocity.v[1], trackedDevicePose.vVelocity.v[2]);
						mTrackedAngularVelocity = glm::vec3(trackedDevicePose.vAngularVelocity.v[0], trackedDevicePose.vAngularVelocity.v[1], trackedDevicePose.vAngularVelocity.v[2]);
					}
				} break;
					
				case vr::TrackedDeviceClass_Controller: {
					// check role to see if these are hands
					vr::ETrackedControllerRole role = mHMD->GetControllerRoleForTrackedDeviceIndex(i);
					switch (role) {
					case vr::TrackedControllerRole_LeftHand:
					case vr::TrackedControllerRole_RightHand: {
						//if (trackedDevicePose.eTrackingResult == vr::TrackingResult_Running_OK) {

						int hand = (role == vr::TrackedControllerRole_RightHand);
						mHandControllerDeviceIndex[hand] = i;

						if (trackedDevicePose.bPoseIsValid) {

							glm::mat4& tracked_pose = mDevicePose[i];
							glm::mat4 world_pose = modelview_mat * tracked_pose;
							
							// probably want to output this for navigation etc. use
							glm::vec3 p = glm::vec3(tracked_pose[3]); // the translation component
							mHandTrackedPosition[hand] = p;

							glm::quat q = glm::quat_cast(tracked_pose);
							//q = glm::normalize(q);
							mHandTrackedQuat[hand] = q;

/*
							// output the raw tracking data:
							glm::vec3 p = glm::vec3(tracked_pose[3]); // the translation component
							atom_setfloat(a + 0, p.x);
							atom_setfloat(a + 1, p.y);
							atom_setfloat(a + 2, p.z);
							outlet_anything(outlet_controller[hand], ps_tracked_position, 3, a);

							glm::quat q = glm::quat_cast(tracked_pose);
							//q = glm::normalize(q);
							atom_setfloat(a + 0, q.x);
							atom_setfloat(a + 1, q.y);
							atom_setfloat(a + 2, q.z);
							atom_setfloat(a + 3, q.w);
							outlet_anything(outlet_controller[hand], ps_tracked_quat, 4, a);

							p = glm::vec3(world_pose[3]); // the translation component
							atom_setfloat(a + 0, p.x);
							atom_setfloat(a + 1, p.y);
							atom_setfloat(a + 2, p.z);
							outlet_anything(outlet_controller[hand], _jit_sym_position, 3, a);

							q = glm::quat_cast(world_pose);
							atom_setfloat(a + 0, q.x);
							atom_setfloat(a + 1, q.y);
							atom_setfloat(a + 2, q.z);
							atom_setfloat(a + 3, q.w);
							outlet_anything(outlet_controller[hand], _jit_sym_quat, 4, a);

							atom_setfloat(a + 0, trackedDevicePose.vVelocity.v[0]);
							atom_setfloat(a + 1, trackedDevicePose.vVelocity.v[1]);
							atom_setfloat(a + 2, trackedDevicePose.vVelocity.v[2]);
							outlet_anything(outlet_controller[hand], ps_velocity, 3, a);

							atom_setfloat(a + 0, trackedDevicePose.vAngularVelocity.v[0]);
							atom_setfloat(a + 1, trackedDevicePose.vAngularVelocity.v[1]);
							atom_setfloat(a + 2, trackedDevicePose.vAngularVelocity.v[2]);
							outlet_anything(outlet_controller[hand], ps_angular_velocity, 3, a);
							*/
						}

						/*
						vr::VRControllerState_t cs;
						//OpenVR SDK 1.0.4 adds a 3rd arg for size
						mHMD->GetControllerState(i, &cs, sizeof(cs));

						atom_setlong(a + 0, (cs.ulButtonTouched & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)) != 0);
						atom_setfloat(a + 1, cs.rAxis[1].x);
						outlet_anything(outlet_controller[hand], ps_trigger, 2, a);

						atom_setlong(a + 0, (cs.ulButtonTouched & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad)) != 0);
						atom_setfloat(a + 1, cs.rAxis[0].x);
						atom_setfloat(a + 2, cs.rAxis[0].y);
						atom_setlong(a + 3, (cs.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad)) != 0);
						outlet_anything(outlet_controller[hand], ps_trackpad, 4, a);

						//TODO: The API appears to partition the Touchpad to D-Pad quadrants internally, investigate!
						//vr::k_EButton_DPad_Down etc.

						atom_setlong(a + 0, (cs.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_ApplicationMenu)) != 0);
						atom_setlong(a + 1, (cs.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Grip)) != 0);
						outlet_anything(outlet_controller[hand], ps_buttons, 2, a);

						//}
						*/
					}
					
															  break;
					default:
						break;
					}
				} break;
				/*
				case vr::TrackedDeviceClass_GenericTracker:
				{
					if (trackedDevicePose.bPoseIsValid) {

						//Figure out which tracker it is using some kind of unique identifier
						vr::ETrackedPropertyError err = vr::TrackedProp_Success;
						char buf[32];
						uint32_t unPropLen = vr::VRSystem()->GetStringTrackedDeviceProperty(i, vr::Prop_SerialNumber_String, buf, sizeof(buf), &err);
						//Append the UID onto the data header going into Max
						if (err == vr::TrackedProp_Success)
						{
							char* result;
							result = (char*)calloc(strlen("tracker_velocity_") + strlen(buf) + 1, sizeof(char));
							strcpy(result, "tracker_velocity_");
							strcat(result, buf);
							trk_velocity = gensym(result);
							free(result);

							result = (char*)calloc(strlen("tracker_angular_velocity_") + strlen(buf) + 1, sizeof(char));
							strcpy(result, "tracker_angular_velocity_");
							strcat(result, buf);
							trk_angular_velocity = gensym(result);
							free(result);

							result = (char*)calloc(strlen("tracker_tracked_position_") + strlen(buf) + 1, sizeof(char));
							strcpy(result, "tracker_tracked_position_");
							strcat(result, buf);
							trk_tracked_position = gensym(result);
							free(result);

							result = (char*)calloc(strlen("tracker_tracked_quat_") + strlen(buf) + 1, sizeof(char));
							strcpy(result, "tracker_tracked_quat_");
							strcat(result, buf);
							trk_tracked_quat = gensym(result);
							free(result);
						}

						mat4& tracked_pose = mDevicePose[i];
						glm::mat4 world_pose = modelview_mat * tracked_pose;

						// output the raw tracking data:
						glm::vec3 p = glm::vec3(tracked_pose[3]); // the translation component
						atom_setfloat(a + 0, p.x);
						atom_setfloat(a + 1, p.y);
						atom_setfloat(a + 2, p.z);
						outlet_anything(outlet_tracking, trk_tracked_position, 3, a);
						glm::quat q = glm::quat_cast(tracked_pose);
						//q = glm::normalize(q);
						atom_setfloat(a + 0, q.x);
						atom_setfloat(a + 1, q.y);
						atom_setfloat(a + 2, q.z);
						atom_setfloat(a + 3, q.w);
						outlet_anything(outlet_tracking, trk_tracked_quat, 4, a);

						atom_setfloat(a + 0, trackedDevicePose.vVelocity.v[0]);
						atom_setfloat(a + 1, trackedDevicePose.vVelocity.v[1]);
						atom_setfloat(a + 2, trackedDevicePose.vVelocity.v[2]);
						outlet_anything(outlet_tracking, trk_velocity, 3, a);

						atom_setfloat(a + 0, trackedDevicePose.vAngularVelocity.v[0]);
						atom_setfloat(a + 1, trackedDevicePose.vAngularVelocity.v[1]);
						atom_setfloat(a + 2, trackedDevicePose.vAngularVelocity.v[2]);
						outlet_anything(outlet_tracking, trk_angular_velocity, 3, a);
					}
				} break;

				*/
				default:
					break;
				}
			}
		}

        // now update cameras:
		for (int i = 0; i < 2; i++) {
			mModelViewEye[i] = modelview_mat * mHMDPose * m_mat4viewEye[i];
			mWorldPositionEye[i] = glm::vec3(mModelViewEye[i][3]);
			mWorldQuatEye[i] = glm::quat_cast(mModelViewEye[i]);
		}

        // video_step()?
#endif
    }

	// maybe this is redundant. SimpleFBO already has a blit method...
	void submit(SimpleFBO& srcFBO) {
#ifdef USE_STEAM_DRIVER
		if (!mHMD) return;
		if (!fbo.fbo) {
			console.log("no HMD FBO/texure yet");
			return;	// no texture to copy from.
		}

		// blit it.
		glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFBO.fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.fbo);
		glBlitFramebuffer(0, 0, srcFBO.dim.x, srcFBO.dim.y, 0, 0, fbo.dim.x, fbo.dim.y, GL_COLOR_BUFFER_BIT,GL_LINEAR);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		submit();
#endif		
	}

	
	void submit() {
#ifdef USE_STEAM_DRIVER
		if (!mHMD) return;
		if (!fbo.tex) {
			console.log("no HMD FBO/texure yet");
			return;	// no texture to copy from.
		}

		vr::EVRCompositorError err;
		vr::Texture_t vrTexture = { (void*)fbo.tex, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRTextureBounds_t leftBounds = { 0.f, 0.f, 0.5f, 1.f };
		vr::VRTextureBounds_t rightBounds = { 0.5f, 0.f, 1.f, 1.f };

		err = vr::VRCompositor()->Submit(vr::Eye_Left, &vrTexture, &leftBounds);
		switch (err) {
		case 0:
			break;
		case 1:
			console.log("submit error: Request failed.");
			break;
		case 100:
			console.log("submit error: Incompatible version.");
			break;
		case 101:
			console.log("submit error: Do not have focus.");
			break;
		case 102:
			console.log("submit error: Invalid texture.");
			break;
		case 103:
			console.log("submit error: Is not scene application.");
			break;
		case 104:
			console.log("submit error: Texture is on wrong device.");
			break;
		case 105:
			console.log("submit error: Texture uses unsupported format.");
			break;
		case 106:
			console.log("submit error: Shared textures not supported.");
			break;
		case 107:
			console.log("submit error: Index out of range.");
			break;
		case 108:
			console.log("submit error: Already submitted.");
			break;
		}

		err = vr::VRCompositor()->Submit(vr::Eye_Right, &vrTexture, &rightBounds);

#endif		
	}

    void disconnect() {
#ifdef USE_STEAM_DRIVER
        if (mHMD) {
			console.log("disconnecting\n");
			//video_stop();
			vr::VR_Shutdown();
			mHMD = 0;
            connected = 0;
		}
#endif
    }

	bool dest_changed() {
		console.log("HMD dest_changed %d %d", fbo.dim.x, fbo.dim.y);
		return fbo.dest_changed();
	}

    void dest_closing() {
		fbo.dest_closing();
	}

    ~Hmd() {
		dest_closing();
        disconnect();
    }
};

#endif //AL_HMD_H
