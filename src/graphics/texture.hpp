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

		auto slot() -> u32;
		auto bind() const -> void;
		auto unbind() const -> void;
		
		auto enable() const -> void;
		auto disable() const -> void;
		
		auto width() const -> i32;
		auto height() const -> i32;
        auto load(byte* texture_buffer, i32 width, i32 height) -> void;
	    auto load(const char* filepath)-> void;
	private:
		u32 _texture_id;
		const char* _filepath;
		byte* _buffer;
		i32 _width,_height, _bits_per_pixel;
		mutable u32 _slot = 0;
	};
}