#pragma once

#include <array>
#include <string>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <filesystem>

#include "utils/common.h"
#include "glad/glad.h"

namespace cyx {

	enum class ShaderType {
		NONE,
		VERTEX, FRAGMENT, 
		COUNT
	};

	class Shader {
	public:	
		
		enum DataType {

		};


		/*
		*  use #pragma vertex on your .glsl file to hint the start the vertex shader
		*  use #pragma fragment on your .glsl file to hint the start the fragment shader
		*/
		Shader(const char* filepath="assets/shaders/default.glsl");
		
		// simply pass the src directly for both shaders
 		Shader(const char* vertex_src, const char* frag_src);
 		Shader(const std::string& vertex_src, const std::string& frag_src);
 		~Shader();

		auto compile() -> void;

		auto valid(GLuint id) -> bool; 
		inline operator GLuint () { return this->_programID; }
		// bind actually selects all the 'sub shaders' (frag, geo, vertex) as the current program
		auto bind()  const -> void;
		// undbind selects 'none' program, meaning is uses program null or 0;
		auto unbind() const -> void;
		// src gets the string source for a shader of a certain type
		auto src(ShaderType type) -> const char*;
		
		
		/* 
		*	BEFORE using any 'Uniform', the shader must be bound
		*	// TODO(Everton): Show a warnning when tring to set uniform before 'bind()'
		*/
		template<DataType T>
		auto uniform(const char* var, void* data) -> void;

		
		auto uniform_intarray() -> void;



		auto uniform_mat4(const char *var, f32 *values, bool transpose=false) -> void;

		auto uniform_int1(const char *var, i32 value) -> void;

		auto uniform_float4(const char* var, float* values) -> void;
		auto uniform_float3(const char* var, f32* values) -> void;
		
		auto uniform_float2(const char* var, f32* values) -> void;
		
		auto uniform_float(const char* var, f32 value) -> void;
		
		template<u32 dimension>
		auto uniform_int(const char* var, i32 values[dimension]) -> void
		{
			i32 location;
			if (dimension == 1) {
				location = glGetUniformLocation(_programID, var);
				glUniform1i(location, values[0]);
			}
			if (dimension > 1) {
				location = glGetUniformLocation(_programID, var);
				glUniform1iv(location, dimension, values);

			}
			else {
				assert(0 && "Uniform Int has no support for negative dimensions (count)");
			}
		}

	public:
		std::string _vertex_source;
		std::string _fragment_source;
		std::string _filepath;
		u32 _programID = -1;
		mutable bool _isbound = false;
		/* compile a shader and return its shaderID 
		*  return: '0' if coudnt compile the shader
		*/
		auto compile_shader(u32 type, const char* src) -> u32;
		/* create the shader program using all shaders provided
		* return: boolean 'true' if success, 'false' otherwise
		*/
		auto create_program(const char* vertex_shader_src, const char* frag_shader_src) -> bool;
		auto set_default(ShaderType type) -> void;

		//auto uniform_float(const char* var, u32 count, f32* values) -> void {}
		
	};
}