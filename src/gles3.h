#include <node_api.h>
//#include <GLES3/gl3.h>
#include <GL/glew.h>

#include <stdio.h>
#include <string>

#define BIGSTRINGLEN 4096*64

size_t checkArgCount(napi_env env, napi_callback_info info, napi_value * args, size_t max, size_t min=0) {
	size_t argc = max;
	napi_status status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
	if(status != napi_ok || argc < min) {
		napi_throw_type_error(env, nullptr, "Missing arguments");
	}
	return argc;
}

template<typename T> bool isTypedArrayType(napi_typedarray_type ty);

template<> bool isTypedArrayType<char>(napi_typedarray_type ty) { return ty == napi_int8_array; }
template<> bool isTypedArrayType<int8_t>(napi_typedarray_type ty) { return ty == napi_int8_array; }
template<> bool isTypedArrayType<uint8_t>(napi_typedarray_type ty) { return ty == napi_uint8_array; }
template<> bool isTypedArrayType<int16_t>(napi_typedarray_type ty) { return ty == napi_int16_array; }
template<> bool isTypedArrayType<uint16_t>(napi_typedarray_type ty) { return ty == napi_uint16_array; }
template<> bool isTypedArrayType<int32_t>(napi_typedarray_type ty) { return ty == napi_int32_array; }
template<> bool isTypedArrayType<uint32_t>(napi_typedarray_type ty) { return ty == napi_uint32_array; }
// template<> bool isTypedArrayType<int64_t>(napi_typedarray_type ty) { return ty == napi_bigint64_array; }
// template<> bool isTypedArrayType<uint64_t>(napi_typedarray_type ty) { return ty == napi_biguint64_array; }
template<> bool isTypedArrayType<float>(napi_typedarray_type ty) { return ty == napi_float32_array; }
template<> bool isTypedArrayType<double>(napi_typedarray_type ty) { return ty == napi_float64_array; }
template<> bool isTypedArrayType<void>(napi_typedarray_type ty) { return true; }

size_t typedArrayElementSize(napi_typedarray_type ty) {
	switch(ty) {
		case napi_int8_array:
		case napi_uint8_array: return 1;
		case napi_int16_array:
		case napi_uint16_array: return 2;
		case napi_int32_array:
		case napi_uint32_array: 
		case napi_float32_array: return 4;
		case napi_float64_array: return 8;
		default: return 0;
	}
}

napi_status getPointerAndSize(napi_env env, napi_value &arg, void *& data, size_t &size) {
	napi_valuetype valuetype;
	napi_status status = napi_typeof(env, arg, &valuetype);
	bool is_typedarray=0, is_arraybuffer=0, is_dataview=0, is_external = (valuetype == napi_external);
	napi_is_typedarray(env, arg, &is_typedarray) == napi_ok &&
	napi_is_arraybuffer(env, arg, &is_arraybuffer) == napi_ok &&
	napi_is_dataview(env, arg, &is_dataview);
	if (is_typedarray) {
		napi_typedarray_type value_typedarray_type;
		status = napi_get_typedarray_info(env, arg, &value_typedarray_type, &size, (void **)&data, nullptr, nullptr);
		size *= typedArrayElementSize(value_typedarray_type);
	} else if (is_arraybuffer) {
		status = napi_get_arraybuffer_info(env, arg, (void **)&data, &size);
		if (status != napi_ok ) {
			napi_throw_type_error(env, nullptr, "Failed to read argument as arraybuffer");
		} 
	} else if (is_dataview) {
		status = napi_get_dataview_info(env, arg, &size, (void **)&data, nullptr, nullptr);
		if (status != napi_ok ) {
			napi_throw_type_error(env, nullptr, "Failed to read argument as dataview");
		} 
	} else {
		data = nullptr;
		size = 0;
	}
	return status;
}

template<typename T> 
napi_status getTypedArray(napi_env env, napi_value &arg, T * &value) {
	napi_valuetype valuetype;
	napi_status status = napi_typeof(env, arg, &valuetype);
	bool is_typedarray=0, is_arraybuffer=0, is_dataview=0, is_external = (valuetype == napi_external);
	napi_is_typedarray(env, arg, &is_typedarray) == napi_ok &&
	napi_is_arraybuffer(env, arg, &is_arraybuffer) == napi_ok &&
	napi_is_dataview(env, arg, &is_dataview);
	if (is_typedarray) {
		napi_typedarray_type value_typedarray_type;
		status = napi_get_typedarray_info(env, arg, &value_typedarray_type, nullptr, (void **)&value, nullptr, nullptr);
		if (status != napi_ok || !isTypedArrayType<T>(value_typedarray_type)) {
			napi_throw_type_error(env, nullptr, "Wrong type for typed array");
		}
	} else if (is_arraybuffer) {
		status = napi_get_arraybuffer_info(env, arg, (void **)&value, nullptr);
		if (status != napi_ok ) {
			napi_throw_type_error(env, nullptr, "Failed to read argument as arraybuffer");
		} 
	} else if (is_dataview) {
		status = napi_get_dataview_info(env, arg, nullptr, (void **)&value, nullptr, nullptr);
		if (status != napi_ok ) {
			napi_throw_type_error(env, nullptr, "Failed to read argument as dataview");
		} 
	} else if (is_external) {
		status = napi_get_value_external(env, arg, (void **)&value);
		if (status != napi_ok ) {
			napi_throw_type_error(env, nullptr, "Failed to read argument as external");
		} 
	} else {
		value = nullptr;
	}
	return status;
}

napi_status getCharacterArray(napi_env env, napi_value &arg, char *& buf) {
	napi_status status = napi_ok;
	napi_valuetype valuetype;
	status = napi_typeof(env, arg, &valuetype);
	if (status == napi_ok && valuetype == napi_string) {
		size_t len;
		status = napi_get_value_string_utf8(env, arg, nullptr, 0, &len);
		// allocate string buffer:
		napi_value ab;
		status = napi_create_arraybuffer(env, len, (void **)&buf, &ab);
		status = napi_get_value_string_utf8(env, arg, buf, len+1, &len);
	} else {
		status = getTypedArray(env, arg, buf);
	}
	return status;
}

napi_status getCharacterArray(napi_env env, napi_value &arg, char *& buf, size_t &len) {
	napi_status status = napi_ok;
	napi_valuetype valuetype;
	status = napi_typeof(env, arg, &valuetype);
	if (status == napi_ok && valuetype == napi_string) {
		status = napi_get_value_string_utf8(env, arg, nullptr, 0, &len);
		// allocate string buffer:
		napi_value ab;
		status = napi_create_arraybuffer(env, len, (void **)&buf, &ab);
		status = napi_get_value_string_utf8(env, arg, buf, len+1, &len);
	} else {
		napi_typedarray_type value_typedarray_type;
		status = napi_get_typedarray_info(env, arg, &value_typedarray_type, &len, (void **)&buf, nullptr, nullptr);
		if (status != napi_ok || !isTypedArrayType<int8_t>(value_typedarray_type)) {
			napi_throw_type_error(env, nullptr, "Wrong type for typed array");
		}
	}
	return status;
}

napi_status getListOfStrings(napi_env env, napi_value &arg, char **& strings) {
	napi_status status = napi_ok;
	bool isArray = false;
	status =  napi_is_array(env, arg, &isArray);
	if (status == napi_ok && isArray) {
		uint32_t len;
		status = napi_get_array_length(env, arg, &len);
		// need to construct that char**
		napi_value ab;
		status = napi_create_arraybuffer(env, len, (void **)&strings, &ab);
		for (uint32_t i=0; i<len; i++) {
			// get array item at i
			napi_value item;
			status = napi_get_element(env, arg, i, &item);
			status = getCharacterArray(env, item, strings[i]);
		}
	} else {
		napi_throw_type_error(env, nullptr, "Expected an array of strings");
	}
	return status;
}

double getDouble(napi_env env, napi_value &arg) {
	napi_valuetype ty;
	napi_status status = napi_typeof(env, arg, &ty);
	if (ty == napi_undefined || ty == napi_null) return 0.0;
	double v = 0;
	napi_value coerced;
	status = napi_coerce_to_number(env, arg, &coerced);
	if (status == napi_ok) status = napi_get_value_double(env, arg, &v);
	if (status != napi_ok) napi_throw_type_error(env, nullptr, "Expected number");
	return v;
}

uint32_t getUint32(napi_env env, napi_value &arg) {
	napi_valuetype ty;
	napi_status status = napi_typeof(env, arg, &ty);
	if (ty == napi_undefined || ty == napi_null) return 0;
	uint32_t v = 0;
	napi_value coerced;
	status = napi_coerce_to_number(env, arg, &coerced);
	if (status == napi_ok) status = napi_get_value_uint32(env, arg, &v);
	if (status != napi_ok) napi_throw_type_error(env, nullptr, "Expected integer");
	return v;
}

int32_t getInt32(napi_env env, napi_value &arg) {
	napi_valuetype ty;
	napi_status status = napi_typeof(env, arg, &ty);
	if (ty == napi_undefined || ty == napi_null) return 0;
	int32_t v = 0;
	napi_value coerced;
	status = napi_coerce_to_number(env, arg, &coerced);
	if (status == napi_ok) status = napi_get_value_int32(env, arg, &v);
	if (status != napi_ok) napi_throw_type_error(env, nullptr, "Expected integer");
	return v;
}

bool getBool(napi_env env, napi_value &arg) {
	napi_valuetype ty;
	napi_status status = napi_typeof(env, arg, &ty);
	if (ty == napi_undefined || ty == napi_null) return false;
	bool v = 0;
	napi_value coerced;
	status = napi_coerce_to_bool(env, arg, &coerced);
	if (status == napi_ok) status = napi_get_value_bool(env, coerced, &v);
	if (status != napi_ok) napi_throw_type_error(env, nullptr, "Expected boolean");
	return v;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

napi_value glewInit(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		napi_throw_type_error(env, nullptr, (char *)glewGetErrorString(err));
		return nullptr;
	}
	napi_value greeting;
	status = napi_create_string_utf8(env, (char *)glewGetString(GLEW_VERSION), NAPI_AUTO_LENGTH, &greeting);
	if (status != napi_ok) return nullptr;


	// TODO: why is this needed?
	glEnable(GL_PROGRAM_POINT_SIZE);
	return greeting;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

/*
// WebGL1: 
void gl.bufferData(target, size, usage); 
void gl.bufferData(target, ArrayBuffer? srcData, usage); 
void gl.bufferData(target, ArrayBufferView srcData, usage); 

// TODO WebGL2: 
void gl.bufferData(target, ArrayBufferView srcData, usage, srcOffset, length);
*/
napi_value BufferData(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 2);

	GLenum target = getUint32(env, args[0]);
	napi_valuetype args1type;
	status = napi_typeof(env, args[1], &args1type);
	if (argc == 2) {
		//void gl.bufferData(target, usage); 
		GLenum usage = getUint32(env, args[1]);
		glBufferData(target, 0, nullptr, usage);
	} else if (argc == 3 && args1type == napi_number) {
		// void gl.bufferData(target, size, usage); 
		GLsizeiptr size = getUint32(env, args[1]);
		GLenum usage = getUint32(env, args[2]);
		glBufferData(target, size, nullptr, usage);
	} else {
		//void gl.bufferData(target, ArrayBuffer srcData, usage);
		void * data;
		size_t size;
		getPointerAndSize(env, args[1], data, size);
		GLenum usage = getUint32(env, args[2]);
		glBufferData(target, size, data, usage);
	}
	return nullptr;
}

napi_value CreateBuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	GLuint result;
	glGenBuffers(1, &result);
	napi_value result_value;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value CreateFramebuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	GLuint result;
	glGenFramebuffers(1, &result);
	napi_value result_value;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value CreateProgram(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	GLuint result = glCreateProgram();
	napi_value result_value;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value CreateRenderbuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	GLuint result;
	glGenRenderbuffers(1, &result);
	napi_value result_value;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value CreateTexture(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	GLuint result;
	glGenTextures(1, &result);
	napi_value result_value;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value CreateVertexArray(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	GLuint result;
	glGenVertexArrays(1, &result);
	napi_value result_value;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetAttribLocation(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);

	status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
	if(status != napi_ok) return nullptr;
	if (argc < 2) {
		napi_throw_type_error(env, nullptr, "Wrong number of arguments");
		return nullptr;
	}
	GLuint program = getUint32(env, args[0]);
	
	size_t bufsize;
	status = napi_get_value_string_utf8(env, args[1], nullptr, 0, &bufsize);
	GLchar name[BIGSTRINGLEN];
	status = napi_get_value_string_utf8(env, args[1], name, bufsize+1, &bufsize);

	GLint result = glGetAttribLocation(program, name);
	napi_value result_value;
	status = napi_create_int32(env, (int32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetProgramInfoLog(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);

	GLuint program = getUint32(env, args[0]);
	
	GLint bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);	
	GLchar infoLog[BIGSTRINGLEN];
	GLsizei length_result = bufSize;
	glGetProgramInfoLog(program, length_result, &length_result, infoLog);

	napi_value result_value;
	status = napi_create_string_utf8(env, infoLog, length_result, &result_value);
	return (status == napi_ok) ? result_value : nullptr; 
}

napi_value GetProgramParameter(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);

	GLuint program = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);

	GLint value;
	glGetProgramiv(program, pname, &value);

	napi_value result_value;
	status = napi_create_int32(env, value, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetShaderInfoLog(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);

	GLuint shader = getUint32(env, args[0]);
	
	GLint bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);
	GLchar infoLog[BIGSTRINGLEN];
	GLsizei length_result = bufSize;
	glGetShaderInfoLog(shader, length_result, &length_result, infoLog);

	napi_value result_value;
	status = napi_create_string_utf8(env, infoLog, length_result, &result_value);
	return (status == napi_ok) ? result_value : nullptr; 
}

napi_value GetShaderParameter(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);

	GLuint shader = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);

	GLint value;
	glGetShaderiv(shader, pname, &value);

	napi_value result_value;
	status = napi_create_int32(env, value, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetUniformLocation(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);

	GLuint program = getUint32(env, args[0]);
	
	size_t bufsize;
	status = napi_get_value_string_utf8(env, args[1], nullptr, 0, &bufsize);
	GLchar name[BIGSTRINGLEN];
	status = napi_get_value_string_utf8(env, args[1], name, bufsize+1, &bufsize);

	GLint result = glGetUniformLocation(program, name);
	napi_value result_value;
	status = napi_create_int32(env, (int32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value ShaderSource(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);

	GLuint shader = getUint32(env, args[0]);

	char * buf;
	size_t len;
	getCharacterArray(env, args[1], buf, len);

	const char* codes[1];
	codes[0] = buf;
	GLint size = len;
	glShaderSource(shader, 1, codes, &size);
	return nullptr;
}

// webgl: WebGLRenderingContext.uniformMatrix3fv(location, transpose, value);
napi_value UniformMatrix2fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	
	GLint location = getInt32(env, args[0]);
	// TODO: if (args[2]) is an array of typed arrays, we could have count > 1
	GLsizei count = 1;
	GLboolean transpose = getBool(env, args[1]);

	GLfloat * value = nullptr;
	status = getTypedArray(env, args[2], value);
	glUniformMatrix2fv(location, count, transpose, value);
	return nullptr;
}

// webgl: WebGLRenderingContext.uniformMatrix3fv(location, transpose, value);
napi_value UniformMatrix3fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	
	GLint location = getInt32(env, args[0]);
	// TODO: if (args[2]) is an array of typed arrays, we could have count > 1
	GLsizei count = 1;
	GLboolean transpose = getBool(env, args[1]);

	GLfloat * value = nullptr;
	status = getTypedArray(env, args[2], value);
	glUniformMatrix3fv(location, count, transpose, value);
	return nullptr;
}

// webgl: WebGLRenderingContext.uniformMatrix3fv(location, transpose, value);
napi_value UniformMatrix4fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	
	GLint location = getInt32(env, args[0]);
	// TODO: if (args[2]) is an array of typed arrays, we could have count > 1
	GLsizei count = 1;
	GLboolean transpose = getBool(env, args[1]);
	
	GLfloat * value = nullptr;
	status = getTypedArray(env, args[2], value);

	glUniformMatrix4fv(location, count, transpose, value);
	return nullptr;
}

// attrLoc, components, type, normalize, stride, offset
napi_value VertexAttribPointer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[6];
	size_t argc = checkArgCount(env, info, args, 6, 6);
	
	GLuint index = getUint32(env, args[0]);
	GLint components = getInt32(env, args[1]);
	GLenum type = getUint32(env, args[2]);
	GLboolean normalized = getBool(env, args[3]);
	GLsizei stride = getInt32(env, args[4]);
	GLsizei offset = getInt32(env, args[5]);

	//console.log("attrib", index, components, type, normalied stride, offset)

	glVertexAttribPointer(index, components, type, normalized, stride, reinterpret_cast<const void*>(offset));
	return NULL;
}