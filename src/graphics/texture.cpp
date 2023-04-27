#include "graphics/texture.hpp"

namespace cyx {
	Texture::Texture(const char* filepath) 
	: _width(0), _height(0), _filepath(filepath),_buffer(nullptr)
	{	
        glLogCall(
			glGenTextures(1,&_texture_id)
		);
		std::cout << "[Texture]" << "Gen Textures\n";
		this->bind();
        this->load(filepath);
	}

    void Texture::load(byte* texture_buffer, i32 width, i32 height) {
		assert(texture_buffer != nullptr && "Ta nulo cara" );
		if(_buffer == nullptr) {
			std::cout << "[Texture]" << " _buffer is nullptr\n";
            exit(1);
		}

		this->bind();
        this->_buffer = texture_buffer;
        this->_width = width;
        this->_height = height;

		// 0 because is not a multilevel texture
		glLogCall(
			glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA, _width, _height,0,GL_RGBA,GL_UNSIGNED_BYTE,_buffer);
		);
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

		std::cout << "[Texture]" << "Texture data set \n";

		std::cout << "[Texture]" << "All Textures options set\n";
        this->unbind();
    }

    auto Texture::load(const char* filepath) -> void {
        // only for PNG, since PNG start top down, 
		// Whereas OpenGL expects from the bottom left.
		stbi_set_flip_vertically_on_load(true);
		_buffer = stbi_load(filepath, &_width, &_height, &_bits_per_pixel, 0);
        this->load(_buffer,_width,_height);
        if(_buffer == nullptr) {
            std::cout <<"[Texture]"  << "failled to load the buffer from :" 
                << filepath << " Reason:" << stbi_failure_reason();
            exit(0);
        }
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
    
	auto Texture::slot()  const -> u32 {
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

	auto Texture::unbind() const -> void {

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
    



    //*********************** ------------- ***********************//
    //*********************** SimpleTexture ***********************//
    //*********************** ------------- ***********************//

    void SimpleTexture::bind() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void SimpleTexture::load(const char* filepath){
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id); 
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load image, create texture and generate mipmaps
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
        unsigned char *data = stbi_load(filepath, &width, &height, &channels, 0);

        if (data == nullptr) {
            std::cout << "id:" << id << " Failed to load texture" << filepath << std::endl;
            std::cout <<"[Texture]"  << "failled to load the buffer from :" 
                    << filepath << " Reason:" << stbi_failure_reason();
            exit(1);
        }

        GLint format;
        switch (channels) {
            case 1:
                format = GL_RED;
                break;
            case 2:
                format = GL_RG;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                (assert(0 && "idk about this format"));
        }

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "id:" << id << " Loaded texture from " << filepath << std::endl;

        stbi_image_free(data);
    }
}
