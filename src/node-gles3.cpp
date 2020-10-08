/* THIS IS A GENERATED FILE -- DO NOT EDIT!! */

#include "node-gles3.h"

napi_value ActiveTexture(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum texture = getUint32(env, args[0]);
	// void glActiveTexture(GLenum texture)
	glActiveTexture(texture);
	return NULL;
}

napi_value AttachShader(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLuint program = getUint32(env, args[0]);
	GLuint shader = getUint32(env, args[1]);
	// void glAttachShader(GLuint program, GLuint shader)
	glAttachShader(program, shader);
	return NULL;
}

napi_value BindAttribLocation(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint program = getUint32(env, args[0]);
	GLuint index = getUint32(env, args[1]);
	GLchar * name = nullptr;
	status = getCharacterArray(env, args[2], name);
	// void glBindAttribLocation(GLuint program, GLuint index, const GLchar *name)
	glBindAttribLocation(program, index, name);
	return NULL;
}

napi_value BindBuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum target = getUint32(env, args[0]);
	GLuint buffer = getUint32(env, args[1]);
	// void glBindBuffer(GLenum target, GLuint buffer)
	glBindBuffer(target, buffer);
	return NULL;
}

napi_value BindFramebuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum target = getUint32(env, args[0]);
	GLuint framebuffer = getUint32(env, args[1]);
	// void glBindFramebuffer(GLenum target, GLuint framebuffer)
	glBindFramebuffer(target, framebuffer);
	return NULL;
}

napi_value BindRenderbuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum target = getUint32(env, args[0]);
	GLuint renderbuffer = getUint32(env, args[1]);
	// void glBindRenderbuffer(GLenum target, GLuint renderbuffer)
	glBindRenderbuffer(target, renderbuffer);
	return NULL;
}

napi_value BindTexture(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum target = getUint32(env, args[0]);
	GLuint texture = getUint32(env, args[1]);
	// void glBindTexture(GLenum target, GLuint texture)
	glBindTexture(target, texture);
	return NULL;
}

napi_value BlendColor(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLfloat red = getDouble(env, args[0]);
	GLfloat green = getDouble(env, args[1]);
	GLfloat blue = getDouble(env, args[2]);
	GLfloat alpha = getDouble(env, args[3]);
	// void glBlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
	glBlendColor(red, green, blue, alpha);
	return NULL;
}

napi_value BlendEquation(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum mode = getUint32(env, args[0]);
	// void glBlendEquation(GLenum mode)
	glBlendEquation(mode);
	return NULL;
}

napi_value BlendEquationSeparate(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum modeRGB = getUint32(env, args[0]);
	GLenum modeAlpha = getUint32(env, args[1]);
	// void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
	glBlendEquationSeparate(modeRGB, modeAlpha);
	return NULL;
}

napi_value BlendFunc(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum sfactor = getUint32(env, args[0]);
	GLenum dfactor = getUint32(env, args[1]);
	// void glBlendFunc(GLenum sfactor, GLenum dfactor)
	glBlendFunc(sfactor, dfactor);
	return NULL;
}

napi_value BlendFuncSeparate(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLenum sfactorRGB = getUint32(env, args[0]);
	GLenum dfactorRGB = getUint32(env, args[1]);
	GLenum sfactorAlpha = getUint32(env, args[2]);
	GLenum dfactorAlpha = getUint32(env, args[3]);
	// void glBlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
	glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
	return NULL;
}

napi_value BufferSubData(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLenum target = getUint32(env, args[0]);
	GLintptr offset = getInt32(env, args[1]);
	GLsizeiptr size = getUint32(env, args[2]);
	void * data = nullptr;
	status = getTypedArray(env, args[3], *(void **)&data);
	if (status != napi_ok) return nullptr;
	// void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
	glBufferSubData(target, offset, size, data);
	return NULL;
}

napi_value CheckFramebufferStatus(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum target = getUint32(env, args[0]);
	// GLenum glCheckFramebufferStatus(GLenum target)
	GLenum result = glCheckFramebufferStatus(target);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value Clear(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLbitfield mask = getUint32(env, args[0]);
	// void glClear(GLbitfield mask)
	glClear(mask);
	return NULL;
}

napi_value ClearColor(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLfloat red = getDouble(env, args[0]);
	GLfloat green = getDouble(env, args[1]);
	GLfloat blue = getDouble(env, args[2]);
	GLfloat alpha = getDouble(env, args[3]);
	// void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
	glClearColor(red, green, blue, alpha);
	return NULL;
}

napi_value ClearDepthf(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLfloat d = getDouble(env, args[0]);
	// void glClearDepthf(GLfloat d)
	glClearDepthf(d);
	return NULL;
}

napi_value ClearStencil(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLint s = getInt32(env, args[0]);
	// void glClearStencil(GLint s)
	glClearStencil(s);
	return NULL;
}

napi_value ColorMask(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLboolean red = getBool(env, args[0]);
	GLboolean green = getBool(env, args[1]);
	GLboolean blue = getBool(env, args[2]);
	GLboolean alpha = getBool(env, args[3]);
	// void glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
	glColorMask(red, green, blue, alpha);
	return NULL;
}

napi_value CompileShader(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint shader = getUint32(env, args[0]);
	// void glCompileShader(GLuint shader)
	glCompileShader(shader);
	return NULL;
}

napi_value CompressedTexImage2D(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[8];
	size_t argc = checkArgCount(env, info, args, 8, 8);
	GLenum target = getUint32(env, args[0]);
	GLint level = getInt32(env, args[1]);
	GLenum internalformat = getUint32(env, args[2]);
	GLsizei width = getInt32(env, args[3]);
	GLsizei height = getInt32(env, args[4]);
	GLint border = getInt32(env, args[5]);
	GLsizei imageSize = getInt32(env, args[6]);
	void * data = nullptr;
	status = getTypedArray(env, args[7], *(void **)&data);
	if (status != napi_ok) return nullptr;
	// void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data)
	glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
	return NULL;
}

napi_value CompressedTexSubImage2D(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[9];
	size_t argc = checkArgCount(env, info, args, 9, 9);
	GLenum target = getUint32(env, args[0]);
	GLint level = getInt32(env, args[1]);
	GLint xoffset = getInt32(env, args[2]);
	GLint yoffset = getInt32(env, args[3]);
	GLsizei width = getInt32(env, args[4]);
	GLsizei height = getInt32(env, args[5]);
	GLenum format = getUint32(env, args[6]);
	GLsizei imageSize = getInt32(env, args[7]);
	void * data = nullptr;
	status = getTypedArray(env, args[8], *(void **)&data);
	if (status != napi_ok) return nullptr;
	// void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data)
	glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
	return NULL;
}

napi_value CopyTexImage2D(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[8];
	size_t argc = checkArgCount(env, info, args, 8, 8);
	GLenum target = getUint32(env, args[0]);
	GLint level = getInt32(env, args[1]);
	GLenum internalformat = getUint32(env, args[2]);
	GLint x = getInt32(env, args[3]);
	GLint y = getInt32(env, args[4]);
	GLsizei width = getInt32(env, args[5]);
	GLsizei height = getInt32(env, args[6]);
	GLint border = getInt32(env, args[7]);
	// void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
	glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
	return NULL;
}

napi_value CopyTexSubImage2D(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[8];
	size_t argc = checkArgCount(env, info, args, 8, 8);
	GLenum target = getUint32(env, args[0]);
	GLint level = getInt32(env, args[1]);
	GLint xoffset = getInt32(env, args[2]);
	GLint yoffset = getInt32(env, args[3]);
	GLint x = getInt32(env, args[4]);
	GLint y = getInt32(env, args[5]);
	GLsizei width = getInt32(env, args[6]);
	GLsizei height = getInt32(env, args[7]);
	// void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
	glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
	return NULL;
}

napi_value CreateShader(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum type = getUint32(env, args[0]);
	// GLuint glCreateShader(GLenum type)
	GLuint result = glCreateShader(type);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value CullFace(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum mode = getUint32(env, args[0]);
	// void glCullFace(GLenum mode)
	glCullFace(mode);
	return NULL;
}

napi_value DeleteBuffers(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLuint * buffers = nullptr;
	status = getTypedArray(env, args[1], buffers);
	// void glDeleteBuffers(GLsizei n, const GLuint *buffers)
	glDeleteBuffers(n, buffers);
	return NULL;
}

napi_value DeleteFramebuffers(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLuint * framebuffers = nullptr;
	status = getTypedArray(env, args[1], framebuffers);
	// void glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
	glDeleteFramebuffers(n, framebuffers);
	return NULL;
}

napi_value DeleteProgram(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint program = getUint32(env, args[0]);
	// void glDeleteProgram(GLuint program)
	glDeleteProgram(program);
	return NULL;
}

napi_value DeleteRenderbuffers(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLuint * renderbuffers = nullptr;
	status = getTypedArray(env, args[1], renderbuffers);
	// void glDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
	glDeleteRenderbuffers(n, renderbuffers);
	return NULL;
}

napi_value DeleteShader(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint shader = getUint32(env, args[0]);
	// void glDeleteShader(GLuint shader)
	glDeleteShader(shader);
	return NULL;
}

napi_value DeleteTextures(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLuint * textures = nullptr;
	status = getTypedArray(env, args[1], textures);
	// void glDeleteTextures(GLsizei n, const GLuint *textures)
	glDeleteTextures(n, textures);
	return NULL;
}

napi_value DepthFunc(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum func = getUint32(env, args[0]);
	// void glDepthFunc(GLenum func)
	glDepthFunc(func);
	return NULL;
}

napi_value DepthMask(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLboolean flag = getBool(env, args[0]);
	// void glDepthMask(GLboolean flag)
	glDepthMask(flag);
	return NULL;
}

napi_value DepthRangef(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLfloat n = getDouble(env, args[0]);
	GLfloat f = getDouble(env, args[1]);
	// void glDepthRangef(GLfloat n, GLfloat f)
	glDepthRangef(n, f);
	return NULL;
}

napi_value DetachShader(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLuint program = getUint32(env, args[0]);
	GLuint shader = getUint32(env, args[1]);
	// void glDetachShader(GLuint program, GLuint shader)
	glDetachShader(program, shader);
	return NULL;
}

napi_value Disable(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum cap = getUint32(env, args[0]);
	// void glDisable(GLenum cap)
	glDisable(cap);
	return NULL;
}

napi_value DisableVertexAttribArray(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint index = getUint32(env, args[0]);
	// void glDisableVertexAttribArray(GLuint index)
	glDisableVertexAttribArray(index);
	return NULL;
}

napi_value DrawArrays(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum mode = getUint32(env, args[0]);
	GLint first = getInt32(env, args[1]);
	GLsizei count = getInt32(env, args[2]);
	// void glDrawArrays(GLenum mode, GLint first, GLsizei count)
	glDrawArrays(mode, first, count);
	return NULL;
}

napi_value DrawElements(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLenum mode = getUint32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLenum type = getUint32(env, args[2]);
	void * indices = nullptr;
	status = getTypedArray(env, args[3], *(void **)&indices);
	if (status != napi_ok) return nullptr;
	// void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices)
	glDrawElements(mode, count, type, indices);
	return NULL;
}

napi_value Enable(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum cap = getUint32(env, args[0]);
	// void glEnable(GLenum cap)
	glEnable(cap);
	return NULL;
}

napi_value EnableVertexAttribArray(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint index = getUint32(env, args[0]);
	// void glEnableVertexAttribArray(GLuint index)
	glEnableVertexAttribArray(index);
	return NULL;
}

napi_value Finish(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// void glFinish(void)
	glFinish();
	return NULL;
}

napi_value Flush(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// void glFlush(void)
	glFlush();
	return NULL;
}

napi_value FramebufferRenderbuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLenum target = getUint32(env, args[0]);
	GLenum attachment = getUint32(env, args[1]);
	GLenum renderbuffertarget = getUint32(env, args[2]);
	GLuint renderbuffer = getUint32(env, args[3]);
	// void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
	glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
	return NULL;
}

napi_value FramebufferTexture2D(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLenum target = getUint32(env, args[0]);
	GLenum attachment = getUint32(env, args[1]);
	GLenum textarget = getUint32(env, args[2]);
	GLuint texture = getUint32(env, args[3]);
	GLint level = getInt32(env, args[4]);
	// void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
	glFramebufferTexture2D(target, attachment, textarget, texture, level);
	return NULL;
}

napi_value FrontFace(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum mode = getUint32(env, args[0]);
	// void glFrontFace(GLenum mode)
	glFrontFace(mode);
	return NULL;
}

napi_value GenBuffers(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLuint * buffers = nullptr;
	status = getTypedArray(env, args[1], buffers);
	// void glGenBuffers(GLsizei n, GLuint *buffers)
	glGenBuffers(n, buffers);
	return NULL;
}

napi_value GenerateMipmap(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum target = getUint32(env, args[0]);
	// void glGenerateMipmap(GLenum target)
	glGenerateMipmap(target);
	return NULL;
}

napi_value GenFramebuffers(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLuint * framebuffers = nullptr;
	status = getTypedArray(env, args[1], framebuffers);
	// void glGenFramebuffers(GLsizei n, GLuint *framebuffers)
	glGenFramebuffers(n, framebuffers);
	return NULL;
}

napi_value GenRenderbuffers(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLuint * renderbuffers = nullptr;
	status = getTypedArray(env, args[1], renderbuffers);
	// void glGenRenderbuffers(GLsizei n, GLuint *renderbuffers)
	glGenRenderbuffers(n, renderbuffers);
	return NULL;
}

napi_value GenTextures(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLuint * textures = nullptr;
	status = getTypedArray(env, args[1], textures);
	// void glGenTextures(GLsizei n, GLuint *textures)
	glGenTextures(n, textures);
	return NULL;
}

napi_value GetActiveAttrib(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[7];
	size_t argc = checkArgCount(env, info, args, 7, 7);
	GLuint program = getUint32(env, args[0]);
	GLuint index = getUint32(env, args[1]);
	GLsizei bufSize = getInt32(env, args[2]);
	GLsizei length_result;
	GLsizei * length = &length_result;
	GLint size_result;
	GLint * size = &size_result;
	GLenum type_result;
	GLenum * type = &type_result;
	GLchar * name;
	status = getTypedArray(env, args[6], name);
	// void glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
	glGetActiveAttrib(program, index, bufSize, length, size, type, name);
	return NULL;
}

napi_value GetActiveUniform(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[7];
	size_t argc = checkArgCount(env, info, args, 7, 7);
	GLuint program = getUint32(env, args[0]);
	GLuint index = getUint32(env, args[1]);
	GLsizei bufSize = getInt32(env, args[2]);
	GLsizei length_result;
	GLsizei * length = &length_result;
	GLint size_result;
	GLint * size = &size_result;
	GLenum type_result;
	GLenum * type = &type_result;
	GLchar * name;
	status = getTypedArray(env, args[6], name);
	// void glGetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name)
	glGetActiveUniform(program, index, bufSize, length, size, type, name);
	return NULL;
}

napi_value GetAttachedShaders(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLuint program = getUint32(env, args[0]);
	GLsizei maxCount = getInt32(env, args[1]);
	GLsizei count_result;
	GLsizei * count = &count_result;
	GLuint shaders_result;
	GLuint * shaders = &shaders_result;
	// void glGetAttachedShaders(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders)
	glGetAttachedShaders(program, maxCount, count, shaders);
	return NULL;
}

napi_value GetBufferParameteriv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint params_result;
	GLint * params = &params_result;
	// void glGetBufferParameteriv(GLenum target, GLenum pname, GLint *params)
	glGetBufferParameteriv(target, pname, params);
	return NULL;
}

napi_value GetFramebufferAttachmentParameteriv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLenum target = getUint32(env, args[0]);
	GLenum attachment = getUint32(env, args[1]);
	GLenum pname = getUint32(env, args[2]);
	GLint params_result;
	GLint * params = &params_result;
	// void glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params)
	glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
	return NULL;
}

napi_value GetProgramiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint program = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint params_result;
	GLint * params = &params_result;
	// void glGetProgramiv(GLuint program, GLenum pname, GLint *params)
	glGetProgramiv(program, pname, params);
	return NULL;
}

napi_value GetRenderbufferParameteriv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint params_result;
	GLint * params = &params_result;
	// void glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint *params)
	glGetRenderbufferParameteriv(target, pname, params);
	return NULL;
}

napi_value GetShaderiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint shader = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint params_result;
	GLint * params = &params_result;
	// void glGetShaderiv(GLuint shader, GLenum pname, GLint *params)
	glGetShaderiv(shader, pname, params);
	return NULL;
}

napi_value GetShaderSource(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLuint shader = getUint32(env, args[0]);
	GLsizei bufSize = getInt32(env, args[1]);
	GLsizei length_result;
	GLsizei * length = &length_result;
	GLchar * source;
	status = getTypedArray(env, args[3], source);
	// void glGetShaderSource(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source)
	glGetShaderSource(shader, bufSize, length, source);
	return NULL;
}

napi_value GetTexParameterfv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLfloat params_result;
	GLfloat * params = &params_result;
	// void glGetTexParameterfv(GLenum target, GLenum pname, GLfloat *params)
	glGetTexParameterfv(target, pname, params);
	return NULL;
}

napi_value GetTexParameteriv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint params_result;
	GLint * params = &params_result;
	// void glGetTexParameteriv(GLenum target, GLenum pname, GLint *params)
	glGetTexParameteriv(target, pname, params);
	return NULL;
}

napi_value GetUniformfv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint program = getUint32(env, args[0]);
	GLint location = getInt32(env, args[1]);
	GLfloat params_result;
	GLfloat * params = &params_result;
	// void glGetUniformfv(GLuint program, GLint location, GLfloat *params)
	glGetUniformfv(program, location, params);
	return NULL;
}

napi_value GetUniformiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint program = getUint32(env, args[0]);
	GLint location = getInt32(env, args[1]);
	GLint params_result;
	GLint * params = &params_result;
	// void glGetUniformiv(GLuint program, GLint location, GLint *params)
	glGetUniformiv(program, location, params);
	return NULL;
}

napi_value GetVertexAttribfv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint index = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLfloat params_result;
	GLfloat * params = &params_result;
	// void glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat *params)
	glGetVertexAttribfv(index, pname, params);
	return NULL;
}

napi_value GetVertexAttribiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint index = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint params_result;
	GLint * params = &params_result;
	// void glGetVertexAttribiv(GLuint index, GLenum pname, GLint *params)
	glGetVertexAttribiv(index, pname, params);
	return NULL;
}

napi_value GetVertexAttribPointerv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint index = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	void * pointer_result = nullptr;
	void ** pointer = &pointer_result;
	// void glGetVertexAttribPointerv(GLuint index, GLenum pname, void **pointer)
	glGetVertexAttribPointerv(index, pname, pointer);
	return NULL;
}

napi_value Hint(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum target = getUint32(env, args[0]);
	GLenum mode = getUint32(env, args[1]);
	// void glHint(GLenum target, GLenum mode)
	glHint(target, mode);
	return NULL;
}

napi_value IsBuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint buffer = getUint32(env, args[0]);
	// GLboolean glIsBuffer(GLuint buffer)
	GLboolean result = glIsBuffer(buffer);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value IsEnabled(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum cap = getUint32(env, args[0]);
	// GLboolean glIsEnabled(GLenum cap)
	GLboolean result = glIsEnabled(cap);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value IsFramebuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint framebuffer = getUint32(env, args[0]);
	// GLboolean glIsFramebuffer(GLuint framebuffer)
	GLboolean result = glIsFramebuffer(framebuffer);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value IsProgram(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint program = getUint32(env, args[0]);
	// GLboolean glIsProgram(GLuint program)
	GLboolean result = glIsProgram(program);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value IsRenderbuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint renderbuffer = getUint32(env, args[0]);
	// GLboolean glIsRenderbuffer(GLuint renderbuffer)
	GLboolean result = glIsRenderbuffer(renderbuffer);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value IsShader(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint shader = getUint32(env, args[0]);
	// GLboolean glIsShader(GLuint shader)
	GLboolean result = glIsShader(shader);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value IsTexture(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint texture = getUint32(env, args[0]);
	// GLboolean glIsTexture(GLuint texture)
	GLboolean result = glIsTexture(texture);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value LineWidth(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLfloat width = getDouble(env, args[0]);
	// void glLineWidth(GLfloat width)
	glLineWidth(width);
	return NULL;
}

napi_value LinkProgram(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint program = getUint32(env, args[0]);
	// void glLinkProgram(GLuint program)
	glLinkProgram(program);
	return NULL;
}

napi_value PixelStorei(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum pname = getUint32(env, args[0]);
	GLint param = getInt32(env, args[1]);
	// void glPixelStorei(GLenum pname, GLint param)
	glPixelStorei(pname, param);
	return NULL;
}

napi_value PolygonOffset(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLfloat factor = getDouble(env, args[0]);
	GLfloat units = getDouble(env, args[1]);
	// void glPolygonOffset(GLfloat factor, GLfloat units)
	glPolygonOffset(factor, units);
	return NULL;
}

napi_value ReadPixels(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[7];
	size_t argc = checkArgCount(env, info, args, 7, 7);
	GLint x = getInt32(env, args[0]);
	GLint y = getInt32(env, args[1]);
	GLsizei width = getInt32(env, args[2]);
	GLsizei height = getInt32(env, args[3]);
	GLenum format = getUint32(env, args[4]);
	GLenum type = getUint32(env, args[5]);
	void * pixels = nullptr;
	status = getTypedArray(env, args[6], *(void **)&pixels);
	if (status != napi_ok) return nullptr;
	// void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels)
	glReadPixels(x, y, width, height, format, type, pixels);
	return NULL;
}

napi_value ReleaseShaderCompiler(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// void glReleaseShaderCompiler(void)
	glReleaseShaderCompiler();
	return NULL;
}

napi_value RenderbufferStorage(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLenum target = getUint32(env, args[0]);
	GLenum internalformat = getUint32(env, args[1]);
	GLsizei width = getInt32(env, args[2]);
	GLsizei height = getInt32(env, args[3]);
	// void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
	glRenderbufferStorage(target, internalformat, width, height);
	return NULL;
}

napi_value SampleCoverage(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLfloat value = getDouble(env, args[0]);
	GLboolean invert = getBool(env, args[1]);
	// void glSampleCoverage(GLfloat value, GLboolean invert)
	glSampleCoverage(value, invert);
	return NULL;
}

napi_value Scissor(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLint x = getInt32(env, args[0]);
	GLint y = getInt32(env, args[1]);
	GLsizei width = getInt32(env, args[2]);
	GLsizei height = getInt32(env, args[3]);
	// void glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
	glScissor(x, y, width, height);
	return NULL;
}

napi_value ShaderBinary(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLsizei count = getInt32(env, args[0]);
	GLuint * shaders = nullptr;
	status = getTypedArray(env, args[1], shaders);
	GLenum binaryformat = getUint32(env, args[2]);
	void * binary = nullptr;
	status = getTypedArray(env, args[3], *(void **)&binary);
	if (status != napi_ok) return nullptr;
	GLsizei length = getInt32(env, args[4]);
	// void glShaderBinary(GLsizei count, const GLuint *shaders, GLenum binaryformat, const void *binary, GLsizei length)
	glShaderBinary(count, shaders, binaryformat, binary, length);
	return NULL;
}

napi_value StencilFunc(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum func = getUint32(env, args[0]);
	GLint ref = getInt32(env, args[1]);
	GLuint mask = getUint32(env, args[2]);
	// void glStencilFunc(GLenum func, GLint ref, GLuint mask)
	glStencilFunc(func, ref, mask);
	return NULL;
}

napi_value StencilFuncSeparate(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLenum face = getUint32(env, args[0]);
	GLenum func = getUint32(env, args[1]);
	GLint ref = getInt32(env, args[2]);
	GLuint mask = getUint32(env, args[3]);
	// void glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
	glStencilFuncSeparate(face, func, ref, mask);
	return NULL;
}

napi_value StencilMask(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint mask = getUint32(env, args[0]);
	// void glStencilMask(GLuint mask)
	glStencilMask(mask);
	return NULL;
}

napi_value StencilMaskSeparate(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum face = getUint32(env, args[0]);
	GLuint mask = getUint32(env, args[1]);
	// void glStencilMaskSeparate(GLenum face, GLuint mask)
	glStencilMaskSeparate(face, mask);
	return NULL;
}

napi_value StencilOp(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum fail = getUint32(env, args[0]);
	GLenum zfail = getUint32(env, args[1]);
	GLenum zpass = getUint32(env, args[2]);
	// void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
	glStencilOp(fail, zfail, zpass);
	return NULL;
}

napi_value StencilOpSeparate(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLenum face = getUint32(env, args[0]);
	GLenum sfail = getUint32(env, args[1]);
	GLenum dpfail = getUint32(env, args[2]);
	GLenum dppass = getUint32(env, args[3]);
	// void glStencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass)
	glStencilOpSeparate(face, sfail, dpfail, dppass);
	return NULL;
}

napi_value TexImage2D(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[9];
	size_t argc = checkArgCount(env, info, args, 9, 9);
	GLenum target = getUint32(env, args[0]);
	GLint level = getInt32(env, args[1]);
	GLint internalformat = getInt32(env, args[2]);
	GLsizei width = getInt32(env, args[3]);
	GLsizei height = getInt32(env, args[4]);
	GLint border = getInt32(env, args[5]);
	GLenum format = getUint32(env, args[6]);
	GLenum type = getUint32(env, args[7]);
	void * pixels = nullptr;
	status = getTypedArray(env, args[8], *(void **)&pixels);
	if (status != napi_ok) return nullptr;
	// void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels)
	glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	return NULL;
}

napi_value TexParameterf(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLfloat param = getDouble(env, args[2]);
	// void glTexParameterf(GLenum target, GLenum pname, GLfloat param)
	glTexParameterf(target, pname, param);
	return NULL;
}

napi_value TexParameterfv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLfloat * params = nullptr;
	status = getTypedArray(env, args[2], params);
	// void glTexParameterfv(GLenum target, GLenum pname, const GLfloat *params)
	glTexParameterfv(target, pname, params);
	return NULL;
}

napi_value TexParameteri(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint param = getInt32(env, args[2]);
	// void glTexParameteri(GLenum target, GLenum pname, GLint param)
	glTexParameteri(target, pname, param);
	return NULL;
}

napi_value TexParameteriv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint * params = nullptr;
	status = getTypedArray(env, args[2], params);
	// void glTexParameteriv(GLenum target, GLenum pname, const GLint *params)
	glTexParameteriv(target, pname, params);
	return NULL;
}

napi_value TexSubImage2D(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[9];
	size_t argc = checkArgCount(env, info, args, 9, 9);
	GLenum target = getUint32(env, args[0]);
	GLint level = getInt32(env, args[1]);
	GLint xoffset = getInt32(env, args[2]);
	GLint yoffset = getInt32(env, args[3]);
	GLsizei width = getInt32(env, args[4]);
	GLsizei height = getInt32(env, args[5]);
	GLenum format = getUint32(env, args[6]);
	GLenum type = getUint32(env, args[7]);
	void * pixels = nullptr;
	status = getTypedArray(env, args[8], *(void **)&pixels);
	if (status != napi_ok) return nullptr;
	// void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
	glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
	return NULL;
}

napi_value Uniform1f(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLint location = getInt32(env, args[0]);
	GLfloat v0 = getDouble(env, args[1]);
	// void glUniform1f(GLint location, GLfloat v0)
	glUniform1f(location, v0);
	return NULL;
}

napi_value Uniform1fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLfloat * value = nullptr;
	status = getTypedArray(env, args[2], value);
	// void glUniform1fv(GLint location, GLsizei count, const GLfloat *value)
	glUniform1fv(location, count, value);
	return NULL;
}

napi_value Uniform1i(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLint location = getInt32(env, args[0]);
	GLint v0 = getInt32(env, args[1]);
	// void glUniform1i(GLint location, GLint v0)
	glUniform1i(location, v0);
	return NULL;
}

napi_value Uniform1iv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLint * value = nullptr;
	status = getTypedArray(env, args[2], value);
	// void glUniform1iv(GLint location, GLsizei count, const GLint *value)
	glUniform1iv(location, count, value);
	return NULL;
}

napi_value Uniform2f(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLfloat v0 = getDouble(env, args[1]);
	GLfloat v1 = getDouble(env, args[2]);
	// void glUniform2f(GLint location, GLfloat v0, GLfloat v1)
	glUniform2f(location, v0, v1);
	return NULL;
}

napi_value Uniform2fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLfloat * value = nullptr;
	status = getTypedArray(env, args[2], value);
	// void glUniform2fv(GLint location, GLsizei count, const GLfloat *value)
	glUniform2fv(location, count, value);
	return NULL;
}

napi_value Uniform2i(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLint v0 = getInt32(env, args[1]);
	GLint v1 = getInt32(env, args[2]);
	// void glUniform2i(GLint location, GLint v0, GLint v1)
	glUniform2i(location, v0, v1);
	return NULL;
}

napi_value Uniform2iv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLint * value = nullptr;
	status = getTypedArray(env, args[2], value);
	// void glUniform2iv(GLint location, GLsizei count, const GLint *value)
	glUniform2iv(location, count, value);
	return NULL;
}

napi_value Uniform3f(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLint location = getInt32(env, args[0]);
	GLfloat v0 = getDouble(env, args[1]);
	GLfloat v1 = getDouble(env, args[2]);
	GLfloat v2 = getDouble(env, args[3]);
	// void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
	glUniform3f(location, v0, v1, v2);
	return NULL;
}

napi_value Uniform3fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLfloat * value = nullptr;
	status = getTypedArray(env, args[2], value);
	// void glUniform3fv(GLint location, GLsizei count, const GLfloat *value)
	glUniform3fv(location, count, value);
	return NULL;
}

napi_value Uniform3i(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLint location = getInt32(env, args[0]);
	GLint v0 = getInt32(env, args[1]);
	GLint v1 = getInt32(env, args[2]);
	GLint v2 = getInt32(env, args[3]);
	// void glUniform3i(GLint location, GLint v0, GLint v1, GLint v2)
	glUniform3i(location, v0, v1, v2);
	return NULL;
}

napi_value Uniform3iv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLint * value = nullptr;
	status = getTypedArray(env, args[2], value);
	// void glUniform3iv(GLint location, GLsizei count, const GLint *value)
	glUniform3iv(location, count, value);
	return NULL;
}

napi_value Uniform4f(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLint location = getInt32(env, args[0]);
	GLfloat v0 = getDouble(env, args[1]);
	GLfloat v1 = getDouble(env, args[2]);
	GLfloat v2 = getDouble(env, args[3]);
	GLfloat v3 = getDouble(env, args[4]);
	// void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
	glUniform4f(location, v0, v1, v2, v3);
	return NULL;
}

napi_value Uniform4fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLfloat * value = nullptr;
	status = getTypedArray(env, args[2], value);
	// void glUniform4fv(GLint location, GLsizei count, const GLfloat *value)
	glUniform4fv(location, count, value);
	return NULL;
}

napi_value Uniform4i(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLint location = getInt32(env, args[0]);
	GLint v0 = getInt32(env, args[1]);
	GLint v1 = getInt32(env, args[2]);
	GLint v2 = getInt32(env, args[3]);
	GLint v3 = getInt32(env, args[4]);
	// void glUniform4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
	glUniform4i(location, v0, v1, v2, v3);
	return NULL;
}

napi_value Uniform4iv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLint * value = nullptr;
	status = getTypedArray(env, args[2], value);
	// void glUniform4iv(GLint location, GLsizei count, const GLint *value)
	glUniform4iv(location, count, value);
	return NULL;
}

napi_value UseProgram(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint program = getUint32(env, args[0]);
	// void glUseProgram(GLuint program)
	glUseProgram(program);
	return NULL;
}

napi_value ValidateProgram(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint program = getUint32(env, args[0]);
	// void glValidateProgram(GLuint program)
	glValidateProgram(program);
	return NULL;
}

napi_value VertexAttrib1f(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLuint index = getUint32(env, args[0]);
	GLfloat x = getDouble(env, args[1]);
	// void glVertexAttrib1f(GLuint index, GLfloat x)
	glVertexAttrib1f(index, x);
	return NULL;
}

napi_value VertexAttrib1fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLuint index = getUint32(env, args[0]);
	GLfloat * v = nullptr;
	status = getTypedArray(env, args[1], v);
	// void glVertexAttrib1fv(GLuint index, const GLfloat *v)
	glVertexAttrib1fv(index, v);
	return NULL;
}

napi_value VertexAttrib2f(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint index = getUint32(env, args[0]);
	GLfloat x = getDouble(env, args[1]);
	GLfloat y = getDouble(env, args[2]);
	// void glVertexAttrib2f(GLuint index, GLfloat x, GLfloat y)
	glVertexAttrib2f(index, x, y);
	return NULL;
}

napi_value VertexAttrib2fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLuint index = getUint32(env, args[0]);
	GLfloat * v = nullptr;
	status = getTypedArray(env, args[1], v);
	// void glVertexAttrib2fv(GLuint index, const GLfloat *v)
	glVertexAttrib2fv(index, v);
	return NULL;
}

napi_value VertexAttrib3f(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLuint index = getUint32(env, args[0]);
	GLfloat x = getDouble(env, args[1]);
	GLfloat y = getDouble(env, args[2]);
	GLfloat z = getDouble(env, args[3]);
	// void glVertexAttrib3f(GLuint index, GLfloat x, GLfloat y, GLfloat z)
	glVertexAttrib3f(index, x, y, z);
	return NULL;
}

napi_value VertexAttrib3fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLuint index = getUint32(env, args[0]);
	GLfloat * v = nullptr;
	status = getTypedArray(env, args[1], v);
	// void glVertexAttrib3fv(GLuint index, const GLfloat *v)
	glVertexAttrib3fv(index, v);
	return NULL;
}

napi_value VertexAttrib4f(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLuint index = getUint32(env, args[0]);
	GLfloat x = getDouble(env, args[1]);
	GLfloat y = getDouble(env, args[2]);
	GLfloat z = getDouble(env, args[3]);
	GLfloat w = getDouble(env, args[4]);
	// void glVertexAttrib4f(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
	glVertexAttrib4f(index, x, y, z, w);
	return NULL;
}

napi_value VertexAttrib4fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLuint index = getUint32(env, args[0]);
	GLfloat * v = nullptr;
	status = getTypedArray(env, args[1], v);
	// void glVertexAttrib4fv(GLuint index, const GLfloat *v)
	glVertexAttrib4fv(index, v);
	return NULL;
}

napi_value Viewport(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLint x = getInt32(env, args[0]);
	GLint y = getInt32(env, args[1]);
	GLsizei width = getInt32(env, args[2]);
	GLsizei height = getInt32(env, args[3]);
	// void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
	glViewport(x, y, width, height);
	return NULL;
}

napi_value ReadBuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum src = getUint32(env, args[0]);
	// void glReadBuffer(GLenum src)
	glReadBuffer(src);
	return NULL;
}

napi_value DrawRangeElements(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[6];
	size_t argc = checkArgCount(env, info, args, 6, 6);
	GLenum mode = getUint32(env, args[0]);
	GLuint start = getUint32(env, args[1]);
	GLuint end = getUint32(env, args[2]);
	GLsizei count = getInt32(env, args[3]);
	GLenum type = getUint32(env, args[4]);
	void * indices = nullptr;
	status = getTypedArray(env, args[5], *(void **)&indices);
	if (status != napi_ok) return nullptr;
	// void glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
	glDrawRangeElements(mode, start, end, count, type, indices);
	return NULL;
}

napi_value TexImage3D(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[10];
	size_t argc = checkArgCount(env, info, args, 10, 10);
	GLenum target = getUint32(env, args[0]);
	GLint level = getInt32(env, args[1]);
	GLint internalformat = getInt32(env, args[2]);
	GLsizei width = getInt32(env, args[3]);
	GLsizei height = getInt32(env, args[4]);
	GLsizei depth = getInt32(env, args[5]);
	GLint border = getInt32(env, args[6]);
	GLenum format = getUint32(env, args[7]);
	GLenum type = getUint32(env, args[8]);
	void * pixels = nullptr;
	status = getTypedArray(env, args[9], *(void **)&pixels);
	if (status != napi_ok) return nullptr;
	// void glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
	glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
	return NULL;
}

napi_value TexSubImage3D(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[11];
	size_t argc = checkArgCount(env, info, args, 11, 11);
	GLenum target = getUint32(env, args[0]);
	GLint level = getInt32(env, args[1]);
	GLint xoffset = getInt32(env, args[2]);
	GLint yoffset = getInt32(env, args[3]);
	GLint zoffset = getInt32(env, args[4]);
	GLsizei width = getInt32(env, args[5]);
	GLsizei height = getInt32(env, args[6]);
	GLsizei depth = getInt32(env, args[7]);
	GLenum format = getUint32(env, args[8]);
	GLenum type = getUint32(env, args[9]);
	void * pixels = nullptr;
	status = getTypedArray(env, args[10], *(void **)&pixels);
	if (status != napi_ok) return nullptr;
	// void glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels)
	glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
	return NULL;
}

napi_value CopyTexSubImage3D(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[9];
	size_t argc = checkArgCount(env, info, args, 9, 9);
	GLenum target = getUint32(env, args[0]);
	GLint level = getInt32(env, args[1]);
	GLint xoffset = getInt32(env, args[2]);
	GLint yoffset = getInt32(env, args[3]);
	GLint zoffset = getInt32(env, args[4]);
	GLint x = getInt32(env, args[5]);
	GLint y = getInt32(env, args[6]);
	GLsizei width = getInt32(env, args[7]);
	GLsizei height = getInt32(env, args[8]);
	// void glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
	glCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height);
	return NULL;
}

napi_value CompressedTexImage3D(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[9];
	size_t argc = checkArgCount(env, info, args, 9, 9);
	GLenum target = getUint32(env, args[0]);
	GLint level = getInt32(env, args[1]);
	GLenum internalformat = getUint32(env, args[2]);
	GLsizei width = getInt32(env, args[3]);
	GLsizei height = getInt32(env, args[4]);
	GLsizei depth = getInt32(env, args[5]);
	GLint border = getInt32(env, args[6]);
	GLsizei imageSize = getInt32(env, args[7]);
	void * data = nullptr;
	status = getTypedArray(env, args[8], *(void **)&data);
	if (status != napi_ok) return nullptr;
	// void glCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
	glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data);
	return NULL;
}

napi_value CompressedTexSubImage3D(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[11];
	size_t argc = checkArgCount(env, info, args, 11, 11);
	GLenum target = getUint32(env, args[0]);
	GLint level = getInt32(env, args[1]);
	GLint xoffset = getInt32(env, args[2]);
	GLint yoffset = getInt32(env, args[3]);
	GLint zoffset = getInt32(env, args[4]);
	GLsizei width = getInt32(env, args[5]);
	GLsizei height = getInt32(env, args[6]);
	GLsizei depth = getInt32(env, args[7]);
	GLenum format = getUint32(env, args[8]);
	GLsizei imageSize = getInt32(env, args[9]);
	void * data = nullptr;
	status = getTypedArray(env, args[10], *(void **)&data);
	if (status != napi_ok) return nullptr;
	// void glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data)
	glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
	return NULL;
}

napi_value GenQueries(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLuint * ids = nullptr;
	status = getTypedArray(env, args[1], ids);
	// void glGenQueries(GLsizei n, GLuint *ids)
	glGenQueries(n, ids);
	return NULL;
}

napi_value DeleteQueries(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLuint * ids = nullptr;
	status = getTypedArray(env, args[1], ids);
	// void glDeleteQueries(GLsizei n, const GLuint *ids)
	glDeleteQueries(n, ids);
	return NULL;
}

napi_value IsQuery(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint id = getUint32(env, args[0]);
	// GLboolean glIsQuery(GLuint id)
	GLboolean result = glIsQuery(id);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value BeginQuery(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum target = getUint32(env, args[0]);
	GLuint id = getUint32(env, args[1]);
	// void glBeginQuery(GLenum target, GLuint id)
	glBeginQuery(target, id);
	return NULL;
}

napi_value EndQuery(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum target = getUint32(env, args[0]);
	// void glEndQuery(GLenum target)
	glEndQuery(target);
	return NULL;
}

napi_value GetQueryiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint params_result;
	GLint * params = &params_result;
	// void glGetQueryiv(GLenum target, GLenum pname, GLint *params)
	glGetQueryiv(target, pname, params);
	return NULL;
}

napi_value GetQueryObjectuiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint id = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLuint params_result;
	GLuint * params = &params_result;
	// void glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params)
	glGetQueryObjectuiv(id, pname, params);
	return NULL;
}

napi_value UnmapBuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum target = getUint32(env, args[0]);
	// GLboolean glUnmapBuffer(GLenum target)
	GLboolean result = glUnmapBuffer(target);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetBufferPointerv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	void * params_result = nullptr;
	void ** params = &params_result;
	// void glGetBufferPointerv(GLenum target, GLenum pname, void **params)
	glGetBufferPointerv(target, pname, params);
	return NULL;
}

napi_value DrawBuffers(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLenum * bufs = nullptr;
	status = getTypedArray(env, args[1], bufs);
	// void glDrawBuffers(GLsizei n, const GLenum *bufs)
	glDrawBuffers(n, bufs);
	return NULL;
}

napi_value UniformMatrix2x3fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLboolean transpose = getBool(env, args[2]);
	GLfloat * value = nullptr;
	status = getTypedArray(env, args[3], value);
	// void glUniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	glUniformMatrix2x3fv(location, count, transpose, value);
	return NULL;
}

napi_value UniformMatrix3x2fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLboolean transpose = getBool(env, args[2]);
	GLfloat * value = nullptr;
	status = getTypedArray(env, args[3], value);
	// void glUniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	glUniformMatrix3x2fv(location, count, transpose, value);
	return NULL;
}

napi_value UniformMatrix2x4fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLboolean transpose = getBool(env, args[2]);
	GLfloat * value = nullptr;
	status = getTypedArray(env, args[3], value);
	// void glUniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	glUniformMatrix2x4fv(location, count, transpose, value);
	return NULL;
}

napi_value UniformMatrix4x2fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLboolean transpose = getBool(env, args[2]);
	GLfloat * value = nullptr;
	status = getTypedArray(env, args[3], value);
	// void glUniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	glUniformMatrix4x2fv(location, count, transpose, value);
	return NULL;
}

napi_value UniformMatrix3x4fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLboolean transpose = getBool(env, args[2]);
	GLfloat * value = nullptr;
	status = getTypedArray(env, args[3], value);
	// void glUniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	glUniformMatrix3x4fv(location, count, transpose, value);
	return NULL;
}

napi_value UniformMatrix4x3fv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLboolean transpose = getBool(env, args[2]);
	GLfloat * value = nullptr;
	status = getTypedArray(env, args[3], value);
	// void glUniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)
	glUniformMatrix4x3fv(location, count, transpose, value);
	return NULL;
}

napi_value BlitFramebuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[10];
	size_t argc = checkArgCount(env, info, args, 10, 10);
	GLint srcX0 = getInt32(env, args[0]);
	GLint srcY0 = getInt32(env, args[1]);
	GLint srcX1 = getInt32(env, args[2]);
	GLint srcY1 = getInt32(env, args[3]);
	GLint dstX0 = getInt32(env, args[4]);
	GLint dstY0 = getInt32(env, args[5]);
	GLint dstX1 = getInt32(env, args[6]);
	GLint dstY1 = getInt32(env, args[7]);
	GLbitfield mask = getUint32(env, args[8]);
	GLenum filter = getUint32(env, args[9]);
	// void glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
	glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
	return NULL;
}

napi_value RenderbufferStorageMultisample(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLenum target = getUint32(env, args[0]);
	GLsizei samples = getInt32(env, args[1]);
	GLenum internalformat = getUint32(env, args[2]);
	GLsizei width = getInt32(env, args[3]);
	GLsizei height = getInt32(env, args[4]);
	// void glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
	glRenderbufferStorageMultisample(target, samples, internalformat, width, height);
	return NULL;
}

napi_value FramebufferTextureLayer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLenum target = getUint32(env, args[0]);
	GLenum attachment = getUint32(env, args[1]);
	GLuint texture = getUint32(env, args[2]);
	GLint level = getInt32(env, args[3]);
	GLint layer = getInt32(env, args[4]);
	// void glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
	glFramebufferTextureLayer(target, attachment, texture, level, layer);
	return NULL;
}

napi_value FlushMappedBufferRange(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLintptr offset = getInt32(env, args[1]);
	GLsizeiptr length = getUint32(env, args[2]);
	// void glFlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length)
	glFlushMappedBufferRange(target, offset, length);
	return NULL;
}

napi_value BindVertexArray(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint array = getUint32(env, args[0]);
	// void glBindVertexArray(GLuint array)
	glBindVertexArray(array);
	return NULL;
}

napi_value DeleteVertexArrays(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLuint * arrays = nullptr;
	status = getTypedArray(env, args[1], arrays);
	// void glDeleteVertexArrays(GLsizei n, const GLuint *arrays)
	glDeleteVertexArrays(n, arrays);
	return NULL;
}

napi_value GenVertexArrays(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLuint * arrays = nullptr;
	status = getTypedArray(env, args[1], arrays);
	// void glGenVertexArrays(GLsizei n, GLuint *arrays)
	glGenVertexArrays(n, arrays);
	return NULL;
}

napi_value IsVertexArray(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint array = getUint32(env, args[0]);
	// GLboolean glIsVertexArray(GLuint array)
	GLboolean result = glIsVertexArray(array);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value BeginTransformFeedback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLenum primitiveMode = getUint32(env, args[0]);
	// void glBeginTransformFeedback(GLenum primitiveMode)
	glBeginTransformFeedback(primitiveMode);
	return NULL;
}

napi_value EndTransformFeedback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// void glEndTransformFeedback(void)
	glEndTransformFeedback();
	return NULL;
}

napi_value BindBufferRange(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLenum target = getUint32(env, args[0]);
	GLuint index = getUint32(env, args[1]);
	GLuint buffer = getUint32(env, args[2]);
	GLintptr offset = getInt32(env, args[3]);
	GLsizeiptr size = getUint32(env, args[4]);
	// void glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
	glBindBufferRange(target, index, buffer, offset, size);
	return NULL;
}

napi_value BindBufferBase(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLuint index = getUint32(env, args[1]);
	GLuint buffer = getUint32(env, args[2]);
	// void glBindBufferBase(GLenum target, GLuint index, GLuint buffer)
	glBindBufferBase(target, index, buffer);
	return NULL;
}

napi_value TransformFeedbackVaryings(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLuint program = getUint32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLchar ** varyings = nullptr;
	status = getListOfStrings(env, args[2], varyings);
	GLenum bufferMode = getUint32(env, args[3]);
	// void glTransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar *const*varyings, GLenum bufferMode)
	glTransformFeedbackVaryings(program, count, varyings, bufferMode);
	return NULL;
}

napi_value GetTransformFeedbackVarying(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[7];
	size_t argc = checkArgCount(env, info, args, 7, 7);
	GLuint program = getUint32(env, args[0]);
	GLuint index = getUint32(env, args[1]);
	GLsizei bufSize = getInt32(env, args[2]);
	GLsizei length_result;
	GLsizei * length = &length_result;
	GLsizei size_result;
	GLsizei * size = &size_result;
	GLenum type_result;
	GLenum * type = &type_result;
	GLchar * name;
	status = getTypedArray(env, args[6], name);
	// void glGetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name)
	glGetTransformFeedbackVarying(program, index, bufSize, length, size, type, name);
	return NULL;
}

napi_value VertexAttribIPointer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLuint index = getUint32(env, args[0]);
	GLint size = getInt32(env, args[1]);
	GLenum type = getUint32(env, args[2]);
	GLsizei stride = getInt32(env, args[3]);
	void * pointer = nullptr;
	status = getTypedArray(env, args[4], *(void **)&pointer);
	if (status != napi_ok) return nullptr;
	// void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer)
	glVertexAttribIPointer(index, size, type, stride, pointer);
	return NULL;
}

napi_value GetVertexAttribIiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint index = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint params_result;
	GLint * params = &params_result;
	// void glGetVertexAttribIiv(GLuint index, GLenum pname, GLint *params)
	glGetVertexAttribIiv(index, pname, params);
	return NULL;
}

napi_value GetVertexAttribIuiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint index = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLuint params_result;
	GLuint * params = &params_result;
	// void glGetVertexAttribIuiv(GLuint index, GLenum pname, GLuint *params)
	glGetVertexAttribIuiv(index, pname, params);
	return NULL;
}

napi_value VertexAttribI4i(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLuint index = getUint32(env, args[0]);
	GLint x = getInt32(env, args[1]);
	GLint y = getInt32(env, args[2]);
	GLint z = getInt32(env, args[3]);
	GLint w = getInt32(env, args[4]);
	// void glVertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w)
	glVertexAttribI4i(index, x, y, z, w);
	return NULL;
}

napi_value VertexAttribI4ui(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLuint index = getUint32(env, args[0]);
	GLuint x = getUint32(env, args[1]);
	GLuint y = getUint32(env, args[2]);
	GLuint z = getUint32(env, args[3]);
	GLuint w = getUint32(env, args[4]);
	// void glVertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
	glVertexAttribI4ui(index, x, y, z, w);
	return NULL;
}

napi_value VertexAttribI4iv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLuint index = getUint32(env, args[0]);
	GLint * v = nullptr;
	status = getTypedArray(env, args[1], v);
	// void glVertexAttribI4iv(GLuint index, const GLint *v)
	glVertexAttribI4iv(index, v);
	return NULL;
}

napi_value VertexAttribI4uiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLuint index = getUint32(env, args[0]);
	GLuint * v = nullptr;
	status = getTypedArray(env, args[1], v);
	// void glVertexAttribI4uiv(GLuint index, const GLuint *v)
	glVertexAttribI4uiv(index, v);
	return NULL;
}

napi_value GetUniformuiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint program = getUint32(env, args[0]);
	GLint location = getInt32(env, args[1]);
	GLuint params_result;
	GLuint * params = &params_result;
	// void glGetUniformuiv(GLuint program, GLint location, GLuint *params)
	glGetUniformuiv(program, location, params);
	return NULL;
}

napi_value GetFragDataLocation(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLuint program = getUint32(env, args[0]);
	GLchar * name;
	status = getTypedArray(env, args[1], name);
	// GLint glGetFragDataLocation(GLuint program, const GLchar *name)
	GLint result = glGetFragDataLocation(program, name);
	napi_value result_value = nullptr;
	status = napi_create_int32(env, (int32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value Uniform1ui(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLint location = getInt32(env, args[0]);
	GLuint v0 = getUint32(env, args[1]);
	// void glUniform1ui(GLint location, GLuint v0)
	glUniform1ui(location, v0);
	return NULL;
}

napi_value Uniform2ui(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLuint v0 = getUint32(env, args[1]);
	GLuint v1 = getUint32(env, args[2]);
	// void glUniform2ui(GLint location, GLuint v0, GLuint v1)
	glUniform2ui(location, v0, v1);
	return NULL;
}

napi_value Uniform3ui(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLint location = getInt32(env, args[0]);
	GLuint v0 = getUint32(env, args[1]);
	GLuint v1 = getUint32(env, args[2]);
	GLuint v2 = getUint32(env, args[3]);
	// void glUniform3ui(GLint location, GLuint v0, GLuint v1, GLuint v2)
	glUniform3ui(location, v0, v1, v2);
	return NULL;
}

napi_value Uniform4ui(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLint location = getInt32(env, args[0]);
	GLuint v0 = getUint32(env, args[1]);
	GLuint v1 = getUint32(env, args[2]);
	GLuint v2 = getUint32(env, args[3]);
	GLuint v3 = getUint32(env, args[4]);
	// void glUniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
	glUniform4ui(location, v0, v1, v2, v3);
	return NULL;
}

napi_value Uniform1uiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLuint * value = nullptr;
	status = getTypedArray(env, args[2], value);
	// void glUniform1uiv(GLint location, GLsizei count, const GLuint *value)
	glUniform1uiv(location, count, value);
	return NULL;
}

napi_value Uniform2uiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLuint * value = nullptr;
	status = getTypedArray(env, args[2], value);
	// void glUniform2uiv(GLint location, GLsizei count, const GLuint *value)
	glUniform2uiv(location, count, value);
	return NULL;
}

napi_value Uniform3uiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLuint * value = nullptr;
	status = getTypedArray(env, args[2], value);
	// void glUniform3uiv(GLint location, GLsizei count, const GLuint *value)
	glUniform3uiv(location, count, value);
	return NULL;
}

napi_value Uniform4uiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLint location = getInt32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLuint * value = nullptr;
	status = getTypedArray(env, args[2], value);
	// void glUniform4uiv(GLint location, GLsizei count, const GLuint *value)
	glUniform4uiv(location, count, value);
	return NULL;
}

napi_value ClearBufferiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum buffer = getUint32(env, args[0]);
	GLint drawbuffer = getInt32(env, args[1]);
	GLint * value = nullptr;
	status = getTypedArray(env, args[2], value);
	// void glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint *value)
	glClearBufferiv(buffer, drawbuffer, value);
	return NULL;
}

napi_value ClearBufferuiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum buffer = getUint32(env, args[0]);
	GLint drawbuffer = getInt32(env, args[1]);
	GLuint * value = nullptr;
	status = getTypedArray(env, args[2], value);
	// void glClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint *value)
	glClearBufferuiv(buffer, drawbuffer, value);
	return NULL;
}

napi_value ClearBufferfi(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLenum buffer = getUint32(env, args[0]);
	GLint drawbuffer = getInt32(env, args[1]);
	GLfloat depth = getDouble(env, args[2]);
	GLint stencil = getInt32(env, args[3]);
	// void glClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
	glClearBufferfi(buffer, drawbuffer, depth, stencil);
	return NULL;
}

napi_value CopyBufferSubData(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLenum readTarget = getUint32(env, args[0]);
	GLenum writeTarget = getUint32(env, args[1]);
	GLintptr readOffset = getInt32(env, args[2]);
	GLintptr writeOffset = getInt32(env, args[3]);
	GLsizeiptr size = getUint32(env, args[4]);
	// void glCopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
	glCopyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size);
	return NULL;
}

napi_value GetUniformIndices(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLuint program = getUint32(env, args[0]);
	GLsizei uniformCount = getInt32(env, args[1]);
	GLchar ** uniformNames = nullptr;
	status = getListOfStrings(env, args[2], uniformNames);
	GLuint uniformIndices_result;
	GLuint * uniformIndices = &uniformIndices_result;
	// void glGetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices)
	glGetUniformIndices(program, uniformCount, uniformNames, uniformIndices);
	return NULL;
}

napi_value GetActiveUniformsiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLuint program = getUint32(env, args[0]);
	GLsizei uniformCount = getInt32(env, args[1]);
	GLuint uniformIndices_result;
	GLuint * uniformIndices = &uniformIndices_result;
	GLenum pname = getUint32(env, args[3]);
	GLint params_result;
	GLint * params = &params_result;
	// void glGetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params)
	glGetActiveUniformsiv(program, uniformCount, uniformIndices, pname, params);
	return NULL;
}

napi_value GetUniformBlockIndex(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLuint program = getUint32(env, args[0]);
	GLchar * uniformBlockName;
	status = getTypedArray(env, args[1], uniformBlockName);
	// GLuint glGetUniformBlockIndex(GLuint program, const GLchar *uniformBlockName)
	GLuint result = glGetUniformBlockIndex(program, uniformBlockName);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value GetActiveUniformBlockiv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLuint program = getUint32(env, args[0]);
	GLuint uniformBlockIndex = getUint32(env, args[1]);
	GLenum pname = getUint32(env, args[2]);
	GLint params_result;
	GLint * params = &params_result;
	// void glGetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params)
	glGetActiveUniformBlockiv(program, uniformBlockIndex, pname, params);
	return NULL;
}

napi_value GetActiveUniformBlockName(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLuint program = getUint32(env, args[0]);
	GLuint uniformBlockIndex = getUint32(env, args[1]);
	GLsizei bufSize = getInt32(env, args[2]);
	GLsizei length_result;
	GLsizei * length = &length_result;
	GLchar * uniformBlockName;
	status = getTypedArray(env, args[4], uniformBlockName);
	// void glGetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName)
	glGetActiveUniformBlockName(program, uniformBlockIndex, bufSize, length, uniformBlockName);
	return NULL;
}

napi_value UniformBlockBinding(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint program = getUint32(env, args[0]);
	GLuint uniformBlockIndex = getUint32(env, args[1]);
	GLuint uniformBlockBinding = getUint32(env, args[2]);
	// void glUniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
	glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
	return NULL;
}

napi_value DrawArraysInstanced(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLenum mode = getUint32(env, args[0]);
	GLint first = getInt32(env, args[1]);
	GLsizei count = getInt32(env, args[2]);
	GLsizei instancecount = getInt32(env, args[3]);
	// void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
	glDrawArraysInstanced(mode, first, count, instancecount);
	return NULL;
}

napi_value DrawElementsInstanced(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLenum mode = getUint32(env, args[0]);
	GLsizei count = getInt32(env, args[1]);
	GLenum type = getUint32(env, args[2]);
	void * indices = nullptr;
	status = getTypedArray(env, args[3], *(void **)&indices);
	if (status != napi_ok) return nullptr;
	GLsizei instancecount = getInt32(env, args[4]);
	// void glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
	glDrawElementsInstanced(mode, count, type, indices, instancecount);
	return NULL;
}

napi_value FenceSync(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum condition = getUint32(env, args[0]);
	GLbitfield flags = getUint32(env, args[1]);
	// GLsync glFenceSync(GLenum condition, GLbitfield flags)
	GLsync result = glFenceSync(condition, flags);
	napi_value result_value = nullptr;
	status = napi_create_external(env, (void *)result, NULL, NULL, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value IsSync(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLsync sync = nullptr;
	status = getTypedArray(env, args[0], *(void **)&sync);
	if (status != napi_ok) return nullptr;
	// GLboolean glIsSync(GLsync sync)
	GLboolean result = glIsSync(sync);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value DeleteSync(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLsync sync = nullptr;
	status = getTypedArray(env, args[0], *(void **)&sync);
	if (status != napi_ok) return nullptr;
	// void glDeleteSync(GLsync sync)
	glDeleteSync(sync);
	return NULL;
}

napi_value ClientWaitSync(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLsync sync = nullptr;
	status = getTypedArray(env, args[0], *(void **)&sync);
	if (status != napi_ok) return nullptr;
	GLbitfield flags = getUint32(env, args[1]);
	uint64_t timeout = 0;
	uint32_t timeout_uint32;
	bool timeout_lossless;
		status = napi_get_value_uint32(env, args[2], &timeout_uint32);
		timeout = timeout_uint32;
	if (status != napi_ok) return nullptr;
	// GLenum glClientWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
	GLenum result = glClientWaitSync(sync, flags, timeout);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value WaitSync(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLsync sync = nullptr;
	status = getTypedArray(env, args[0], *(void **)&sync);
	if (status != napi_ok) return nullptr;
	GLbitfield flags = getUint32(env, args[1]);
	uint64_t timeout = 0;
	uint32_t timeout_uint32;
	bool timeout_lossless;
		status = napi_get_value_uint32(env, args[2], &timeout_uint32);
		timeout = timeout_uint32;
	if (status != napi_ok) return nullptr;
	// void glWaitSync(GLsync sync, GLbitfield flags, GLuint64 timeout)
	glWaitSync(sync, flags, timeout);
	return NULL;
}

napi_value GetSynciv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLsync sync = nullptr;
	status = getTypedArray(env, args[0], *(void **)&sync);
	if (status != napi_ok) return nullptr;
	GLenum pname = getUint32(env, args[1]);
	GLsizei bufSize = getInt32(env, args[2]);
	GLsizei length_result;
	GLsizei * length = &length_result;
	GLint values_result;
	GLint * values = &values_result;
	// void glGetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values)
	glGetSynciv(sync, pname, bufSize, length, values);
	return NULL;
}

napi_value GenSamplers(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei count = getInt32(env, args[0]);
	GLuint * samplers = nullptr;
	status = getTypedArray(env, args[1], samplers);
	// void glGenSamplers(GLsizei count, GLuint *samplers)
	glGenSamplers(count, samplers);
	return NULL;
}

napi_value DeleteSamplers(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei count = getInt32(env, args[0]);
	GLuint * samplers = nullptr;
	status = getTypedArray(env, args[1], samplers);
	// void glDeleteSamplers(GLsizei count, const GLuint *samplers)
	glDeleteSamplers(count, samplers);
	return NULL;
}

napi_value IsSampler(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint sampler = getUint32(env, args[0]);
	// GLboolean glIsSampler(GLuint sampler)
	GLboolean result = glIsSampler(sampler);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value BindSampler(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLuint unit = getUint32(env, args[0]);
	GLuint sampler = getUint32(env, args[1]);
	// void glBindSampler(GLuint unit, GLuint sampler)
	glBindSampler(unit, sampler);
	return NULL;
}

napi_value SamplerParameteri(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint sampler = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint param = getInt32(env, args[2]);
	// void glSamplerParameteri(GLuint sampler, GLenum pname, GLint param)
	glSamplerParameteri(sampler, pname, param);
	return NULL;
}

napi_value SamplerParameteriv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint sampler = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint * param = nullptr;
	status = getTypedArray(env, args[2], param);
	// void glSamplerParameteriv(GLuint sampler, GLenum pname, const GLint *param)
	glSamplerParameteriv(sampler, pname, param);
	return NULL;
}

napi_value SamplerParameterf(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint sampler = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLfloat param = getDouble(env, args[2]);
	// void glSamplerParameterf(GLuint sampler, GLenum pname, GLfloat param)
	glSamplerParameterf(sampler, pname, param);
	return NULL;
}

napi_value SamplerParameterfv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint sampler = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLfloat * param = nullptr;
	status = getTypedArray(env, args[2], param);
	// void glSamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat *param)
	glSamplerParameterfv(sampler, pname, param);
	return NULL;
}

napi_value GetSamplerParameteriv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint sampler = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint params_result;
	GLint * params = &params_result;
	// void glGetSamplerParameteriv(GLuint sampler, GLenum pname, GLint *params)
	glGetSamplerParameteriv(sampler, pname, params);
	return NULL;
}

napi_value GetSamplerParameterfv(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint sampler = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLfloat params_result;
	GLfloat * params = &params_result;
	// void glGetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat *params)
	glGetSamplerParameterfv(sampler, pname, params);
	return NULL;
}

napi_value VertexAttribDivisor(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLuint index = getUint32(env, args[0]);
	GLuint divisor = getUint32(env, args[1]);
	// void glVertexAttribDivisor(GLuint index, GLuint divisor)
	glVertexAttribDivisor(index, divisor);
	return NULL;
}

napi_value BindTransformFeedback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLenum target = getUint32(env, args[0]);
	GLuint id = getUint32(env, args[1]);
	// void glBindTransformFeedback(GLenum target, GLuint id)
	glBindTransformFeedback(target, id);
	return NULL;
}

napi_value DeleteTransformFeedbacks(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLuint * ids = nullptr;
	status = getTypedArray(env, args[1], ids);
	// void glDeleteTransformFeedbacks(GLsizei n, const GLuint *ids)
	glDeleteTransformFeedbacks(n, ids);
	return NULL;
}

napi_value GenTransformFeedbacks(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[2];
	size_t argc = checkArgCount(env, info, args, 2, 2);
	GLsizei n = getInt32(env, args[0]);
	GLuint * ids = nullptr;
	status = getTypedArray(env, args[1], ids);
	// void glGenTransformFeedbacks(GLsizei n, GLuint *ids)
	glGenTransformFeedbacks(n, ids);
	return NULL;
}

napi_value IsTransformFeedback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[1];
	size_t argc = checkArgCount(env, info, args, 1, 1);
	GLuint id = getUint32(env, args[0]);
	// GLboolean glIsTransformFeedback(GLuint id)
	GLboolean result = glIsTransformFeedback(id);
	napi_value result_value = nullptr;
	status = napi_create_uint32(env, (uint32_t)result, &result_value);
	return (status == napi_ok) ? result_value : nullptr;
}

napi_value PauseTransformFeedback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// void glPauseTransformFeedback(void)
	glPauseTransformFeedback();
	return NULL;
}

napi_value ResumeTransformFeedback(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	// void glResumeTransformFeedback(void)
	glResumeTransformFeedback();
	return NULL;
}

napi_value GetProgramBinary(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLuint program = getUint32(env, args[0]);
	GLsizei bufSize = getInt32(env, args[1]);
	GLsizei length_result;
	GLsizei * length = &length_result;
	GLenum binaryFormat_result;
	GLenum * binaryFormat = &binaryFormat_result;
	void * binary = nullptr;
	status = getTypedArray(env, args[4], *(void **)&binary);
	if (status != napi_ok) return nullptr;
	// void glGetProgramBinary(GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary)
	glGetProgramBinary(program, bufSize, length, binaryFormat, binary);
	return NULL;
}

napi_value ProgramBinary(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[4];
	size_t argc = checkArgCount(env, info, args, 4, 4);
	GLuint program = getUint32(env, args[0]);
	GLenum binaryFormat = getUint32(env, args[1]);
	void * binary = nullptr;
	status = getTypedArray(env, args[2], *(void **)&binary);
	if (status != napi_ok) return nullptr;
	GLsizei length = getInt32(env, args[3]);
	// void glProgramBinary(GLuint program, GLenum binaryFormat, const void *binary, GLsizei length)
	glProgramBinary(program, binaryFormat, binary, length);
	return NULL;
}

napi_value ProgramParameteri(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLuint program = getUint32(env, args[0]);
	GLenum pname = getUint32(env, args[1]);
	GLint value = getInt32(env, args[2]);
	// void glProgramParameteri(GLuint program, GLenum pname, GLint value)
	glProgramParameteri(program, pname, value);
	return NULL;
}

napi_value InvalidateFramebuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[3];
	size_t argc = checkArgCount(env, info, args, 3, 3);
	GLenum target = getUint32(env, args[0]);
	GLsizei numAttachments = getInt32(env, args[1]);
	GLenum * attachments = nullptr;
	status = getTypedArray(env, args[2], attachments);
	// void glInvalidateFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments)
	glInvalidateFramebuffer(target, numAttachments, attachments);
	return NULL;
}

napi_value InvalidateSubFramebuffer(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[7];
	size_t argc = checkArgCount(env, info, args, 7, 7);
	GLenum target = getUint32(env, args[0]);
	GLsizei numAttachments = getInt32(env, args[1]);
	GLenum * attachments = nullptr;
	status = getTypedArray(env, args[2], attachments);
	GLint x = getInt32(env, args[3]);
	GLint y = getInt32(env, args[4]);
	GLsizei width = getInt32(env, args[5]);
	GLsizei height = getInt32(env, args[6]);
	// void glInvalidateSubFramebuffer(GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height)
	glInvalidateSubFramebuffer(target, numAttachments, attachments, x, y, width, height);
	return NULL;
}

napi_value TexStorage2D(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLenum target = getUint32(env, args[0]);
	GLsizei levels = getInt32(env, args[1]);
	GLenum internalformat = getUint32(env, args[2]);
	GLsizei width = getInt32(env, args[3]);
	GLsizei height = getInt32(env, args[4]);
	// void glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
	glTexStorage2D(target, levels, internalformat, width, height);
	return NULL;
}

napi_value TexStorage3D(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[6];
	size_t argc = checkArgCount(env, info, args, 6, 6);
	GLenum target = getUint32(env, args[0]);
	GLsizei levels = getInt32(env, args[1]);
	GLenum internalformat = getUint32(env, args[2]);
	GLsizei width = getInt32(env, args[3]);
	GLsizei height = getInt32(env, args[4]);
	GLsizei depth = getInt32(env, args[5]);
	// void glTexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
	glTexStorage3D(target, levels, internalformat, width, height, depth);
	return NULL;
}

napi_value GetInternalformativ(napi_env env, napi_callback_info info) {
	napi_status status = napi_ok;
	napi_value args[5];
	size_t argc = checkArgCount(env, info, args, 5, 5);
	GLenum target = getUint32(env, args[0]);
	GLenum internalformat = getUint32(env, args[1]);
	GLenum pname = getUint32(env, args[2]);
	GLsizei bufSize = getInt32(env, args[3]);
	GLint params_result;
	GLint * params = &params_result;
	// void glGetInternalformativ(GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params)
	glGetInternalformativ(target, internalformat, pname, bufSize, params);
	return NULL;
}

napi_value init(napi_env env, napi_value exports) {
	napi_status status;
	napi_property_descriptor properties[] = {
		{ "glewInit", 0, glewInit, 0, 0, 0, napi_default, 0 },
		{ "getError", 0, GetError, 0, 0, 0, napi_default, 0 },
		{ "clearBufferfv", 0, ClearBufferfv, 0, 0, 0, napi_default, 0 },
		{ "bufferData", 0, BufferData, 0, 0, 0, napi_default, 0 },
		{ "createBuffer", 0, CreateBuffer, 0, 0, 0, napi_default, 0 },
		{ "createFramebuffer", 0, CreateFramebuffer, 0, 0, 0, napi_default, 0 },
		{ "createProgram", 0, CreateProgram, 0, 0, 0, napi_default, 0 },
		{ "createRenderbuffer", 0, CreateRenderbuffer, 0, 0, 0, napi_default, 0 },
		{ "createTexture", 0, CreateTexture, 0, 0, 0, napi_default, 0 },
		{ "createVertexArray", 0, CreateVertexArray, 0, 0, 0, napi_default, 0 },
		{ "getAttribLocation", 0, GetAttribLocation, 0, 0, 0, napi_default, 0 },
		{ "getBufferParameteri64v", 0, GetBufferParameteri64v, 0, 0, 0, napi_default, 0 },
		{ "getProgramInfoLog", 0, GetProgramInfoLog, 0, 0, 0, napi_default, 0 },
		{ "getProgramParameter", 0, GetProgramParameter, 0, 0, 0, napi_default, 0 },
		{ "getShaderInfoLog", 0, GetShaderInfoLog, 0, 0, 0, napi_default, 0 },
		{ "getShaderParameter", 0, GetShaderParameter, 0, 0, 0, napi_default, 0 },
		{ "getUniformLocation", 0, GetUniformLocation, 0, 0, 0, napi_default, 0 },
		{ "shaderSource", 0, ShaderSource, 0, 0, 0, napi_default, 0 },
		{ "uniformMatrix2fv", 0, UniformMatrix2fv, 0, 0, 0, napi_default, 0 },
		{ "uniformMatrix3fv", 0, UniformMatrix3fv, 0, 0, 0, napi_default, 0 },
		{ "uniformMatrix4fv", 0, UniformMatrix4fv, 0, 0, 0, napi_default, 0 },
		{ "vertexAttribPointer", 0, VertexAttribPointer, 0, 0, 0, napi_default, 0 },
		{ "getShaderPrecisionFormat", 0, GetShaderPrecisionFormat, 0, 0, 0, napi_default, 0 },
		{ "getIntegerv", 0, GetIntegerv, 0, 0, 0, napi_default, 0 },
		{ "getIntegeri_v", 0, GetIntegeri_v, 0, 0, 0, napi_default, 0 },
		{ "getInteger64v", 0, GetInteger64v, 0, 0, 0, napi_default, 0 },
		{ "getInteger64i_v", 0, GetInteger64i_v, 0, 0, 0, napi_default, 0 },
		{ "getBooleanv", 0, GetBooleanv, 0, 0, 0, napi_default, 0 },
		{ "getFloatv", 0, GetFloatv, 0, 0, 0, napi_default, 0 },
		{ "activeTexture", 0, ActiveTexture, 0, 0, 0, napi_default, 0 },
		{ "attachShader", 0, AttachShader, 0, 0, 0, napi_default, 0 },
		{ "bindAttribLocation", 0, BindAttribLocation, 0, 0, 0, napi_default, 0 },
		{ "bindBuffer", 0, BindBuffer, 0, 0, 0, napi_default, 0 },
		{ "bindFramebuffer", 0, BindFramebuffer, 0, 0, 0, napi_default, 0 },
		{ "bindRenderbuffer", 0, BindRenderbuffer, 0, 0, 0, napi_default, 0 },
		{ "bindTexture", 0, BindTexture, 0, 0, 0, napi_default, 0 },
		{ "blendColor", 0, BlendColor, 0, 0, 0, napi_default, 0 },
		{ "blendEquation", 0, BlendEquation, 0, 0, 0, napi_default, 0 },
		{ "blendEquationSeparate", 0, BlendEquationSeparate, 0, 0, 0, napi_default, 0 },
		{ "blendFunc", 0, BlendFunc, 0, 0, 0, napi_default, 0 },
		{ "blendFuncSeparate", 0, BlendFuncSeparate, 0, 0, 0, napi_default, 0 },
		{ "bufferSubData", 0, BufferSubData, 0, 0, 0, napi_default, 0 },
		{ "checkFramebufferStatus", 0, CheckFramebufferStatus, 0, 0, 0, napi_default, 0 },
		{ "clear", 0, Clear, 0, 0, 0, napi_default, 0 },
		{ "clearColor", 0, ClearColor, 0, 0, 0, napi_default, 0 },
		{ "clearDepthf", 0, ClearDepthf, 0, 0, 0, napi_default, 0 },
		{ "clearStencil", 0, ClearStencil, 0, 0, 0, napi_default, 0 },
		{ "colorMask", 0, ColorMask, 0, 0, 0, napi_default, 0 },
		{ "compileShader", 0, CompileShader, 0, 0, 0, napi_default, 0 },
		{ "compressedTexImage2D", 0, CompressedTexImage2D, 0, 0, 0, napi_default, 0 },
		{ "compressedTexSubImage2D", 0, CompressedTexSubImage2D, 0, 0, 0, napi_default, 0 },
		{ "copyTexImage2D", 0, CopyTexImage2D, 0, 0, 0, napi_default, 0 },
		{ "copyTexSubImage2D", 0, CopyTexSubImage2D, 0, 0, 0, napi_default, 0 },
		{ "createShader", 0, CreateShader, 0, 0, 0, napi_default, 0 },
		{ "cullFace", 0, CullFace, 0, 0, 0, napi_default, 0 },
		{ "deleteBuffers", 0, DeleteBuffers, 0, 0, 0, napi_default, 0 },
		{ "deleteFramebuffers", 0, DeleteFramebuffers, 0, 0, 0, napi_default, 0 },
		{ "deleteProgram", 0, DeleteProgram, 0, 0, 0, napi_default, 0 },
		{ "deleteRenderbuffers", 0, DeleteRenderbuffers, 0, 0, 0, napi_default, 0 },
		{ "deleteShader", 0, DeleteShader, 0, 0, 0, napi_default, 0 },
		{ "deleteTextures", 0, DeleteTextures, 0, 0, 0, napi_default, 0 },
		{ "depthFunc", 0, DepthFunc, 0, 0, 0, napi_default, 0 },
		{ "depthMask", 0, DepthMask, 0, 0, 0, napi_default, 0 },
		{ "depthRangef", 0, DepthRangef, 0, 0, 0, napi_default, 0 },
		{ "detachShader", 0, DetachShader, 0, 0, 0, napi_default, 0 },
		{ "disable", 0, Disable, 0, 0, 0, napi_default, 0 },
		{ "disableVertexAttribArray", 0, DisableVertexAttribArray, 0, 0, 0, napi_default, 0 },
		{ "drawArrays", 0, DrawArrays, 0, 0, 0, napi_default, 0 },
		{ "drawElements", 0, DrawElements, 0, 0, 0, napi_default, 0 },
		{ "enable", 0, Enable, 0, 0, 0, napi_default, 0 },
		{ "enableVertexAttribArray", 0, EnableVertexAttribArray, 0, 0, 0, napi_default, 0 },
		{ "finish", 0, Finish, 0, 0, 0, napi_default, 0 },
		{ "flush", 0, Flush, 0, 0, 0, napi_default, 0 },
		{ "framebufferRenderbuffer", 0, FramebufferRenderbuffer, 0, 0, 0, napi_default, 0 },
		{ "framebufferTexture2D", 0, FramebufferTexture2D, 0, 0, 0, napi_default, 0 },
		{ "frontFace", 0, FrontFace, 0, 0, 0, napi_default, 0 },
		{ "genBuffers", 0, GenBuffers, 0, 0, 0, napi_default, 0 },
		{ "generateMipmap", 0, GenerateMipmap, 0, 0, 0, napi_default, 0 },
		{ "genFramebuffers", 0, GenFramebuffers, 0, 0, 0, napi_default, 0 },
		{ "genRenderbuffers", 0, GenRenderbuffers, 0, 0, 0, napi_default, 0 },
		{ "genTextures", 0, GenTextures, 0, 0, 0, napi_default, 0 },
		{ "getActiveAttrib", 0, GetActiveAttrib, 0, 0, 0, napi_default, 0 },
		{ "getActiveUniform", 0, GetActiveUniform, 0, 0, 0, napi_default, 0 },
		{ "getAttachedShaders", 0, GetAttachedShaders, 0, 0, 0, napi_default, 0 },
		{ "getBufferParameteriv", 0, GetBufferParameteriv, 0, 0, 0, napi_default, 0 },
		{ "getFramebufferAttachmentParameteriv", 0, GetFramebufferAttachmentParameteriv, 0, 0, 0, napi_default, 0 },
		{ "getProgramiv", 0, GetProgramiv, 0, 0, 0, napi_default, 0 },
		{ "getRenderbufferParameteriv", 0, GetRenderbufferParameteriv, 0, 0, 0, napi_default, 0 },
		{ "getShaderiv", 0, GetShaderiv, 0, 0, 0, napi_default, 0 },
		{ "getShaderSource", 0, GetShaderSource, 0, 0, 0, napi_default, 0 },
		{ "getTexParameterfv", 0, GetTexParameterfv, 0, 0, 0, napi_default, 0 },
		{ "getTexParameteriv", 0, GetTexParameteriv, 0, 0, 0, napi_default, 0 },
		{ "getUniformfv", 0, GetUniformfv, 0, 0, 0, napi_default, 0 },
		{ "getUniformiv", 0, GetUniformiv, 0, 0, 0, napi_default, 0 },
		{ "getVertexAttribfv", 0, GetVertexAttribfv, 0, 0, 0, napi_default, 0 },
		{ "getVertexAttribiv", 0, GetVertexAttribiv, 0, 0, 0, napi_default, 0 },
		{ "getVertexAttribPointerv", 0, GetVertexAttribPointerv, 0, 0, 0, napi_default, 0 },
		{ "hint", 0, Hint, 0, 0, 0, napi_default, 0 },
		{ "isBuffer", 0, IsBuffer, 0, 0, 0, napi_default, 0 },
		{ "isEnabled", 0, IsEnabled, 0, 0, 0, napi_default, 0 },
		{ "isFramebuffer", 0, IsFramebuffer, 0, 0, 0, napi_default, 0 },
		{ "isProgram", 0, IsProgram, 0, 0, 0, napi_default, 0 },
		{ "isRenderbuffer", 0, IsRenderbuffer, 0, 0, 0, napi_default, 0 },
		{ "isShader", 0, IsShader, 0, 0, 0, napi_default, 0 },
		{ "isTexture", 0, IsTexture, 0, 0, 0, napi_default, 0 },
		{ "lineWidth", 0, LineWidth, 0, 0, 0, napi_default, 0 },
		{ "linkProgram", 0, LinkProgram, 0, 0, 0, napi_default, 0 },
		{ "pixelStorei", 0, PixelStorei, 0, 0, 0, napi_default, 0 },
		{ "polygonOffset", 0, PolygonOffset, 0, 0, 0, napi_default, 0 },
		{ "readPixels", 0, ReadPixels, 0, 0, 0, napi_default, 0 },
		{ "releaseShaderCompiler", 0, ReleaseShaderCompiler, 0, 0, 0, napi_default, 0 },
		{ "renderbufferStorage", 0, RenderbufferStorage, 0, 0, 0, napi_default, 0 },
		{ "sampleCoverage", 0, SampleCoverage, 0, 0, 0, napi_default, 0 },
		{ "scissor", 0, Scissor, 0, 0, 0, napi_default, 0 },
		{ "shaderBinary", 0, ShaderBinary, 0, 0, 0, napi_default, 0 },
		{ "stencilFunc", 0, StencilFunc, 0, 0, 0, napi_default, 0 },
		{ "stencilFuncSeparate", 0, StencilFuncSeparate, 0, 0, 0, napi_default, 0 },
		{ "stencilMask", 0, StencilMask, 0, 0, 0, napi_default, 0 },
		{ "stencilMaskSeparate", 0, StencilMaskSeparate, 0, 0, 0, napi_default, 0 },
		{ "stencilOp", 0, StencilOp, 0, 0, 0, napi_default, 0 },
		{ "stencilOpSeparate", 0, StencilOpSeparate, 0, 0, 0, napi_default, 0 },
		{ "texImage2D", 0, TexImage2D, 0, 0, 0, napi_default, 0 },
		{ "texParameterf", 0, TexParameterf, 0, 0, 0, napi_default, 0 },
		{ "texParameterfv", 0, TexParameterfv, 0, 0, 0, napi_default, 0 },
		{ "texParameteri", 0, TexParameteri, 0, 0, 0, napi_default, 0 },
		{ "texParameteriv", 0, TexParameteriv, 0, 0, 0, napi_default, 0 },
		{ "texSubImage2D", 0, TexSubImage2D, 0, 0, 0, napi_default, 0 },
		{ "uniform1f", 0, Uniform1f, 0, 0, 0, napi_default, 0 },
		{ "uniform1fv", 0, Uniform1fv, 0, 0, 0, napi_default, 0 },
		{ "uniform1i", 0, Uniform1i, 0, 0, 0, napi_default, 0 },
		{ "uniform1iv", 0, Uniform1iv, 0, 0, 0, napi_default, 0 },
		{ "uniform2f", 0, Uniform2f, 0, 0, 0, napi_default, 0 },
		{ "uniform2fv", 0, Uniform2fv, 0, 0, 0, napi_default, 0 },
		{ "uniform2i", 0, Uniform2i, 0, 0, 0, napi_default, 0 },
		{ "uniform2iv", 0, Uniform2iv, 0, 0, 0, napi_default, 0 },
		{ "uniform3f", 0, Uniform3f, 0, 0, 0, napi_default, 0 },
		{ "uniform3fv", 0, Uniform3fv, 0, 0, 0, napi_default, 0 },
		{ "uniform3i", 0, Uniform3i, 0, 0, 0, napi_default, 0 },
		{ "uniform3iv", 0, Uniform3iv, 0, 0, 0, napi_default, 0 },
		{ "uniform4f", 0, Uniform4f, 0, 0, 0, napi_default, 0 },
		{ "uniform4fv", 0, Uniform4fv, 0, 0, 0, napi_default, 0 },
		{ "uniform4i", 0, Uniform4i, 0, 0, 0, napi_default, 0 },
		{ "uniform4iv", 0, Uniform4iv, 0, 0, 0, napi_default, 0 },
		{ "useProgram", 0, UseProgram, 0, 0, 0, napi_default, 0 },
		{ "validateProgram", 0, ValidateProgram, 0, 0, 0, napi_default, 0 },
		{ "vertexAttrib1f", 0, VertexAttrib1f, 0, 0, 0, napi_default, 0 },
		{ "vertexAttrib1fv", 0, VertexAttrib1fv, 0, 0, 0, napi_default, 0 },
		{ "vertexAttrib2f", 0, VertexAttrib2f, 0, 0, 0, napi_default, 0 },
		{ "vertexAttrib2fv", 0, VertexAttrib2fv, 0, 0, 0, napi_default, 0 },
		{ "vertexAttrib3f", 0, VertexAttrib3f, 0, 0, 0, napi_default, 0 },
		{ "vertexAttrib3fv", 0, VertexAttrib3fv, 0, 0, 0, napi_default, 0 },
		{ "vertexAttrib4f", 0, VertexAttrib4f, 0, 0, 0, napi_default, 0 },
		{ "vertexAttrib4fv", 0, VertexAttrib4fv, 0, 0, 0, napi_default, 0 },
		{ "viewport", 0, Viewport, 0, 0, 0, napi_default, 0 },
		{ "readBuffer", 0, ReadBuffer, 0, 0, 0, napi_default, 0 },
		{ "drawRangeElements", 0, DrawRangeElements, 0, 0, 0, napi_default, 0 },
		{ "texImage3D", 0, TexImage3D, 0, 0, 0, napi_default, 0 },
		{ "texSubImage3D", 0, TexSubImage3D, 0, 0, 0, napi_default, 0 },
		{ "copyTexSubImage3D", 0, CopyTexSubImage3D, 0, 0, 0, napi_default, 0 },
		{ "compressedTexImage3D", 0, CompressedTexImage3D, 0, 0, 0, napi_default, 0 },
		{ "compressedTexSubImage3D", 0, CompressedTexSubImage3D, 0, 0, 0, napi_default, 0 },
		{ "genQueries", 0, GenQueries, 0, 0, 0, napi_default, 0 },
		{ "deleteQueries", 0, DeleteQueries, 0, 0, 0, napi_default, 0 },
		{ "isQuery", 0, IsQuery, 0, 0, 0, napi_default, 0 },
		{ "beginQuery", 0, BeginQuery, 0, 0, 0, napi_default, 0 },
		{ "endQuery", 0, EndQuery, 0, 0, 0, napi_default, 0 },
		{ "getQueryiv", 0, GetQueryiv, 0, 0, 0, napi_default, 0 },
		{ "getQueryObjectuiv", 0, GetQueryObjectuiv, 0, 0, 0, napi_default, 0 },
		{ "unmapBuffer", 0, UnmapBuffer, 0, 0, 0, napi_default, 0 },
		{ "getBufferPointerv", 0, GetBufferPointerv, 0, 0, 0, napi_default, 0 },
		{ "drawBuffers", 0, DrawBuffers, 0, 0, 0, napi_default, 0 },
		{ "uniformMatrix2x3fv", 0, UniformMatrix2x3fv, 0, 0, 0, napi_default, 0 },
		{ "uniformMatrix3x2fv", 0, UniformMatrix3x2fv, 0, 0, 0, napi_default, 0 },
		{ "uniformMatrix2x4fv", 0, UniformMatrix2x4fv, 0, 0, 0, napi_default, 0 },
		{ "uniformMatrix4x2fv", 0, UniformMatrix4x2fv, 0, 0, 0, napi_default, 0 },
		{ "uniformMatrix3x4fv", 0, UniformMatrix3x4fv, 0, 0, 0, napi_default, 0 },
		{ "uniformMatrix4x3fv", 0, UniformMatrix4x3fv, 0, 0, 0, napi_default, 0 },
		{ "blitFramebuffer", 0, BlitFramebuffer, 0, 0, 0, napi_default, 0 },
		{ "renderbufferStorageMultisample", 0, RenderbufferStorageMultisample, 0, 0, 0, napi_default, 0 },
		{ "framebufferTextureLayer", 0, FramebufferTextureLayer, 0, 0, 0, napi_default, 0 },
		{ "flushMappedBufferRange", 0, FlushMappedBufferRange, 0, 0, 0, napi_default, 0 },
		{ "bindVertexArray", 0, BindVertexArray, 0, 0, 0, napi_default, 0 },
		{ "deleteVertexArrays", 0, DeleteVertexArrays, 0, 0, 0, napi_default, 0 },
		{ "genVertexArrays", 0, GenVertexArrays, 0, 0, 0, napi_default, 0 },
		{ "isVertexArray", 0, IsVertexArray, 0, 0, 0, napi_default, 0 },
		{ "beginTransformFeedback", 0, BeginTransformFeedback, 0, 0, 0, napi_default, 0 },
		{ "endTransformFeedback", 0, EndTransformFeedback, 0, 0, 0, napi_default, 0 },
		{ "bindBufferRange", 0, BindBufferRange, 0, 0, 0, napi_default, 0 },
		{ "bindBufferBase", 0, BindBufferBase, 0, 0, 0, napi_default, 0 },
		{ "transformFeedbackVaryings", 0, TransformFeedbackVaryings, 0, 0, 0, napi_default, 0 },
		{ "getTransformFeedbackVarying", 0, GetTransformFeedbackVarying, 0, 0, 0, napi_default, 0 },
		{ "vertexAttribIPointer", 0, VertexAttribIPointer, 0, 0, 0, napi_default, 0 },
		{ "getVertexAttribIiv", 0, GetVertexAttribIiv, 0, 0, 0, napi_default, 0 },
		{ "getVertexAttribIuiv", 0, GetVertexAttribIuiv, 0, 0, 0, napi_default, 0 },
		{ "vertexAttribI4i", 0, VertexAttribI4i, 0, 0, 0, napi_default, 0 },
		{ "vertexAttribI4ui", 0, VertexAttribI4ui, 0, 0, 0, napi_default, 0 },
		{ "vertexAttribI4iv", 0, VertexAttribI4iv, 0, 0, 0, napi_default, 0 },
		{ "vertexAttribI4uiv", 0, VertexAttribI4uiv, 0, 0, 0, napi_default, 0 },
		{ "getUniformuiv", 0, GetUniformuiv, 0, 0, 0, napi_default, 0 },
		{ "getFragDataLocation", 0, GetFragDataLocation, 0, 0, 0, napi_default, 0 },
		{ "uniform1ui", 0, Uniform1ui, 0, 0, 0, napi_default, 0 },
		{ "uniform2ui", 0, Uniform2ui, 0, 0, 0, napi_default, 0 },
		{ "uniform3ui", 0, Uniform3ui, 0, 0, 0, napi_default, 0 },
		{ "uniform4ui", 0, Uniform4ui, 0, 0, 0, napi_default, 0 },
		{ "uniform1uiv", 0, Uniform1uiv, 0, 0, 0, napi_default, 0 },
		{ "uniform2uiv", 0, Uniform2uiv, 0, 0, 0, napi_default, 0 },
		{ "uniform3uiv", 0, Uniform3uiv, 0, 0, 0, napi_default, 0 },
		{ "uniform4uiv", 0, Uniform4uiv, 0, 0, 0, napi_default, 0 },
		{ "clearBufferiv", 0, ClearBufferiv, 0, 0, 0, napi_default, 0 },
		{ "clearBufferuiv", 0, ClearBufferuiv, 0, 0, 0, napi_default, 0 },
		{ "clearBufferfi", 0, ClearBufferfi, 0, 0, 0, napi_default, 0 },
		{ "copyBufferSubData", 0, CopyBufferSubData, 0, 0, 0, napi_default, 0 },
		{ "getUniformIndices", 0, GetUniformIndices, 0, 0, 0, napi_default, 0 },
		{ "getActiveUniformsiv", 0, GetActiveUniformsiv, 0, 0, 0, napi_default, 0 },
		{ "getUniformBlockIndex", 0, GetUniformBlockIndex, 0, 0, 0, napi_default, 0 },
		{ "getActiveUniformBlockiv", 0, GetActiveUniformBlockiv, 0, 0, 0, napi_default, 0 },
		{ "getActiveUniformBlockName", 0, GetActiveUniformBlockName, 0, 0, 0, napi_default, 0 },
		{ "uniformBlockBinding", 0, UniformBlockBinding, 0, 0, 0, napi_default, 0 },
		{ "drawArraysInstanced", 0, DrawArraysInstanced, 0, 0, 0, napi_default, 0 },
		{ "drawElementsInstanced", 0, DrawElementsInstanced, 0, 0, 0, napi_default, 0 },
		{ "fenceSync", 0, FenceSync, 0, 0, 0, napi_default, 0 },
		{ "isSync", 0, IsSync, 0, 0, 0, napi_default, 0 },
		{ "deleteSync", 0, DeleteSync, 0, 0, 0, napi_default, 0 },
		{ "clientWaitSync", 0, ClientWaitSync, 0, 0, 0, napi_default, 0 },
		{ "waitSync", 0, WaitSync, 0, 0, 0, napi_default, 0 },
		{ "getSynciv", 0, GetSynciv, 0, 0, 0, napi_default, 0 },
		{ "genSamplers", 0, GenSamplers, 0, 0, 0, napi_default, 0 },
		{ "deleteSamplers", 0, DeleteSamplers, 0, 0, 0, napi_default, 0 },
		{ "isSampler", 0, IsSampler, 0, 0, 0, napi_default, 0 },
		{ "bindSampler", 0, BindSampler, 0, 0, 0, napi_default, 0 },
		{ "samplerParameteri", 0, SamplerParameteri, 0, 0, 0, napi_default, 0 },
		{ "samplerParameteriv", 0, SamplerParameteriv, 0, 0, 0, napi_default, 0 },
		{ "samplerParameterf", 0, SamplerParameterf, 0, 0, 0, napi_default, 0 },
		{ "samplerParameterfv", 0, SamplerParameterfv, 0, 0, 0, napi_default, 0 },
		{ "getSamplerParameteriv", 0, GetSamplerParameteriv, 0, 0, 0, napi_default, 0 },
		{ "getSamplerParameterfv", 0, GetSamplerParameterfv, 0, 0, 0, napi_default, 0 },
		{ "vertexAttribDivisor", 0, VertexAttribDivisor, 0, 0, 0, napi_default, 0 },
		{ "bindTransformFeedback", 0, BindTransformFeedback, 0, 0, 0, napi_default, 0 },
		{ "deleteTransformFeedbacks", 0, DeleteTransformFeedbacks, 0, 0, 0, napi_default, 0 },
		{ "genTransformFeedbacks", 0, GenTransformFeedbacks, 0, 0, 0, napi_default, 0 },
		{ "isTransformFeedback", 0, IsTransformFeedback, 0, 0, 0, napi_default, 0 },
		{ "pauseTransformFeedback", 0, PauseTransformFeedback, 0, 0, 0, napi_default, 0 },
		{ "resumeTransformFeedback", 0, ResumeTransformFeedback, 0, 0, 0, napi_default, 0 },
		{ "getProgramBinary", 0, GetProgramBinary, 0, 0, 0, napi_default, 0 },
		{ "programBinary", 0, ProgramBinary, 0, 0, 0, napi_default, 0 },
		{ "programParameteri", 0, ProgramParameteri, 0, 0, 0, napi_default, 0 },
		{ "invalidateFramebuffer", 0, InvalidateFramebuffer, 0, 0, 0, napi_default, 0 },
		{ "invalidateSubFramebuffer", 0, InvalidateSubFramebuffer, 0, 0, 0, napi_default, 0 },
		{ "texStorage2D", 0, TexStorage2D, 0, 0, 0, napi_default, 0 },
		{ "texStorage3D", 0, TexStorage3D, 0, 0, 0, napi_default, 0 },
		{ "getInternalformativ", 0, GetInternalformativ, 0, 0, 0, napi_default, 0 }
	};
	status = napi_define_properties(env, exports, 251, properties);
	//assert(status == napi_ok);
	return exports;
}
NAPI_MODULE(NODE_GYP_MODULE_NAME, init)