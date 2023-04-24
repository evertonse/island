#include "texture.hpp"

namespace cyx {
	Texture::Texture(const char* filepath) 
	: _width(0), _height(0), _filepath(filepath),_buffer(nullptr)
	{
		// only for PNG, since PNG start top down, 
		// Whereas OpenGL expects from the bottom left.
		stbi_set_flip_vertically_on_load(true);
		_buffer = stbi_load(filepath,&_width, &_height,&_bits_per_pixel,4);
		std::cout << "[Texture]" << "Loaded Buffer from STB\n";
		
		assert(_buffer != nullptr && "porra ta nulo cara" );
		if(_buffer == nullptr) {
			std::cout << "[Texture]" << " _buffer is nullptr\n";
		}

		glLogCall(
			glGenTextures(1,&_texture_id)
		);
		std::cout << "[Texture]" << "Gen Textures\n";
		this->bind();

		glLogCall(
			// minification and magnificationm is something that need to be studied further
			// for sure theses functions parametere carries more implicit information
			// than i'm able to handle rn
			// Linear sample is kinda okay to understand
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		);
		std::cout << "[Texture]" << "Texture Minify Filter set \n";
		
		glLogCall(
			// when we render a texture on a area that is larger in pixels than the actual texture
			// meaning the surface is bigger than the image in terms of pixels
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		);
		std::cout << "[Texture]" << "Texture Mag Filter set \n";
		
		glLogCall(
			// campling
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		);
		std::cout << "[Texture]" << "Texture clamping set \n";

		glLogCall(
			//tiling
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		);
		std::cout << "[Texture]" << "Texture tiling set \n";

		// 0 because is not a multilevel texture

		glLogCall(
			glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA8, _width, _height,0,GL_RGBA,GL_UNSIGNED_BYTE,_buffer);
		);
		std::cout << "[Texture]" << "Texture data set \n";


		std::cout << "[Texture]" << "All Textures options set\n";
		if(_buffer) {
			// free just on the cpu
			stbi_image_free(_buffer);
			std::cout << "[Texture]" << "Freed Memory from STB Image\n";
		}
	}

	Texture::~Texture(){
		glLogCall(
			glDeleteTextures(1,&this->_texture_id)
		);
	};
	auto 	Texture::slot()  -> u32 {
		return _slot;
	}
	
	auto Texture::bind(u32 slot) const -> void {
	
		this->_slot = slot;
		
		glLogCall(
			glActiveTexture(GL_TEXTURE0 + slot)
		);

		glLogCall(
			glBindTexture(GL_TEXTURE_2D,this->_texture_id)
		);
		std::cout << "[Texture]" << "bound\n";
	}

	auto 	Texture::unbind() const -> void {

		glLogCall(
			glBindTexture(GL_TEXTURE_2D,0)
		);
		std::cout << "[Texture]" << "unbound\n";
	}
	
	auto Texture::width() const -> i32
	{
		return i32(_width);
	}

	auto Texture::height() const -> i32
	{
		return i32(_height);
	}
}
