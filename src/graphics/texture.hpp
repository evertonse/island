#pragma once
#include "utils/common.h"
#include "stb_image.h"
#include "graphics/rendering.hpp"

namespace cyx {
	class Texture {
	public:
		Texture() {};
		Texture(const char* filepath);
		~Texture();

		auto slot() const -> u32;
		auto bind(u32 slot=0) const -> void;
		auto unbind() const -> void;
		
		auto enable() const -> void;
		auto disable() const -> void;
		
		auto width() const -> i32;
		auto height() const -> i32;
        auto load(byte* texture_buffer, i32 width, i32 height) -> void;
	    auto load(const char* filepath)-> void;
    public:
		u32 _texture_id;
	private:
		const char* _filepath;
		byte* _buffer;
		i32 _width,_height, _bits_per_pixel;
		mutable u32 _slot = -1;
	};

 // I'm actually using this one, because it simplifies alot, 
 // it alway binds to the same slot, which means I can use the same shader basically everytime
    struct SimpleTexture {
        u32 id;
        void load(const char* filepath);
        void bind();
    };
}