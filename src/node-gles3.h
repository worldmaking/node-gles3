#include "node-api-helpers.h"
#include <GL/glew.h>

#define BIGSTRLEN 4096*64

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
	printf("glew version %s\n", (char *)glewGetString(GLEW_VERSION));
	napi_value greeting;
	status = napi_create_string_utf8(env, (char *)glGetString(GL_VERSION), NAPI_AUTO_LENGTH, &greeting);
	if (status != napi_ok) return nullptr;

	printf("glversion %s\n", glGetString(GL_VERSION));

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


napi_value GetError(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 0, 0);
	napi_value result_value;
	status = napi_create_uint32(env, (uint32_t)glGetError(), &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value ClearBufferfv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum buffer = getUint32(env, args[0]);
	GLint drawbuffer = getInt32(env, args[1]);

	// args[2] could be either an array or a typed array
	// better to grab the values manually
	GLfloat rgba[4];
	napi_value rgba_value[4];
	for (int i=0; i<4; i++) {
		napi_get_element(env, args[2], i, &rgba_value[i]);
		rgba[i] = getDouble(env, rgba_value[i]);
	}
	glClearBufferfv(buffer, drawbuffer, rgba);
	return NULL;
}

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
	GLchar name[BIGSTRLEN];
	status = napi_get_value_string_utf8(env, args[1], name, bufsize+1, &bufsize);

	GLint result = glGetAttribLocation(program, name);
	napi_value result_value;
	status = napi_create_int32(env, (int32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}


napi_value GetBufferParameteri64v(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint64 data = 0;
	// void glGetBufferParameteri64v(GLenum target, GLenum pname, GLint64 *params)
	glGetBufferParameteri64v(target, pname, &data);
	
	napi_value result_value;
	status = napi_create_int64(env, data, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}


napi_value GetProgramInfoLog(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);

	GLuint program = getUint32(env, args[0]);
	
	GLint bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);	
	GLchar infoLog[BIGSTRLEN];
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
	GLchar infoLog[BIGSTRLEN];
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

	GLchar name[BIGSTRLEN];
	
	
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

// http://docs.gl/es3/glGetShaderPrecisionFormat
// https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getShaderPrecisionFormat
napi_value GetShaderPrecisionFormat(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum shadertype = getUint32(env, args[0]);
	GLenum precisiontype = getUint32(env, args[1]);

	GLint range[2];
	GLint precision[1];
	// void glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision)
	glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);

	napi_value ret = nullptr;
	if (napi_ok == napi_create_object(env, &ret)) {
		napi_value vals[3];
		napi_create_int32(env, (uint32_t)range[0], &vals[0]);
		napi_create_int32(env, (uint32_t)range[1], &vals[1]);
		napi_create_int32(env, (uint32_t)precision[0], &vals[2]);

		napi_set_named_property(env, ret, "rangeMin", vals[0]);
		napi_set_named_property(env, ret, "rangeMax", vals[1]);
		napi_set_named_property(env, ret, "precision", vals[2]);
	}
	return ret;
}

napi_value ClearDepth(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLfloat d = getDouble(env, args[0]);
	// void glClearDepthf(GLfloat d)
	glClearDepth(d);
	return NULL;
}

napi_value GetIntegerv(napi_env env, napi_callback_info info) {
	napi_value ret = nullptr;
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum pname = getUint32(env, args[0]);
	GLint data[1];
	// void glGetIntegerv(GLenum pname, GLint *data)
	glGetIntegerv(pname, data);
	napi_create_int32(env, data[0], &ret);
	return ret;
}

napi_value GetIntegeri_v(napi_env env, napi_callback_info info) {
	napi_value ret = nullptr;
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum pname = getUint32(env, args[0]);
	GLuint index = getUint32(env, args[1]);
	GLint data[1];
	// void glGetIntegeri_v(GLenum target, GLuint index, GLint *data)
	glGetIntegeri_v(pname, index, &data[0]);
	napi_create_int32(env, data[0], &ret);
	return ret;
}


napi_value GetInteger64v(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum pname = getUint32(env, args[0]);
	GLint64 data = 0;
	// void glGetInteger64v(GLenum pname, GLint64 *data)
	glGetInteger64v(pname, &data);
	napi_value result_value;
	status = napi_create_int64(env, data, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetInteger64i_v(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLuint index = getUint32(env, args[1]);
	GLint64 data = 0;
	// void glGetInteger64i_v(GLenum target, GLuint index, GLint64 *data)
	glGetInteger64i_v(target, index, &data);
	napi_value result_value;
	status = napi_create_int64(env, data, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}


napi_value GetBooleanv(napi_env env, napi_callback_info info) {
	napi_value ret = nullptr;
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum pname = getUint32(env, args[0]);
	GLboolean data[1];
	// void glGetFloatv(GLenum pname, GLfloat *data)
	glGetBooleanv(pname, data);
	napi_get_boolean(env, data[0], &ret);
	return ret;
}

napi_value GetFloatv(napi_env env, napi_callback_info info) {
	napi_value ret = nullptr;
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum pname = getUint32(env, args[0]);
	GLfloat data[1];
	// void glGetFloatv(GLenum pname, GLfloat *data)
	glGetFloatv(pname, data);
	napi_create_double(env, data[0], &ret);
	return ret;
}
