#pragma once
#include "utils/common.h"
#include "graphics/window.hpp"
#include "graphics/vertex.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"

#if defined (CYX_GL_DEBUG) 
    #define glLogCall(x) glClearErrors();x;glPrintErrors(#x,__FILE__,__LINE__)
    #define glCheckCall(x) x; if(!GLAD_GL_KHR_debug){ glCheckError(__FILE__, __LINE__, #x) };
#else
    #define glLogCall(x) x
    #define glCheckCall(x) x
#endif 

namespace cyx {
	class VertexArray; 
	class VertexIndex;
	class Shader;
	class Texture;

	static void 
	glClearErrors(){
		while(glGetError() != GL_NO_ERROR);
	}
	
	static void 
	glPrintErrors(const char* function_name, const char* file_name, int line){
		GLenum error;
		error = glGetError();
		while(error != GL_NO_ERROR) {
			std::cout << "[GL Error]" <<"errocode: 0x"<< std::hex << error 
				<< " at function: " << function_name 
				<< " on file: " << file_name 
				<< " on line: " << line 
				<< std::endl;  
			error = glGetError();
		}
	}
	
	class Renderer {
	public:
		Renderer();
		~Renderer();

		void clear(void);
		auto enable_blending() -> void;
		void set_clear_color(f32 r, f32 g, f32 b, f32 a);
		auto set_viewport(i32 x, i32 y, u32 width, u32 height)  -> void;
		auto present() -> void;
		auto draw(const VertexArray& va, const VertexIndex& vi, const Shader& shader) -> void;
		auto draw(const VertexArray& va, const VertexIndex& vi, const Shader& shader, const Texture& tex) -> void;
		

	private:
		union {
			u32 _color;
			u8 r,g,b,a;
		};
	};


}
