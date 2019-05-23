#ifndef AL_GL_H
#define AL_GL_H

// note: GLAD header generated from http://glad2.dav1d.de
// for OpenGL 4.1, Compatibility, header-only enabled
// GLAD_GLAPI_EXPORT ensures that declspec(dllimport) or declspec(dllexport) is added to the gl functions
#define GLAD_GLAPI_EXPORT
#ifdef AL_IMPLEMENTATION
	// if AL_IMPLEMENTATION is defined we are building the alice.exe / alice.lib
	// GLAD_GLAPI_EXPORT_BUILD ensures that declspec(dllexport) is added to the gl functions
	#define GLAD_GLAPI_EXPORT_BUILD
	// GLAD_GL_IMPLEMENTATION ensures that the glLoader function implementations are also generated
	#define GLAD_GL_IMPLEMENTATION
#endif
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "al_math.h"
#include "al_console.h"

#define check_gl_error() _check_gl_error(__FILE__,__LINE__)
void _check_gl_error(const char *file, int line) {
	GLenum err (glGetError());
	while(err!=GL_NO_ERROR) {
		std::string error;
		switch(err) {
			case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
			case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
			case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
			case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
		}
		console.error("GL_%s - %s:%d", error.c_str(), file, line);
		err = glGetError();
	}
}

bool fbo_check() {
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
			console.log("failed to create render to texture target GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
		}
		//else if (status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS) {
		//	console.log("failed to create render to texture target GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS");
		//}
		else if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
			console.log("failed to create render to texture target GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
		}
		else if (status == GL_FRAMEBUFFER_UNSUPPORTED) {
			console.log("failed to create render to texture target GL_FRAMEBUFFER_UNSUPPORTED");
		}
		else {
			console.log("failed to create render to texture target %d", status);
		}
		return false;
	}
	return true;
}

template<typename T>   // primary template
GLenum gl_type();

template<> GLenum gl_type<double>() { return GL_DOUBLE; }
template<> GLenum gl_type<float>() { return GL_FLOAT; }
template<> GLenum gl_type<glm::vec2>() { return GL_FLOAT; }
template<> GLenum gl_type<glm::vec3>() { return GL_FLOAT; }
template<> GLenum gl_type<glm::vec4>() { return GL_FLOAT; }
template<> GLenum gl_type<glm::quat>() { return GL_FLOAT; }
template<> GLenum gl_type<glm::mat2>() { return GL_FLOAT; }
template<> GLenum gl_type<glm::mat3>() { return GL_FLOAT; }
template<> GLenum gl_type<glm::mat4>() { return GL_FLOAT; }

size_t gl_type_size(GLenum t) {
	switch(t) {
		case GL_FLOAT:
		case GL_INT:
		case GL_UNSIGNED_INT: return sizeof(int32_t);

		case GL_DOUBLE: return sizeof(double);

		case GL_SHORT:
		case GL_UNSIGNED_SHORT: return sizeof(int16_t);

		case GL_BYTE:
		case GL_UNSIGNED_BYTE: 
		default: return sizeof(int8_t);
	}
}

struct Viewport {
	glm::ivec2 pos; // "pixel" coordinate of bottom-left corner
	glm::ivec2 dim; // "pixel" width & height of viewport
};


/*
	Typical usage:

	// size in bytes of the underlying data
	VBO vbo(size);
	VBO vbo(size, ptr); // alloc with data too
	// or:
	VBO vbo;
	vbo.resize(size);
	vbo.submit(ptr);
	// or 
	vbo.submit(ptr, size);
*/
struct VBO {
	unsigned int id = 0;
	size_t size = 0;
	void * src = 0;

	VBO() {}
	VBO(size_t size, void * src=NULL) : size(size), src(src) {}

	void dest_changed() {
		if (size <= 0) {
			console.error("VBO size not yet declared, cannot allocate");
			return;
		}
		dest_closing();
		glGenBuffers(1, &id);
		submit();
		unbind();
	}

	void dest_closing() {
		if (id) {
			glDeleteBuffers(1, &id);
			id = 0;
		}
	}

	void bind() { 
		if (!id) dest_changed();
		glBindBuffer(GL_ARRAY_BUFFER, id); 
	}
	void unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

	void resize(size_t s) {
		if (s > size) dest_closing();
		size = s;
	}

	// submit() will re-use any existing data
	// submit(ptr) will update the cached data
	void submit(void * data = NULL, size_t s=0) {
		if (data) src=data;
		if (s) resize(s);
		bind();
		glBufferData(GL_ARRAY_BUFFER, size, src, GL_STATIC_DRAW);
	}
};

struct EBO {
	unsigned int id = 0;
	size_t count = 0;
	unsigned int * src = 0;

	EBO() {}
	EBO(size_t count, unsigned int * src=NULL) : count(count), src(src) {}

	void dest_changed() {
		if (count <= 0) {
			console.error("VBO size not yet declared, cannot allocate");
			return;
		}
		dest_closing();
		glGenBuffers(1, &id);
		submit();
		unbind();
	}

	void dest_closing() {
		if (id) {
			glDeleteBuffers(1, &id);
			id = 0;
		}
	}

	void bind() { 
		if (!id) dest_changed();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); 
	}
	void unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

	void resize(size_t c) {
		if (c > count) dest_closing();
		count = c;
	}

	void submit(unsigned int * data=NULL, size_t c=0) {
		if (data) src=data;
		if (c) resize(c);
		bind();
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, c * sizeof(unsigned int), src, GL_STATIC_DRAW);
	}
};

/*
	A Vertex Array Object (VAO) helps match the components of VBOs to the attributes of Shaders

	Usage:

	VAO vao;

	vao.bind();
	vbo.bind();
	// set up attrs
	vao.attr(...);
	vao.unbind();

	vao.draw(n);
	or
	vao.drawInstanced(n, m);
*/	
struct VAO {
	unsigned int id = 0;

	void bind() { 
		if (!id) dest_changed();
		glBindVertexArray(id); 
	}
	void unbind() { glBindVertexArray(0); }

	void dest_closing() {
		if (id) {
			glDeleteVertexArrays(1, &id);
			id = 0;
		}
	}

	void dest_changed() {
		dest_closing();
		glGenVertexArrays(1, &id);
	}

	// assumes vao and vbo are already bound
	void attr(GLuint location=0, GLint size=3, GLenum type=GL_FLOAT, size_t stride=3*sizeof(float), size_t offset=0, bool instanced=false) {
		// assumes vao and vbo are already bound
		glEnableVertexAttribArray(location);
		// set the data layout
		glVertexAttribPointer(location, (GLsizei)size, type, GL_FALSE, (GLsizei)stride, (void*)offset); 
		// mark this attrib as being per-instance	
		if (instanced) glVertexAttribDivisor(location, 1);  
	}

	// assumes vao and vbo are already bound
	template<typename T, typename U> 
	void attr(GLuint location, U T::*member, bool instanced=false) {
		GLenum type = gl_type<U>();
		attr(location, al_sizeof(member) / gl_type_size(type), type, sizeof(T), al_offsetof(member), instanced);
	}

	void draw(GLsizei num_vertices, GLenum primitive=GL_TRIANGLES) {
		bind();
		glDrawArrays(primitive, 0, num_vertices); 
	}
	
	void drawInstanced(GLsizei num_vertices, GLsizei num_instances, GLenum primitive=GL_TRIANGLES) {
		bind();
		glDrawArraysInstanced(primitive, 0, num_vertices, num_instances); 
	}

	void drawElements(GLsizei num_elements, GLenum primitive=GL_TRIANGLES) {
		bind();
   	 	glDrawElements(primitive, num_elements, GL_UNSIGNED_INT, 0);
	}
};

struct Shader {
	GLuint program = 0;

	std::string vertCode = "";
    std::string fragCode = "";
    std::string geomCode = "";

	// constructor just applies the code strings
	Shader(std::string vertCode = defaultVertexCode(), std::string fragCode = defaultFragmentCode(), std::string geomCode = "") 
		: vertCode(vertCode), fragCode(fragCode), geomCode(geomCode) 
		{}

	~Shader() {
		dest_closing();
	}
	
	void dest_changed() {

		dest_closing();
       	
       	GLuint vid, fid, gid=0;
       	
       	const char * vcode = vertCode.c_str();
       	vid = glCreateShader(GL_VERTEX_SHADER);
       	glShaderSource(vid, 1, &vcode, NULL);
       	glCompileShader(vid);
        checkCompileErrors(vid, "VERTEX");
        
       	const char * fcode = fragCode.c_str();
       	fid = glCreateShader(GL_FRAGMENT_SHADER);
       	glShaderSource(fid, 1, &fcode, NULL);
       	glCompileShader(fid);
        checkCompileErrors(fid, "FRAGMENT");
        
       	if (!geomCode.empty()) {
            const char * gcode = geomCode.c_str();
            gid = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(gid, 1, &gcode, NULL);
            glCompileShader(gid);
            checkCompileErrors(gid, "GEOMETRY");
        }
        
        // shader Program
        program = glCreateProgram();
        glAttachShader(program, vid);
        glAttachShader(program, fid);
        if (gid > 0) glAttachShader(program, gid);
        glLinkProgram(program);
        checkCompileErrors(program, "PROGRAM");
       	
       	// delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vid);
        glDeleteShader(fid);
        if(gid > 0) glDeleteShader(gid);
    }

	void dest_closing() {
		if (program) {
			glDeleteProgram(program);
			program = 0;
		}
	}
    
    void use() { 
		if (!program) dest_changed();
		if (program) glUseProgram(program); 
	}
    static void unuse() { 
		glUseProgram(0); 
	} 
    
    void uniform(const std::string &name, bool value) const { glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value); }
    void uniform(const std::string &name, int value) const { glUniform1i(glGetUniformLocation(program, name.c_str()), value); }
    void uniform(const std::string &name, float value) const { glUniform1f(glGetUniformLocation(program, name.c_str()), value); }
    void uniform(const std::string &name, double value) const { glUniform1f(glGetUniformLocation(program, name.c_str()), (GLfloat)value); }
    void uniform(const std::string &name, const glm::vec2 &value) const { glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);  }
    void uniform(const std::string &name, const glm::vec3 &value) const { glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);  }
    void uniform(const std::string &name, const glm::vec4 &value) const { glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);  }
    void uniform(const std::string &name, const glm::quat &value) const { glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);  }
    void uniform(const std::string &name, float x, float y) { glUniform2f(glGetUniformLocation(program, name.c_str()), x, y); }
    void uniform(const std::string &name, float x, float y, float z) { glUniform3f(glGetUniformLocation(program, name.c_str()), x, y, z); }
    void uniform(const std::string &name, float x, float y, float z, float w) { glUniform4f(glGetUniformLocation(program, name.c_str()), x, y, z, w); }
    void uniform(const std::string &name, const glm::mat2 &mat) const { glUniformMatrix2fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
    void uniform(const std::string &name, const glm::mat3 &mat) const { glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
    void uniform(const std::string &name, const glm::mat4 &mat) const { glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &mat[0][0]); }
    
	static void checkCompileErrors(GLuint shader, std::string type) {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                console.error("GLSL shader compile error: %s ", type.c_str()); 			
                console.error("%s", infoLog);
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                console.error("GLSL program linker error: %s ", type.c_str()); 			
                console.error("%s", infoLog);
            }
        }
    }

	void readFiles(std::string vertPath, std::string fragPath, std::string geomPath="") {
		console.log("shader loading %s %s", vertPath.c_str(), fragPath.c_str());
        std::string vCode, fCode, gCode;
		std::ifstream vFile, fFile, gFile;
        // ensure ifstream objects can throw exceptions:
        vFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        gFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            vFile.open(vertPath);
            fFile.open(fragPath);
            std::stringstream vStream, fStream;
            // read file's buffer contents into streams
            vStream << vFile.rdbuf();
            fStream << fFile.rdbuf();		
            // close file handlers
            vFile.close();
            fFile.close();
            // convert stream into string
            vertCode = vStream.str();
            fragCode = fStream.str();	
            // if geometry shader path is present, also load a geometry shader
            if(!geomPath.empty())
            {
                gFile.open(geomPath);
                std::stringstream gStream;
                gStream << gFile.rdbuf();
                gFile.close();
                geomCode = gStream.str();
            }
			// make sure the next use of the shader uses the new code:
			dest_closing();
        } catch (std::ifstream::failure e) {
            console.error("shader file read failed");
        }
	}

	static const char * defaultVertexCode() {
		return R"(
			#version 330 core
			layout (location = 0) in vec2 aPos;
			layout (location = 1) in vec2 aTexCoord;
			
			out vec2 texCoord;
			
			void main() {
				gl_Position = vec4(aPos, 0., 1.0);
				texCoord = aTexCoord;
			}
		)";
	}
	static const char * defaultFragmentCode() {
		return R"(
			#version 330 core
			out vec4 FragColor;
			in vec2 texCoord;
			uniform sampler2D tex;

							void main() {
				//FragColor = vec4(texCoord, 0.5, 1.);
				FragColor = texture(tex, texCoord);
			}
		)";
	}
};

struct FloatTexture3D {

	GLuint id;
	GLenum wrap = GL_REPEAT;

	void dest_closing() {
		if (id) {
			glDeleteTextures(1, &id);
			id = 0;
		}
	}

	void dest_changed() {
		dest_closing();
		
		glGenTextures(1, &id);

		glBindTexture(GL_TEXTURE_3D, id);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);		
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap);  
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap);  
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap);  
		glTexParameteri( GL_TEXTURE_3D, GL_GENERATE_MIPMAP, GL_TRUE ); 
		glBindTexture(GL_TEXTURE_3D, 0);
	}

	void bind(GLenum texture_unit = 0) {
		if (!id) dest_changed();
		glActiveTexture(GL_TEXTURE0+texture_unit);
		glBindTexture(GL_TEXTURE_3D, id);
	}
	static void unbind(GLenum texture_unit = 0) {
		glActiveTexture(GL_TEXTURE0+texture_unit);
		glBindTexture(GL_TEXTURE_3D, 0);
	}
	
	// single-plane:
	void submit(glm::ivec3 dim, float * data, bool generateMipMap=false) {
		bind();
		glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, dim.x, dim.y, dim.z, 0, GL_RED, GL_FLOAT, data);
		if(generateMipMap) glGenerateMipmap(GL_TEXTURE_3D);  
		unbind();
	}
	
	// 3-plane:
	void submit(glm::ivec3 dim, glm::vec3 * data, bool generateMipMap=false) {
		bind();
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, dim.x, dim.y, dim.z, 0, GL_RGB, GL_FLOAT, data);
		if(generateMipMap) glGenerateMipmap(GL_TEXTURE_3D);  
		unbind();
	}
	
	// 4-plane:
	void submit(glm::ivec3 dim, glm::vec4 * data, bool generateMipMap=false) {
		bind();
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, dim.x, dim.y, dim.z, 0, GL_RGBA, GL_FLOAT, data);
		if(generateMipMap) glGenerateMipmap(GL_TEXTURE_3D);  
		unbind();
	}
};

struct FloatTexture2D {

	GLuint id;
	GLenum wrap = GL_REPEAT;

	void dest_closing() {
		if (id) {
			glDeleteTextures(1, &id);
			id = 0;
		}
	}

	void dest_changed() {
		dest_closing();
		
		glGenTextures(1, &id);

		glBindTexture(GL_TEXTURE_2D, id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);  
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);  
		glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE ); 
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void bind(GLenum texture_unit = 0) {
		if (!id) dest_changed();
		glActiveTexture(GL_TEXTURE0+texture_unit);
		glBindTexture(GL_TEXTURE_2D, id);
	}
	static void unbind(GLenum texture_unit = 0) {
		glActiveTexture(GL_TEXTURE0+texture_unit);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	// single-plane:
	void submit(glm::ivec2 dim, float * data, bool generateMipMap=false) {
		bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dim.x, dim.y, 0, GL_RED, GL_FLOAT, data);
		if(generateMipMap) glGenerateMipmap(GL_TEXTURE_2D);  
		unbind();
	}
	
	// 3-plane:
	void submit(glm::ivec2 dim, glm::vec3 * data, bool generateMipMap=false) {
		bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, dim.x, dim.y, 0, GL_RGB, GL_FLOAT, data);
		if(generateMipMap) glGenerateMipmap(GL_TEXTURE_2D);  
		unbind();
	}
	
	// 4-plane:
	void submit(glm::ivec2 dim, glm::vec4 * data, bool generateMipMap=false) {
		bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, dim.x, dim.y, 0, GL_RGBA, GL_FLOAT, data);
		if(generateMipMap) glGenerateMipmap(GL_TEXTURE_2D);  
		unbind();
	}
};

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};

float positions_cube[] = {
    -1.0f,-1.0f,-1.0f, 
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, 
    
    1.0f, 1.0f,-1.0f, 
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f, 
    
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    
    1.0f, 1.0f,-1.0f,
    1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    
    1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
    
    -1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f,-1.0f,
    
    1.0f,-1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f,
    
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
    
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f, 1.0f, 1.0f,
    
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f,-1.0f, 1.0f
};

struct SimpleMesh {
	unsigned int VAO, VBO, EBO;
	size_t vertex_count, index_count;
	
	SimpleMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
		setup(vertices, indices);
	}
	
	void setup(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
		setup(&vertices[0], vertices.size(), &indices[0], indices.size());
	}
	
	void setup(Vertex * vertices, size_t vertex_count, unsigned int * indices, size_t index_count) {
		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), vertices, GL_STATIC_DRAW);  

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

		// vertex positions
		glEnableVertexAttribArray(0);	
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);	
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);	
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));

		glBindVertexArray(0);
		
		this->vertex_count = vertex_count;
		this->index_count = index_count;
	}  
	
	void drawArrays() {
		glBindVertexArray(VAO);
   	 	glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertex_count);
    	glBindVertexArray(0);
	} 
	
	void draw() {
		glBindVertexArray(VAO);
   	 	glDrawElements(GL_TRIANGLES, (GLsizei)index_count, GL_UNSIGNED_INT, 0);
    	glBindVertexArray(0);
	}
};

struct QuadMesh {
	// define the VAO 
	// (a VAO stores attrib & buffer mappings in a re-usable way)
	GLuint VAO = 0;
	GLuint VBO = 0;
	glm::vec4 vertices[6];

	QuadMesh() {
		// positions, texcoords
		vertices[0] = glm::vec4( 1.,  1., 		1., 1.);
		vertices[1] = glm::vec4(-1.,  1.,		0., 1.);
		vertices[2] = glm::vec4(-1., -1.,		0., 0.);

		vertices[3] = glm::vec4(-1., -1.,		0., 0.);
		vertices[4] = glm::vec4( 1., -1.,		1., 0.);
		vertices[5] = glm::vec4( 1.,  1.,		1., 1);
	}

	void dest_closing() {
		if (VAO) {
			glDeleteVertexArrays(1, &VAO);
			VAO = 0;
		}
		if (VBO) {
			glDeleteBuffers(1, &VBO);
			VBO = 0;
		}
	}

	bool dest_changed() {
		dest_closing();

		// define the VAO 
		// (a VAO stores attrib & buffer mappings in a re-usable way)
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		// define the VBO while VAO is bound:
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// attr location 
		glEnableVertexAttribArray(0);
		// set the data layout
		// attr location, element size & type, normalize?, source stride & offset
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		// set the data layout
		// attr location, element size & type, normalize?, source stride & offset
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		return true;
	}

	void draw() {
		glBindVertexArray(VAO);
		// offset, vertex count
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
};

struct SimpleFBO {
	GLuint fbo = 0, rbo = 0, tex = 0;
	glm::ivec2 dim = glm::ivec2(1024, 1024);
	
	Shader * texDrawShader = 0;
	QuadMesh * quadMesh = 0;
	
	void dest_closing() {
		if (quadMesh) {
			quadMesh->dest_closing();
			delete quadMesh;
			quadMesh = 0;
		}
		if (texDrawShader) {
			delete texDrawShader;
			texDrawShader = 0;
		}
		if (fbo) {
			glDeleteFramebuffers(1, &fbo);
			fbo = 0;
		}
		if (rbo) {
			glDeleteRenderbuffers(1, &tex);
			tex = 0;
		}
		if (tex) {
			glDeleteTextures(1, &tex);
			tex = 0;
		}

		texDrawShader = 0;
	}

	bool dest_changed() {
		dest_closing();

		// make a framebuffer:

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, dim.x, dim.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, dim.x, dim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

		bool result = fbo_check();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return result;
	}

	bool begin() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
		return fbo_check();
	}

	void end(bool generate_mipmaps=true) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		if (generate_mipmaps) {
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
	
	void blit(int targetwidth, int targetheight, GLuint target=0) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target); // Normal FBO can be the default FBO too.
		glBlitFramebuffer(0, 0, dim.x, dim.y, 0, 0, targetwidth, targetheight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		
	}
	
	void draw(glm::vec2 scale=glm::vec2(1.f), glm::vec2 offset=glm::vec2(0.f)) {
		
		if (!texDrawShader) {
			const char * vp = R"(
				#version 330 core
				uniform vec2 uScale, uOffset;
				layout (location = 0) in vec2 aPos;
				layout (location = 1) in vec2 aTexCoord;
				
				out vec2 texCoord;
				
				void main() {
					gl_Position = vec4(aPos*uScale+uOffset, 0., 1.0);
					texCoord = aTexCoord;
				}
			)";
			const char * fp = R"(
				#version 330 core
				out vec4 FragColor;
				in vec2 texCoord;
				uniform sampler2D tex;

								void main() {
					//FragColor = vec4(texCoord, 0.5, 1.);
					FragColor = texture(tex, texCoord);
				}
			)";
			texDrawShader = new Shader(vp, fp);
		}
	
		texDrawShader->use();
		texDrawShader->uniform("uScale", scale);
		texDrawShader->uniform("uOffset", offset);
		draw_no_shader();
		texDrawShader->unuse();
	}

	void draw_no_shader() {
		
		if (!quadMesh) {
			quadMesh = new QuadMesh;
			quadMesh->dest_changed();
		}
		glBindTexture(GL_TEXTURE_2D, tex);
		quadMesh->draw();
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};



#endif