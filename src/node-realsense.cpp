#include <node_api.h> 
#include <napi.h>

#include <assert.h>
#include <stdio.h> 
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <vector>


#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API

#include "al/al_glm.h"

// Euclidean modulo. assumes n > 0
int wrap(int a, int n) { 
	const int r = a % n;
	return r < 0 ? r + n : r; //a % n + (Math.sign(a) !== Math.sign(n) ? n : 0); 
}

 struct Pipeline : public Napi::ObjectWrap<Pipeline> {

	// Create a Pipeline - this serves as a top-level API for streaming and processing frames
	rs2::pipeline p;
	// https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1config.html
	rs2::config config;
	// Declare pointcloud object, for calculating pointclouds and texture mappings
	//https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1pointcloud.html
	rs2::pointcloud pc;
	// We want the points object to be persistent so we can display the last cloud when a frame drops
	// https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1points.html
	rs2::points points;

	// min & max bounds for mesh & voxel processing (world space)
	glm::vec3 min = glm::vec3(-10, -10, -10); 
	glm::vec3 max = glm::vec3(10, 10, 10);

	// storage for the vertex xyz points
	Napi::ArrayBuffer vertices_ab;

//	Napi::TypedArrayOf<float> depth;
	Napi::TypedArrayOf<float> vertices;
	Napi::TypedArrayOf<float> normals;
	Napi::TypedArrayOf<uint32_t> indices;
	Napi::TypedArrayOf<float> accel;

// 	// .getWidth(), .getHeight(), .getResolution(), .getChannels()
// 	// .getDataType(), .getMemoryType() (CPU or GPU), .getPtr()
// 	// sl::Mat left;
// 	// sl::Mat depth;
// 	// The point cloud stores its data on 4 channels using 32-bit float for each channel. The last float is used to store color information, where R, G, B, and alpha channels (4 x 8-bit) are concatenated into a single 32-bit float. 
// 	sl::Mat cloud;
// 	// The output is a 4 channels 32-bit matrix (X,Y,Z,empty), where X,Y,Z values encode the direction of the normal vectors.
// 	sl::Mat normals;
// 	sl::Resolution capture_res;
// 	uint64_t ms = 0;

    Pipeline(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Pipeline>(info) {
		Napi::Env env = info.Env();
		Napi::Object This = info.This().As<Napi::Object>();


		accel = Napi::TypedArrayOf<float>::New(env, 3, napi_float32_array);
		This.Set("accel", accel);
		accel[0] = 0;
		accel[1] = 0;
		accel[2] = -10;

	//	if (info.Length()) open(info);
	}

	Napi::Value start(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();
		Napi::Object This = info.This().As<Napi::Object>();

// 		// expect some configuration here
 		const Napi::Object options = info.Length() ? info[0].ToObject() : Napi::Object::New(env);

		//config.enable_stream(RS2_STREAM_ACCEL, RS2_FORMAT_MOTION_XYZ32F);

		// Configure and start the pipeline
		p.start(config);

// 		sl::InitParameters init_parameters;
// 		init_parameters.depth_mode = sl::DEPTH_MODE::PERFORMANCE;
// 		init_parameters.Pipeline_resolution = sl::RESOLUTION::HD720;
// 		init_parameters.Pipeline_fps = 30;
// 		init_parameters.coordinate_units = sl::UNIT::METER;

// 		sl::RuntimeParameters runtime_parameters;
// 		//runtime_parameters.sensing_mode = SENSING_MODE::FILL;
		
// 		// from SVO file:
// 		//init_parameters.input.setFromSVOFile(input_path);
// 		// from IP stream:
// 		// ..
// 		if (options.Has("serial")) {
// 			// try to open device by serial
// 			uint32_t z = options.Get("serial").ToNumber().Uint32Value();
// 			init_parameters.input.setFromSerialNumber(z);
// 		} else if (options.Has("id")) {
// 			// try to open device by id
// 			uint32_t z = options.Get("id").ToNumber().Uint32Value();
// 			init_parameters.input.setFromPipelineID(z);
// 		} 

// 		sl::ERROR_CODE err = zed.open(init_parameters);
// 		if (err != sl::ERROR_CODE::SUCCESS) {
// 			 throw Napi::Error::New(env, sl::toString(err).get());
// 		}

// 		auto cam_info = zed.getPipelineInformation();
// 		//cout << cam_info.Pipeline_model << ", ID: " << z << ", SN: " << cam_info.serial_number << " Opened" << endl;

// 		This.Set("serial", cam_info.serial_number);
// 		This.Set("model", sl::toString(cam_info.Pipeline_model).get());
// 		This.Set("input_type", sl::toString(cam_info.input_type).get());

// 		// has calibration parameters, firmware, fps, resolution:
// 		//This.Set("Pipeline_configuration", sl::toString(cam_info.Pipeline_configuration).get());
// 		This.Set("firmware_version", cam_info.Pipeline_configuration.firmware_version);
// 		This.Set("fps", cam_info.Pipeline_configuration.fps);
		
// 		auto resolution = cam_info.Pipeline_configuration.resolution;
// 		This.Set("width", resolution.width);
// 		This.Set("height", resolution.height);

// 		size_t subdiv = 1;
// 		capture_res.width = resolution.width / subdiv;
// 		capture_res.height = resolution.height / subdiv;

// 		// CalibrationParameters calibration_params = zed.getPipelineInformation()->calibration_parameters;
// 		// // Focal length of the left eye in pixels
// 		// float focal_left_x = calibration_params.left_cam.fx;
// 		// // First radial distortion coefficient
// 		// float k1 = calibration_params.left_cam.disto[0];
// 		// // Translation between left and right eye on z-axis
// 		// float tz = calibration_params.T.z;
// 		// // Horizontal field of view of the left eye in degrees
// 		// float h_fov = calibration_params.left_cam.h_fov;

// 		// has firmware, imu, barometer, magnetometer etc.
// 		//This.Set("sensors_configuration", sl::toString(cam_info.sensors_configuration).get());

// 		// Pipeline settings:
// 		// Set exposure to 50% of Pipeline framerate
// 		// zed.setPipelineSettings(VIDEO_SETTINGS::EXPOSURE, 50);
// 		// // Set white balance to 4600K
// 		// zed.setPipelineSettings(VIDEO_SETTINGS::WHITE_BALANCE, 4600);
// 		// // Reset to auto exposure
// 		// zed.setPipelineSettings(VIDEO_SETTINGS::EXPOSURE, VIDEO_SETTINGS_VALUE_AUTO);

// 		// allocate some memory to store the Left/Depth images
// 		cloud.alloc(capture_res, sl::MAT_TYPE::F32_C4);
// 		This.Set("cloud", Napi::TypedArrayOf<float>::New(env, 
// 			4 * capture_res.width * capture_res.height,
// 			Napi::ArrayBuffer::New(env, cloud.getPtr<float>(), cloud.getStepBytes() * cloud.getHeight()),
// 			0,
// 			napi_float32_array)
// 		);

// 		normals.alloc(capture_res, sl::MAT_TYPE::F32_C4);
// 		This.Set("normals", Napi::TypedArrayOf<float>::New(env, 
// 			4 * capture_res.width * capture_res.height,
// 			Napi::ArrayBuffer::New(env, normals.getPtr<float>(), normals.getStepBytes() * normals.getHeight()),
// 			0,
// 			napi_float32_array)
// 		);

		return This;
	}

	~Pipeline() {
		//zed_close();
		printf("~Pipeline\n");
	}


	Napi::Value grab(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();
		Napi::Object This = info.This().As<Napi::Object>();
	
		bool wait = info.Length() > 0 ? info[0].As<Napi::Boolean>() : false;

		float maxarea = This.Has("maxarea") ?  This.Get("maxarea").ToNumber().DoubleValue() : 0.001;
		glm::mat4 transform = This.Has("modelmatrix") ? glm::make_mat4(This.Get("modelmatrix").As<Napi::Float32Array>().Data()) : glm::mat4();
		//float miny = This.Has("miny") ? This.Get("miny").ToNumber().DoubleValue() : 0.;

		if (This.Has("min")) {
			const Napi::Object value = This.Get("min").ToObject();
			min.x = value.Get(uint32_t(0)).ToNumber().DoubleValue();
			min.y = value.Get(uint32_t(1)).ToNumber().DoubleValue();
			min.z = value.Get(uint32_t(2)).ToNumber().DoubleValue();
		}
		if (This.Has("max")) {
			const Napi::Object value = This.Get("max").ToObject();
			max.x = value.Get(uint32_t(0)).ToNumber().DoubleValue();
			max.y = value.Get(uint32_t(1)).ToNumber().DoubleValue();
			max.z = value.Get(uint32_t(2)).ToNumber().DoubleValue();
		}

		rs2::frameset frames;
		if (wait) {
			// https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1frameset.html
			// Block program until frames arrive
			frames = p.wait_for_frames();
		} else {
			// non-blocking:
			if (!p.poll_for_frames(&frames)) return env.Null();
		}

		if (rs2::motion_frame accel_frame = frames.first_or_default(RS2_STREAM_ACCEL)) {
			rs2_vector a = accel_frame.get_motion_data();
			accel[0] = a.x;
			accel[1] = a.y;
			accel[2] = a.z;
			//printf("accel %f %f %f\n", accel.x, accel.y, accel.z);
		}

		// Try to get a frame of a depth image
		// https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1depth__frame.html
		rs2::depth_frame depth = frames.get_depth_frame();

		
		// rs2::pose_frame pose_frame = frames.get_pose_frame();
		// rs2_pose pose = pose_frame.get_pose_data();
		// printf("accel %f %f %f\n", pose.acceleration.x, pose.acceleration.y, pose.acceleration.z);

		// frame depth.apply_filter (filter_interface &filter).as<rs2::depth_frame>();


		// Get the depth frame's dimensions
		int width = depth.get_width();
		int height = depth.get_height();
		const size_t num_vertices = width * height;
		const size_t num_floats = num_vertices * 3;
		size_t MAX_NUM_INDICES = num_vertices;
		// retrieve frame stride, meaning the actual line width in memory in bytes (not the logical image width)
		int stride = depth.get_stride_in_bytes ();
		int bitspp = depth.get_bits_per_pixel ();
		int bytespp = depth.get_bytes_per_pixel ();
		double t = depth.get_timestamp ();
		const int sz = depth.get_data_size ();
		const void * data = depth.get_data ();
		// Query the distance from the Pipeline to the object in the center of the image
		//float dist_to_center = depth.get_distance(width / 2, height / 2);
		// Print the distance
		// printf("The Pipeline is facing an object %f x %f meters away \n", depth.get_units(), dist_to_center);
		// printf("size %d points %d x %d = %d \n", sz, width, height, width*height);

		// {
		// 	// const size_t num_bytes = num_floats * sizeof(float);
		// 	if (!this->depth || this->depth.ElementLength() != num_floats) {
		// 		// reallocate it:
		// 		printf("reallocating %d floats\n", num_floats);
		// 		this->depth = Napi::TypedArrayOf<float>::New(env, num_floats, napi_float32_array);
		// 		This.Set("depth", this->depth);
		// 	}


		// Generate the pointcloud and texture mappings
		points = pc.calculate(depth);
		//const rs2::vertex * vertices = points.get_vertices ();
		const glm::vec3 * raw_vertices = (glm::vec3 *)points.get_vertices ();  // xyz
		//const rs2::texture_coordinate * texcoords = points.get_texture_coordinates (); // uv
		//const glm::vec2 * texcoords = (glm::vec2 *)points.get_texture_coordinates (); // uv
		
		const size_t num_bytes = num_floats * sizeof(float);
		if (!this->vertices || this->vertices.ElementLength() != num_floats) {
			// reallocate it:
			printf("reallocating %d floats\n", num_floats);
			printf("width %d height %d num vertices %d %d\n", width, height, num_vertices, width * height);
			this->vertices = Napi::TypedArrayOf<float>::New(env, num_floats, napi_float32_array);
			This.Set("vertices", this->vertices);
			
			// this->normals = Napi::TypedArrayOf<float>::New(env, num_floats, napi_float32_array);
			// This.Set("normals", this->normals);
			
			this->indices = Napi::TypedArrayOf<uint32_t>::New(env, MAX_NUM_INDICES, napi_uint32_array);
			This.Set("indices", this->indices);

			This.Set("count", Napi::Number::New(env, 0));
		}
		//memcpy(this->vertices.Data(), raw_vertices, num_bytes);

	
		// see https://intelrealsense.github.io/librealsense/doxygen/rs__export_8hpp_source.html
		glm::vec3 * vertices = (glm::vec3 *)this->vertices.Data();
		glm::vec3 * normals = (glm::vec3 *)this->normals.Data();
		uint32_t * indices = (uint32_t *)this->indices.Data();
		
		int index_count = 0;
		for (int i=0; i<num_vertices; i++) {
			glm::vec3& v = vertices[i];
			v = glm::vec3(transform * glm::vec4(raw_vertices[i], 1.));

			// meshless index array:
			if (v.x > min.x && v.y > min.y && v.z > min.z && v.x < max.x && v.y < max.y && v.z < max.z) {
				indices[index_count] = i;
				index_count++;
			}
		}
		//printf("index count: %d %d\n", index_count, MAX_NUM_INDICES);
		This.Set("count", Napi::Number::New(env, index_count));

		return This;
	}

	Napi::Value grab2(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();
		Napi::Object This = info.This().As<Napi::Object>();
	
		bool wait = info.Length() > 0 ? info[0].As<Napi::Boolean>() : false;
		bool createMesh = info.Length() > 1 ? info[1].As<Napi::Boolean>() : true;
		bool withNormals = info.Length() > 2 ? info[2].As<Napi::Boolean>() : true;

		float maxarea = This.Has("maxarea") ?  This.Get("maxarea").ToNumber().DoubleValue() : 0.001;
		glm::mat4 transform = This.Has("modelmatrix") ? glm::make_mat4(This.Get("modelmatrix").As<Napi::Float32Array>().Data()) : glm::mat4();
		//float miny = This.Has("miny") ? This.Get("miny").ToNumber().DoubleValue() : 0.;

		if (This.Has("min")) {
			const Napi::Object value = This.Get("min").ToObject();
			min.x = value.Get(uint32_t(0)).ToNumber().DoubleValue();
			min.y = value.Get(uint32_t(1)).ToNumber().DoubleValue();
			min.z = value.Get(uint32_t(2)).ToNumber().DoubleValue();
		}
		if (This.Has("max")) {
			const Napi::Object value = This.Get("max").ToObject();
			max.x = value.Get(uint32_t(0)).ToNumber().DoubleValue();
			max.y = value.Get(uint32_t(1)).ToNumber().DoubleValue();
			max.z = value.Get(uint32_t(2)).ToNumber().DoubleValue();
		}

		rs2::frameset frames;
		if (wait) {
			// https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1frameset.html
			// Block program until frames arrive
			frames = p.wait_for_frames();
		} else {
			// non-blocking:
			if (!p.poll_for_frames(&frames)) return env.Null();
		}

		if (rs2::motion_frame accel_frame = frames.first_or_default(RS2_STREAM_ACCEL)) {
			rs2_vector a = accel_frame.get_motion_data();
			accel[0] = a.x;
			accel[1] = a.y;
			accel[2] = a.z;
			//printf("accel %f %f %f\n", accel.x, accel.y, accel.z);
		}

		// Try to get a frame of a depth image
		// https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1depth__frame.html
		rs2::depth_frame depth = frames.get_depth_frame();

		
		// rs2::pose_frame pose_frame = frames.get_pose_frame();
		// rs2_pose pose = pose_frame.get_pose_data();
		// printf("accel %f %f %f\n", pose.acceleration.x, pose.acceleration.y, pose.acceleration.z);

		// frame depth.apply_filter (filter_interface &filter).as<rs2::depth_frame>();


		// Get the depth frame's dimensions
		int width = depth.get_width();
		int height = depth.get_height();
		const size_t num_vertices = width * height;
		const size_t num_floats = num_vertices * 3;
		size_t MAX_NUM_FACES = (width - 1)*(height - 1)*2;
		size_t MAX_NUM_INDICES = MAX_NUM_FACES*6; // two triangles per face
		// retrieve frame stride, meaning the actual line width in memory in bytes (not the logical image width)
		int stride = depth.get_stride_in_bytes ();
		int bitspp = depth.get_bits_per_pixel ();
		int bytespp = depth.get_bytes_per_pixel ();
		double t = depth.get_timestamp ();
		const int sz = depth.get_data_size ();
		const void * data = depth.get_data ();
		// Query the distance from the Pipeline to the object in the center of the image
		//float dist_to_center = depth.get_distance(width / 2, height / 2);
		// Print the distance
		// printf("The Pipeline is facing an object %f x %f meters away \n", depth.get_units(), dist_to_center);
		// printf("size %d points %d x %d = %d \n", sz, width, height, width*height);

		// {
		// 	// const size_t num_bytes = num_floats * sizeof(float);
		// 	if (!this->depth || this->depth.ElementLength() != num_floats) {
		// 		// reallocate it:
		// 		printf("reallocating %d floats\n", num_floats);
		// 		this->depth = Napi::TypedArrayOf<float>::New(env, num_floats, napi_float32_array);
		// 		This.Set("depth", this->depth);
		// 	}

		{
			// Generate the pointcloud and texture mappings
			points = pc.calculate(depth);
			//const rs2::vertex * vertices = points.get_vertices ();
			const float * vertices = (float *)points.get_vertices ();  // xyz
			//const rs2::texture_coordinate * texcoords = points.get_texture_coordinates (); // uv
			const float * texcoords = (float *)points.get_texture_coordinates (); // uv
			
			const size_t num_bytes = num_floats * sizeof(float);
			if (!this->vertices || this->vertices.ElementLength() != num_floats) {
				// reallocate it:
				printf("reallocating %d floats\n", num_floats);
				printf("width %d height %d num vertices %d %d\n", width, height, num_vertices, width * height);
				this->vertices = Napi::TypedArrayOf<float>::New(env, num_floats, napi_float32_array);
				This.Set("vertices", this->vertices);
				
				this->normals = Napi::TypedArrayOf<float>::New(env, num_floats, napi_float32_array);
				This.Set("normals", this->normals);
				
				this->indices = Napi::TypedArrayOf<uint32_t>::New(env, MAX_NUM_INDICES, napi_uint32_array);
				This.Set("indices", this->indices);

				This.Set("count", Napi::Number::New(env, 0));
			}
			
			memcpy(this->vertices.Data(), vertices, num_bytes);
		}

	
		// see https://intelrealsense.github.io/librealsense/doxygen/rs__export_8hpp_source.html
		glm::vec3 * vertices = (glm::vec3 *)this->vertices.Data();
		glm::vec3 * normals = (glm::vec3 *)this->normals.Data();
		uint32_t * indices = (uint32_t *)this->indices.Data();
		// create a triangle mesh
		int index_count = 0;

		// apply transform:
		for (int i=0; i<num_vertices; i++) {
			glm::vec3& v = vertices[i];
			v = glm::vec3(transform * glm::vec4(v, 1.));

			// meshless index array:
			if (!createMesh && v.x > min.x && v.y > min.y && v.z > min.z && v.x < max.x && v.y < max.y && v.z < max.z) {
				indices[index_count] = i;
				index_count++;
			}
		}

		if (createMesh) {
			for (int y = 0; y < height - 1; ++y) {
				for (int x = 0; x < width - 1; ++x) {
					// indices of a quad
					int a = y * width + x, 
						b = y * width + (x+1), 
						c = (y+1)*width + x, 
						d = (y+1)*width + (x+1);

					// raw point data:
					// vec3d point_a = { verts[a].x ,  -1 * verts[a].y,  -1 * verts[a].z };
					// vec3d point_b = { verts[b].x ,  -1 * verts[b].y,  -1 * verts[b].z };
					// vec3d point_c = { verts[c].x ,  -1 * verts[c].y,  -1 * verts[c].z };
					// vec3d point_d = { verts[d].x ,  -1 * verts[d].y,  -1 * verts[d].z };
					glm::vec3& point_a = vertices[a];
					glm::vec3& point_b = vertices[b];
					glm::vec3& point_c = vertices[c];
					glm::vec3& point_d = vertices[d];

					glm::vec3 da = point_d - point_a;
					glm::vec3 ba = point_b - point_a;
					glm::vec3 ca = point_c - point_a;
					
					// normalized cross product of two triangle edges gives face normal
					glm::vec3 daba = glm::cross(da, ba);
					glm::vec3 cada = glm::cross(ca, da);
					// |cross product| of two sides gives area parallelogram, twice area of triangle
					// this will be zero if there are degenerate points
					float abd = glm::length(daba);
					float acd = glm::length(cada);

					glm::vec3 n12;
					// skip if any of these points are zero
					// skip if triangle area is too large (or too small??)
					if (point_a.x > min.x && point_b.x > min.x && point_d.x > min.x && 
						point_a.y > min.y && point_b.y > min.y && point_d.y > min.y && 
						point_a.z > min.z && point_b.z > min.z && point_d.z > min.z && 
						point_a.x < max.x && point_b.x < max.x && point_d.x < max.x && 
						point_a.y < max.y && point_b.y < max.y && point_d.y < max.y && 
						point_a.z < max.z && point_b.z < max.z && point_d.z < max.z && 
						abd > 0. && abd < maxarea) 
					{
						indices[index_count++] = a;
						indices[index_count++] = d;
						indices[index_count++] = b;
						if (withNormals) {
							glm::vec3 n = glm::normalize(daba);
							n12 = n;
							normals[a] = n;
							normals[b] = n;
							normals[d] = n12;
						}

					}
					if (point_a.x > min.x && point_c.x > min.x && point_d.x > min.x && 
						point_a.y > min.y && point_c.y > min.y && point_d.y > min.y && 
						point_a.z > min.z && point_c.z > min.z && point_d.z > min.z && 
						point_a.x < max.x && point_c.x < max.x && point_d.x < max.x && 
						point_a.y < max.y && point_c.y < max.y && point_d.y < max.y && 
						point_a.z < max.z && point_c.z < max.z && point_d.z < max.z && 
						acd > 0. && acd < maxarea) 
					{
						indices[index_count++] = d;
						indices[index_count++] = a;
						indices[index_count++] = c;
						if (withNormals) {
							glm::vec3 n = glm::normalize(cada);
							n12 = glm::normalize(n12 + n);
							normals[a] = n12;
							normals[c] = n;
							normals[d] = n12;
						}
					}
				}
			}
		} 
		//printf("index count: %d %d\n", index_count, MAX_NUM_INDICES);
		This.Set("count", Napi::Number::New(env, index_count));

		// auto color = frames.get_color_frame();
		// // Tell pointcloud object to map to this color frame
		// pc.map_to(color);

		return This;
	}

	// float32array voxels, [dimx, dimy, dimz], lidar2voxels_mat, mul, add
	Napi::Value voxels(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();
		Napi::Object This = info.This().As<Napi::Object>();
		if (info.Length() < 5) return info.This();

		// voxel data:
		Napi::Float32Array voxels_value = info[0].As<Napi::Float32Array>();
		float * voxels_data = voxels_value.Data();
		const size_t NUM_VOXELS = voxels_value.ElementLength();

		const Napi::Object dim_value = info[1].ToObject();
		const int32_t DIMX = dim_value.Get(uint32_t(0)).ToNumber().Int32Value();
		const int32_t DIMY = dim_value.Get(uint32_t(1)).ToNumber().Int32Value();
		const int32_t DIMZ = dim_value.Get(uint32_t(2)).ToNumber().Int32Value();
		glm::ivec3 dim(DIMX, DIMY, DIMZ);

		glm::mat4 lidar2voxels_mat = glm::make_mat4(info[2].As<Napi::Float32Array>().Data());

		float voxels_mul =  info[3].ToNumber().DoubleValue();
		float voxels_add =  info[4].ToNumber().DoubleValue();

		Napi::Float32Array vertices_value = this->vertices;
		glm::vec3 * vertices = (glm::vec3 *)vertices_value.Data();
		// const size_t NUM_FLOATS = vertices_value.ElementLength();
		// const size_t NUM_POINTS = NUM_FLOATS/3;

		uint32_t * indices = (uint32_t *)this->indices.Data();
		uint32_t count = This.Get("count").ToNumber().Int32Value();
		
		// // decay:
		for (size_t i=0; i<NUM_VOXELS; i++) {
			voxels_data[i] *= voxels_mul;
		}
		int total = 0;

		for (uint32_t idx=0; idx < count; idx++) {
			uint32_t i = indices[idx];

			// copy floats:
			const glm::vec3& V = vertices[i];
			// convert to voxel space:
			const glm::vec4 v = lidar2voxels_mat * glm::vec4(V, 1.);
			// compute index:
			int x = v.x * DIMX;
			int y = v.y * DIMY;
			int z = v.z * DIMZ;
			if (x >= 0 && x < DIMX && y >= 0 && y < DIMY && z >= 0 && z < DIMZ) {
				int j = x + y*(DIMX) + z*(DIMX*DIMY);//

				// should this clamp rather than add? 
				voxels_data[j] += voxels_add;
				total++;
			}
		}
		//printf("added %d points %s %s\n", count, glm::to_string(min).c_str(), glm::to_string(max).c_str());
		//printf("added %d points\n", total);

		return This;
	}

	Napi::Value get_vertices(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();
		Napi::Object This = info.This().As<Napi::Object>();

		const size_t num_vertices = points.size();
		const size_t num_floats = num_vertices * 3;
		const size_t num_bytes = num_floats * sizeof(float);

		// if (!vertices_ab || vertices_ab.ByteLength() != num_bytes) {
		// 	// reallocate it:
		// 	vertices_ab = Napi::ArrayBuffer::New(env, num_bytes);
		// 	This.Set("vertices_arraybuffer", vertices_ab);

		vertices = Napi::TypedArrayOf<float>::New(env, num_vertices * 3, napi_float32_array);
		// }

		//Napi::TypedArrayOf<float> vertices_float32array = Napi::TypedArrayOf<float>::New(env, num_vertices * 3, vertices_ab, 0, napi_float32_array);

		return This;
	}

	Napi::Value get_active_profile(const Napi::CallbackInfo& info) {
		Napi::Env env = info.Env();
		rs2::pipeline_profile profile = p.get_active_profile ();

		// https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1pipeline__profile.html
		if (1) {
			Napi::Object res = Napi::Object::New(env);

			const rs2::device dev = profile.get_device();
			res.Set("device", Napi::Object::New(env));
			//https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1device.html

			const std::vector<rs2::stream_profile> streams = profile.get_streams();
			res.Set("streams", Napi::Array::New(env, streams.size()));

			// populate `res` with details of device & streams
			for (int i=0; i<streams.size(); i++) {
				const rs2::stream_profile sp = streams[i];
				// https://intelrealsense.github.io/librealsense/doxygen/classrs2_1_1stream__profile.html

			}

			return res;
		}
	
		return info.This();
	}

};


class Module : public Napi::Addon<Module> {
public:

	// /*
	// 	Returns array
	// */
	// Napi::Value devices(const Napi::CallbackInfo& info) {
	// 	Napi::Env env = info.Env();
	// 	//Napi::Object devices = Napi::Array::New(env);

	// 	// std::vector<sl::DeviceProperties> devList = sl::Pipeline::getDeviceList();
	// 	// for (int i = 0; i < devList.size(); i++) {
	// 	// 	Napi::Object device = Napi::Object::New(env);
	// 	// 	device.Set("id", devList[i].id);
	// 	// 	device.Set("serial", devList[i].serial_number);
	// 	// 	device.Set("model", sl::toString(devList[i].Pipeline_model).get());
	// 	// 	device.Set("state", sl::toString(devList[i].Pipeline_state).get());
	// 	// 	devices[i] = device;
	// 	// }
	// 	// return devices;
	// }

	// Napi::Value open(const Napi::CallbackInfo& info) {
	// 	Napi::Env env = info.Env();
	// }
	
	Module(Napi::Env env, Napi::Object exports) {
		// See https://github.com/nodejs/node-addon-api/blob/main/doc/class_property_descriptor.md
		DefineAddon(exports, {
			// InstanceMethod("start", &Module::start),
			// InstanceMethod("end", &Module::end),
			// //InstanceMethod("test", &Module::test),

			// // InstanceValue
			// // InstanceAccessor
//			InstanceAccessor<&Module::devices>("devices"),
			// InstanceAccessor<&Module::Gett>("t"),
			// InstanceAccessor<&Module::GetSamplerate>("samplerate"),
		});
		
		// This method is used to hook the accessor and method callbacks
		Napi::Function ctor = Pipeline::DefineClass(env, "Pipeline", {
			Pipeline::InstanceMethod<&Pipeline::start>("start"),
		// 	Pipeline::InstanceMethod<&Pipeline::close>("close"),
		// 	Pipeline::InstanceMethod<&Pipeline::isOpened>("isOpened"),
			Pipeline::InstanceMethod<&Pipeline::grab>("grab"),
			Pipeline::InstanceMethod<&Pipeline::voxels>("voxels"),
			//Pipeline::InstanceMethod<&Pipeline::grab>("get_active_profile"),
		});

		// Create a persistent reference to the class constructor. This will allow
		// a function called on a class prototype and a function
		// called on instance of a class to be distinguished from each other.
		Napi::FunctionReference* constructor = new Napi::FunctionReference();
		*constructor = Napi::Persistent(ctor);
		exports.Set("Pipeline", ctor);
		// Store the constructor as the add-on instance data. This will allow this
		// add-on to support multiple instances of itself running on multiple worker
		// threads, as well as multiple instances of itself running in different
		// contexts on the same thread.
		// By default, the value set on the environment here will be destroyed when
		// the add-on is unloaded using the `delete` operator, but it is also
		// possible to supply a custom deleter.
		env.SetInstanceData<Napi::FunctionReference>(constructor);
	}
};

NODE_API_ADDON(Module)