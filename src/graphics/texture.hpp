#pragma once
#include "utils/common.h"
#include "stb_image.h"
#include "graphics/rendering.hpp"

namespace cyx {
	class Texture {
	public:
		Texture(const char* filepath);
		~Texture();

		auto slot() -> u32;
		auto bind(u32 slot = 0) const -> void;
		auto unbind() const -> void;
		
		auto enable() const -> void;
		auto disable() const -> void;
		
		auto width() const -> i32;
		auto height() const -> i32;
	
	private:
		u32 _texture_id;
		const char* _filepath;
		byte* _buffer;
		i32 _width,_height, _bits_per_pixel;
		mutable u32 _slot = 0;
	};
}