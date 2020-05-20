#ifndef AL_NODE_API_HELPERS
#define AL_NODE_API_HELPERS

#include <node_api.h>
#include <stdio.h>
#include <stdlib.h> // calloc
#include <string>

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
	switch (ty) {
	case napi_number: {
		status = napi_get_value_int32(env, arg, &v);
		return v;
	} break;
	case napi_string: 
	case napi_boolean: {
		napi_value coerced;
		napi_coerce_to_number(env, arg, &coerced);
		status = napi_get_value_int32(env, coerced, &v);
		return v;
	} break;
	default: 
		return 0;
	}
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

#endif //AL_NODE_API_HELPERS