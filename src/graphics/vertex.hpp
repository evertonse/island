#pragma once
#include <vector>
#include "glad.h"
#include "utils/common.h"
#include "graphics/rendering.hpp"
#include "graphics/vertex.hpp"

namespace cyx {

	class VertexBuffer {
	public:
		VertexBuffer();
		VertexBuffer(void* data, u32 size);
		~VertexBuffer();
		
		auto set_data(void* data, u32 size) -> void;
		auto set_attr(u32 location, i32 count, i32 stride_size, i32 offset_size) -> void;

		auto bind()		-> void;
		auto unbind()	-> void;

		private:
		mutable bool _is_bound = false;		
		u32 _vertex_bufferID;
	};

	
	class VertexIndex {
		public:
		VertexIndex();
		~VertexIndex();
		auto set_data(void* data, u32 size) -> void;

		auto bind() const -> void;
		auto unbind() const -> void;
		
		auto count() const -> u32;
		auto set_count(u32)-> void;
		private:
		mutable bool _is_bound = false;		
		u32 _vertex_indexID;
		u32 _count;
	};
	

	struct VertexBufferAttr {
		
		enum Type {
			None = 0, 
			Float, Float2, Float3, Float4, 
			Int, Int2, Int3, Int4, 
			Bool,
			Mat3, Mat4 
		};

		Type 	type = Type::None;
		bool 	normalized = false;
		u32 	offset = 0;

		VertexBufferAttr(Type type, bool normalized = false) 
			:type(type),normalized(normalized)
		{}

		auto size()   const -> u32 const;
		auto count()  const -> u32 const;

		auto to_string() const -> const std::string;

		static auto to_glenum(Type type) -> GLenum {
			switch (type) {
				case Float:    return GL_FLOAT;
				case Float2:   return GL_FLOAT;
				case Float3:   return GL_FLOAT;
				case Float4:   return GL_FLOAT;
				case Mat3:     return GL_FLOAT;
				case Mat4:     return GL_FLOAT;
				case Int:      return GL_INT;
				case Int2:     return GL_INT;
				case Int3:     return GL_INT;
				case Int4:     return GL_INT;
				case Bool:     return GL_BOOL;
			}
			return 0;
		}
		
		static auto size_of_type(Type type) -> u32 {
			switch (type) {
				case Float :  return 4;
				case Float2:  return 4*2;
				case Float3:  return 3*3;
				case Float4:  return 4*4;
				
				case Int   :  return 4;
				case Int2  :  return 4*2;
				case Int3  :  return 4*3;
				case Int4  :  return 4*4;
				
				case Bool  :  return 1;

				case Mat3  :  return 4*3*3;
				case Mat4  :  return 4*4*4; 
			}

			assert(0 && "WTF you passed some bullshit here, try an actual type");
		}

		static auto count_of_type(Type type) -> u32 {
				switch (type) {
				case Float :  return 1;
				case Float2:  return 2;
				case Float3:  return 3;
				case Float4:  return 4;
				
				case Int   :  return 1;
				case Int2  :  return 2;
				case Int3  :  return 3;
				case Int4  :  return 4;
				
				case Bool  :  return 1;

				case Mat3  :  return 3; // 3 of Float3
				case Mat4  :  return 4; // 4 of Float4
			}
			assert(0 && "WTF you passed some bullshit here, try an actual type");
		}
	


	};

	class VertexBufferLayout {
	
	public:
		VertexBufferLayout();

		VertexBufferLayout(std::initializer_list<VertexBufferAttr> attrs);

		auto attrs() -> const std::vector<VertexBufferAttr>&;
		auto stride() const -> u32;
		auto add(VertexBufferAttr& attr) -> void;
		auto add(std::initializer_list<VertexBufferAttr> attrs) -> void;
		auto add(VertexBufferAttr &&attr) -> void;

	private:
		auto calculate() -> void;

	private:
		std::vector<VertexBufferAttr> _attrs;
		u32 _stride = 0;
	};

	
	class VertexArray {
	public:
		VertexArray();
		~VertexArray();
		
		auto bind() const-> void;
		auto unbind() const-> void;

		auto add(VertexBuffer& vb , VertexBufferLayout& layout)-> void;
	private:		
		u32 _vertex_arrayID;
		mutable bool _is_bound = false;
	};
}
