const fs = require("fs"), 
	path = require("path");

let blacklist = {
	// blacklisted because they are not in the libglfw3.dll that is shipped in native-graphics-deps
	glfwInitHint:true,
	glfwJoystickIsGamepad: true,
	glfwRequestWindowAttention: true,
	glfwSetWindowContentScaleCallback: true,
	glfwGetKeyScancode: true,
	glfwSetWindowAttrib: true,
	glfwGetWindowContentScale: true,
	glfwWindowHintString: true,
	glfwGetError: true,
	glfwGetWindowOpacity: true,
	glfwGetGamepadState: true,
	glfwSetJoystickUserPointer: true,
	glfwGetJoystickUserPointer: true,
	glfwSetWindowMaximizeCallback: true,
	glfwUpdateGamepadMappings: true,
	glfwSetWindowOpacity: true,
	glfwGetMonitorContentScale: true,
}

function generate_handler(name, s_name, ret, arg, out_blocks) {

	let args = arg.split(",").map(s=>s.replace(/const/g, '').trim())
	//console.log(arg)
	//console.log(`${ret} gl.${name.substring(2)}(${args.join(", ")})`);
	let nargs = args.length;

	// special case:
	if (nargs == 1 && args[0] == "void") {
		nargs = 0;
		args = [];
	}

	let out_lines = [
		`napi_value ${s_name}(napi_env env, napi_callback_info info) {`,
		`napi_status status = napi_ok;`,
	];

	if (nargs > 0) {
		out_lines.push(`napi_value args[${nargs}];`);
		out_lines.push(`size_t argc = checkArgCount(env, info, args, ${nargs}, ${nargs});`);
	}

	let s_args = [];
	let results = [];
	for (let i in args) {
		let argstr = args[i];
		if (argstr == "void") break;

		let index = argstr.search(/(\w+)$/);
		let argname = argstr.substring(index).trim();
		let argtype = argstr.substring(0, index).trim();

		s_args.push(argname);

		// now handle by type:
		switch (argtype) {
			case "float":
			case "GLfloat": {
				out_lines.push(`${argtype} ${argname} = getDouble(env, args[${i}]);`);
			} break;
			case "GLboolean":  {
				out_lines.push(`${argtype} ${argname} = getBool(env, args[${i}]);`);
			} break;
			case "GLenum": 
			case "GLbitfield":
			case "GLsizeiptr":
			case "GLuint": {
				out_lines.push(`${argtype} ${argname} = getUint32(env, args[${i}]);`);
			} break;
			case "int": 
			case "GLintptr":
			case "GLsizei": 
			case "GLint": {
				out_lines.push(`${argtype} ${argname} = getInt32(env, args[${i}]);`);
			} break;
			case "GLuint64": {
				out_lines.push(
					`uint64_t ${argname} = 0;`,
					`uint32_t ${argname}_uint32;`,
					`bool ${argname}_lossless;`,
					//`status = napi_get_value_bigint_uint64(env, args[${i}], &${argname}, &${argname}_lossless);`,
					//`if (status == napi_bigint_expected) {`,
					`	status = napi_get_value_uint32(env, args[${i}], &${argname}_uint32);`,
					`	${argname} = ${argname}_uint32;`,
					//`}`,
					`if (status != napi_ok) return nullptr;`);
			} break;
			// opaque pointer arguments:
			case "GLFWwindow*":
			case "GLFWmonitor*": {
				out_lines.push(
					`${argtype} ${argname} = nullptr;`,
					`napi_valuetype ${argname}_type;`,
					`status = napi_typeof(env, args[${i}], &${argname}_type);`,
					`if (status != napi_ok || ${argname}_type != napi_external) return nullptr;`,
					`status = napi_get_value_external(env, args[${i}], (void **)&${argname});`,
					`if (status != napi_ok) return nullptr;`
				);
			} break;
			case "GLsync":
			case "void *": {
				out_lines.push(
					`${argtype} ${argname} = nullptr;`,
					`status = getTypedArray(env, args[${i}], *(void **)&${argname});`,
					`if (status != napi_ok) return nullptr;`);
			} break;
			// returning a pointer:
			case "void **": {
				let underlyingtype = argtype.slice(0, -1);
				let result = { name:`${argname}_result`, type:underlyingtype };
				results.push(result);
				out_lines.push(
					`${result.type} ${result.name} = nullptr;`,
					`${argtype} ${argname} = &${result.name};`
				);
			} break;
			// typed pointer types:
			case "GLboolean *":
			case "GLsizei *":
			case "GLint *":
			case "GLuint *":
			case "GLenum *":
			case "GLfloat *": {
				if (name.slice(0,5) == "glGet") {
					let underlyingtype = argtype.slice(0, -1).trim();
					let result = { name:`${argname}_result`, type:underlyingtype };
					results.push(result);
					out_lines.push(
						`${result.type} ${result.name};`,
						`${argtype} ${argname} = &${result.name};`
					);
				} else {
					// assumed to be an array of GLint. depends on sizeof(GLint) (can be 32 or 64)
					out_lines.push(
						`${argtype} ${argname} = nullptr;`,
						`status = getTypedArray(env, args[${i}], ${argname});`
					);
				}
			} break;
			case "char*": 
			case "const char*": 
			case "GLchar *": {
				if (name.slice(0,5) == "glGet") {
					// expect a typed-array of char to write result into:
					out_lines.push(
						`${argtype} ${argname};`,
						`status = getTypedArray(env, args[${i}], ${argname});`
					);
				} else {
					// expect string or typed-array of char:
					out_lines.push(
						`${argtype} ${argname} = nullptr;`,
						`status = getCharacterArray(env, args[${i}], ${argname});`
					);
				}
			} break;
			case "GLchar **": {
				// these are all a "list of strings"
				// glShaderSource (array of strings)
				// glTransformFeedbackVaryings (array of strings)
				// glGetUniformIndices (array of strings)
				// expect JS array of strings (or Int8Array) by default:
				out_lines.push(
					`${argtype} ${argname} = nullptr;`,
					`status = getListOfStrings(env, args[${i}], ${argname});`
				);

			} break;
			default: {
				console.log("unhandled", argtype, argname, `	${ret} ${name}(${args.join(", ")})`);
				out_lines.push(`${argtype} ${argname};`);
				continue;
			} break;
		}
	}

	out_lines.push(`// ${ret} ${name}(${arg})`);

	let s_call = `${name}(${s_args.join(", ")});`
	if (ret == "void") {
		out_lines.push(s_call);
		out_lines.push("return NULL;");
	} else {
		out_lines.push(`${ret} result = ${s_call}`);
		switch(ret) {
			case "GLenum":
			case "GLboolean": 
			case "GLuint": {
				out_lines.push(
					`napi_value result_value = nullptr;`,
					`status = napi_create_uint32(env, (uint32_t)result, &result_value);`,
					`return (status == napi_ok) ? result_value : nullptr;`
				);
			} break;
			case "GLint":
			case "int": {
				out_lines.push(
					`napi_value result_value = nullptr;`,
					`status = napi_create_int32(env, (int32_t)result, &result_value);`,
					`return (status == napi_ok) ? result_value : nullptr;`
				);
			} break;
			case "double":
			case "float": {
				out_lines.push(
					`napi_value result_value = nullptr;`,
					`status = napi_create_double(env, (double)result, &result_value);`,
					`return (status == napi_ok) ? result_value : nullptr;`
				);
			} break;
			case "GLsync": 
			case "GLFWmonitor*": {
				out_lines.push(
					`napi_value result_value = nullptr;`,
					`status = napi_create_external(env, (void *)result, NULL, NULL, &result_value);`,
					`return (status == napi_ok) ? result_value : nullptr;`
				);
			} break;
		}
	}

	let s = out_lines.join("\n\t") + "\n}";
	out_blocks.push(s);

	//out_function_names.push(s_name);
	return s_name;
}


{
	const modulename = "gles3"
	const header = fs.readFileSync(path.join(__dirname, "..", "src", "GLES3", "gl3.h"), "utf-8")
	const module_header = fs.readFileSync(path.join(__dirname, "..", "src", `node-${modulename}.h`), "utf-8")

	
	let out_defines = [
	`/* THIS IS A GENERATED FILE -- DO NOT EDIT!! */`,
	`const ${modulename} = require('bindings')('${modulename}.node');`,
	`module.exports = ${modulename};`
	];

	let out_blocks = [
		`/* THIS IS A GENERATED FILE -- DO NOT EDIT!! */`,
		`#include "node-${modulename}.h"`,
	];

	let out_function_names = [];
	let out_properties = [];

	{
		// capture the #define GL_...    0x... lines
		const regex = /#define ([A-Z0-9_]+)\s+([0-9A-Fx]+)/g
		let match
		while (match = regex.exec(header)) {
			const name = match[1], val = match[2];
			out_defines.push(`${modulename}.${name.substring(3)} = ${val};`);
		}

		out_defines.push(`
gles3.getParameterName = (pname) => {
	for (let k in gles3) {
		if (gles3[k] == pname) return k
	}
	return "?"
}
	
gles3.getParameter = (pname) => {
	console.log("gles3.getParameter " + gles3.getParameterName(pname) + " 0x" + (pname).toString(16) + " = " + pname)
	// this diverts to a gles3.getX where X is a type that depends on what pname is.
	switch(pname) {
		case gles3.ALIASED_LINE_WIDTH_RANGE:
		case gles3.ALIASED_POINT_SIZE_RANGE:
		case gles3.DEPTH_RANGE:
			// TODO: Float32Array (with 2 elements)
		case gles3.BLEND_COLOR:
		case gles3.COLOR_CLEAR_VALUE:
			//	Float32Array (with 4 values)
		case gles3.COMPRESSED_TEXTURE_FORMATS:
			// Uint32Array
		case gles3.MAX_VIEWPORT_DIMS:
			case gles3.SCISSOR_BOX:
			case gles3.VIEWPORT:
				// Int32Array (with 2 elements)
		case gles3.COLOR_WRITEMASK:
			// sequence<GLboolean> (with 4 values)

		case gles3.ARRAY_BUFFER_BINDING:
		case gles3.ELEMENT_ARRAY_BUFFER_BINDING:
		case gles3.COPY_READ_BUFFER_BINDING:
		case gles3.COPY_WRITE_BUFFER_BINDING:
		case gles3.PIXEL_PACK_BUFFER_BINDING:
		case gles3.PIXEL_UNPACK_BUFFER_BINDING:
		case gles3.TRANSFORM_FEEDBACK_BUFFER_BINDING:
		case gles3.UNIFORM_BUFFER_BINDING:
			// WebGLBuffer or null
		case gles3.CURRENT_PROGRAM:
			// WebGLProgram or null
		case gles3.FRAMEBUFFER_BINDING:
		case gles3.DRAW_FRAMEBUFFER_BINDING:
		case gles3.READ_FRAMEBUFFER_BINDING:
			// WebGLFramebuffer or null
		case gles3.RENDERBUFFER_BINDING:
			// WebGLRenderbuffer or null
		case gles3.TEXTURE_BINDING_2D:
		case gles3.TEXTURE_BINDING_CUBE_MAP:
		case gles3.TEXTURE_BINDING_2D_ARRAY:
		case gles3.TEXTURE_BINDING_3D:
			// WebGLTexture or null	
		case gles3.SAMPLER_BINDING:	
			// WebGLSampler or null
		case gles3.TRANSFORM_FEEDBACK_BINDING:
			// WebGLTransformFeedback or null
		case gles3.VERTEX_ARRAY_BINDING:
			//	WebGLVertexArrayObject or null

		// case ext.VERTEX_ARRAY_BINDING_OES:
		// 	//	WebGLVertexArrayObjectOES
		// case ext.TIMESTAMP_EXT:
		// 	//	GLuint64EXT

		case gles3.RENDERER:
		case gles3.SHADING_LANGUAGE_VERSION:
		case gles3.VENDOR:
			// DOMString	

			// all of the above:
			throw ("gles3.getParameter type not yet handled: " + (pname).toString(16));

		
		case gles3.VERSION:
			return "WebGL 2.0 (OpenGL ES 3.0)"  

		case gles3.DEPTH_CLEAR_VALUE:
		case gles3.LINE_WIDTH:
		case gles3.POLYGON_OFFSET_FACTOR:
		case gles3.POLYGON_OFFSET_UNITS:
		case gles3.SAMPLE_COVERAGE_VALUE:
		case gles3.MAX_TEXTURE_LOD_BIAS:
//		case ext.MAX_TEXTURE_MAX_ANISOTROPY_EXT:
			return gles3.getFloatv(pname);

		case gles3.BLEND:
		case gles3.CULL_FACE:
		case gles3.DEPTH_TEST:
		case gles3.DEPTH_WRITEMASK:
		case gles3.DITHER:
		case gles3.POLYGON_OFFSET_FILL:
		case gles3.SAMPLE_COVERAGE_INVERT:
		case gles3.SCISSOR_TEST:
		case gles3.STENCIL_TEST:
		case gles3.UNPACK_FLIP_Y_WEBGL:
		case gles3.UNPACK_PREMULTIPLY_ALPHA_WEBGL:
		case gles3.RASTERIZER_DISCARD:
		case gles3.SAMPLE_ALPHA_TO_COVERAGE:
		case gles3.SAMPLE_COVERAGE:
		case gles3.TRANSFORM_FEEDBACK_ACTIVE:
		case gles3.TRANSFORM_FEEDBACK_PAUSED:
//		case ext.GPU_DISJOINT_EXT:
			return gles3.getBooleanv(pname);

		case gles3.MAX_CLIENT_WAIT_TIMEOUT_WEBGL:
		case gles3.MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS:
		case gles3.MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS:
		case gles3.MAX_ELEMENT_INDEX:
		case gles3.MAX_SERVER_WAIT_TIMEOUT:
		case gles3.MAX_UNIFORM_BLOCK_SIZE:
			return gles3.getInteger64v(pname);

		default: // GLint / GLenum:
			return gles3.getIntegerv(pname);
	}
}

gles3.getExtension = function(name) {
	console.log("gl.getExtension", name)
}
		`)
	}

	{
		// capture the napi functions already defined in the hand-written header:
		const regex = /napi_value\s+([A-Za-z0-9_]+)\(/g
		let match
		while (match = regex.exec(module_header)) {
			const name = match[1]
			out_function_names.push(name)
		}
	}

	{
		// capture the GL_APICALL <ret> GL_APIENTRY <name> (<args...) functions
		const regex = /GL_APICALL\s+([A-Za-z_]+)\s+GL_APIENTRY\s+([A-Za-z0-9_]+)\s*\(([^)]+)/g
		let match
		while (match = regex.exec(header)) {
			const ret = match[1], name = match[2], arg = match[3];
			const s_name = name.substring(2);

			if (!out_function_names.find(s=>s==s_name) && !blacklist[name]) {
				generate_handler(name, s_name, ret, arg, out_blocks);
				out_function_names.push(s_name);
			}
		}
	}

	//{ "${s}", 0, ${s}, 0, 0, 0, napi_default, 0 }
	//out_function_names.map(s => out_properties.push(`{ "${s}", 0, ${s}, 0, 0, 0, napi_default, 0 }`));
	out_function_names.map(s => out_properties.push(`{ "${s.charAt(0).toLowerCase()}${s.substring(1)}", 0, ${s}, 0, 0, 0, napi_default, 0 }`));
	out_blocks.push([
		`napi_value init(napi_env env, napi_value exports) {`,
		`	napi_status status;`,
		`	napi_property_descriptor properties[] = {`,
		`		${out_properties.join(",\n\t\t")}`,
		`	};`,
		`	status = napi_define_properties(env, exports, ${out_properties.length}, properties);`,
		`	//assert(status == napi_ok);`,
		`	return exports;`,
		`}`,
		`NAPI_MODULE(NODE_GYP_MODULE_NAME, init)`].join("\n"));

	fs.writeFileSync(path.join(__dirname,"..", "src", `node-${modulename}.cpp`), out_blocks.join("\n\n"), "utf-8");
	fs.writeFileSync(path.join(__dirname,"..", `${modulename}.js`), out_defines.join("\n"), "utf-8");
}


{
	const modulename = "glfw3"
	const header = fs.readFileSync(path.join(__dirname, "..", "node_modules", "native-graphics-deps", "include", "GLFW", "glfw3.h"), "utf-8")
	const module_header = fs.readFileSync(path.join(__dirname, "..", "src", `node-${modulename}.h`), "utf-8")
	

	let out_defines = [
	`/* THIS IS A GENERATED FILE -- DO NOT EDIT!! */`,
	`const ${modulename} = require('bindings')('${modulename}.node');`,
	`module.exports = ${modulename};`
	];

	let out_blocks = [
		`/* THIS IS A GENERATED FILE -- DO NOT EDIT!! */`,
		`#include "node-${modulename}.h"`,
	];

	let out_function_names = [];
	let out_properties = [];
	
	{
		// GLFW defines 
		const regex = /#define\s+(GLFW_[A-Za-z_]+)\s+([-a-z0-9x_]+)/g
		let match
		while (match = regex.exec(header)) {
			const name = match[1], val = match[2]
			out_defines.push(`${modulename}.${name.substring(5)} = ${val};`);
		}
	}

	{
		// capture the napi functions already defined in the hand-written header:
		const regex = /napi_value\s+([A-Za-z0-9_]+)\(/g
		let match
		while (match = regex.exec(module_header)) {
			const name = match[1]
			out_function_names.push(name)
		}
	}

	{
		// capture the GLFWAPI <ret> <name> (<args...) functions
		const regex = /GLFWAPI\s+([A-Za-z_*]+)\s+([A-Za-z0-9_]+)\s*\(([^)]+)/g
		let match
		while (match = regex.exec(header)) {
			const ret = match[1], name = match[2], arg = match[3];
			const s_name = name.substring(4);
			if (!out_function_names.find(s=>s==s_name) && !blacklist[name]) {
				generate_handler(name, s_name, ret, arg, out_blocks);
				out_function_names.push(s_name);
			}
		}
	}

	//{ "${s}", 0, ${s}, 0, 0, 0, napi_default, 0 }
	//out_function_names.map(s => out_properties.push(`{ "${s}", 0, ${s}, 0, 0, 0, napi_default, 0 }`));
	out_function_names.map(s => out_properties.push(`{ "${s.charAt(0).toLowerCase()}${s.substring(1)}", 0, ${s}, 0, 0, 0, napi_default, 0 }`));
	out_blocks.push([
	`napi_value init(napi_env env, napi_value exports) {`,
	`	napi_status status;`,
	`	napi_property_descriptor properties[] = {`,
	`		${out_properties.join(",\n\t\t")}`,
	`	};`,
	`	status = napi_define_properties(env, exports, ${out_properties.length}, properties);`,
	`	//assert(status == napi_ok);`,
	`	return exports;`,
	`}`,
	`NAPI_MODULE(NODE_GYP_MODULE_NAME, init)`].join("\n"));

	fs.writeFileSync(path.join(__dirname,"..", "src", `node-${modulename}.cpp`), out_blocks.join("\n\n"), "utf-8");
	fs.writeFileSync(path.join(__dirname,"..", `${modulename}.js`), out_defines.join("\n"), "utf-8");
}


{
	const modulename = "openvr"
	//const header = fs.readFileSync(path.join(__dirname, "..", "node_modules", "native-graphics-deps", "include", "GLFW", "glfw3.h"), "utf-8")
	const module_header = fs.readFileSync(path.join(__dirname, "..", "src", `node-${modulename}.h`), "utf-8")

	// Handily, the entire openvr API is also exposed as a JSON file
	const openvr_api = JSON.parse(fs.readFileSync(path.join(__dirname, "../", "node_modules", "native-openvr-deps", "headers", "openvr_api.json"), "utf-8"));

	let out_defines = [
	`/* THIS IS A GENERATED FILE -- DO NOT EDIT!! */`,
	`const ${modulename} = require('bindings')('${modulename}.node');`,
	`module.exports = ${modulename};`
	];

	let out_blocks = [
		`/* THIS IS A GENERATED FILE -- DO NOT EDIT!! */`,
		`#include "node-${modulename}.h"`,
	];

	let out_function_names = [];
	let out_properties = [];
	

	for (let o of openvr_api.typedefs) {
		let name = o.typedef;
		// o.type
	}

	for (let o of openvr_api.enums) {
		let e = {}
		for (let v of o.values) {
			e[+v.value] = v.name 
		}
		let name = o.enumname
		
	}

	for (let o of openvr_api.consts) {
		let name = o.constname;
		//assert(!used[name], name)
		//used[name] = true;
		// o.consttype
		// o.constval
	  
		//json[name] = (typeof +o.constval == "number") ? +o.constval : o.constval
	  }

	for (let o of openvr_api.structs) {
		let name = o.struct;
		if (name == "vr::(anonymous)") continue;
		// o.fields = []
	}

	
	for (let o of openvr_api.methods) {
		let name = `${o.classname}_${o.methodname}`
		if (name == "vr::(anonymous)") continue;
		/*
		used[name] = true;
	
		let params = []
		for (let i in o.params) {
			let p = o.params[i]
			// p.paramname
			// p.paramtype
			params[i] = p
		}
		
		functions[name] = {
			classname: o.classname,
			methodname: o.methodname,
			returntype: o.returntype,
			params: params
		}
	
		
		Generate a Napi method for this function
		*/
	

	}

	
	// {
	// 	// GLFW defines 
	// 	const regex = /#define\s+(GLFW_[A-Za-z_]+)\s+([-a-z0-9x_]+)/g
	// 	let match
	// 	while (match = regex.exec(header)) {
	// 		const name = match[1], val = match[2]
	// 		out_defines.push(`${modulename}.${name.substring(5)} = ${val};`);
	// 	}
	// }

	{
		// capture the napi functions already defined in the hand-written header:
		const regex = /napi_value\s+([A-Za-z0-9_]+)\(/g
		let match
		while (match = regex.exec(module_header)) {
			const name = match[1]
			out_function_names.push(name)
		}
	}

	// {
	// 	// capture the GLFWAPI <ret> <name> (<args...) functions
	// 	const regex = /GLFWAPI\s+([A-Za-z_*]+)\s+([A-Za-z0-9_]+)\s*\(([^)]+)/g
	// 	let match
	// 	while (match = regex.exec(header)) {
	// 		const ret = match[1], name = match[2], arg = match[3];
	// 		const s_name = name.substring(4);
	// 		if (!out_function_names.find(s=>s==s_name) && !blacklist[name]) {
	// 			generate_handler(name, s_name, ret, arg, out_blocks);
	// 			out_function_names.push(s_name);
	// 		}
	// 	}
	// }

	//{ "${s}", 0, ${s}, 0, 0, 0, napi_default, 0 }
	//out_function_names.map(s => out_properties.push(`{ "${s}", 0, ${s}, 0, 0, 0, napi_default, 0 }`));
	out_function_names.map(s => out_properties.push(`{ "${s.charAt(0).toLowerCase()}${s.substring(1)}", 0, ${s}, 0, 0, 0, napi_default, 0 }`));
	out_blocks.push([
	`napi_value init(napi_env env, napi_value exports) {`,
	`	napi_status status;`,
	`	napi_property_descriptor properties[] = {`,
	`		${out_properties.join(",\n\t\t")}`,
	`	};`,
	`	status = napi_define_properties(env, exports, ${out_properties.length}, properties);`,
	`	//assert(status == napi_ok);`,
	`	return exports;`,
	`}`,
	`NAPI_MODULE(NODE_GYP_MODULE_NAME, init)`].join("\n"));

	fs.writeFileSync(path.join(__dirname,"..", "src", `node-${modulename}.cpp`), out_blocks.join("\n\n"), "utf-8");
	fs.writeFileSync(path.join(__dirname,"..", `${modulename}.js`), out_defines.join("\n"), "utf-8");
}