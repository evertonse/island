#include "graphics/skybox.hpp"


namespace cyx {
    void Skybox::init(const char* directory_path, const char* extension) {
        std::string dir;
        dir = std::regex_replace(directory_path, std::basic_regex("/+"), "/");
        dir = std::regex_replace(dir,std::basic_regex("/$"),"");
        shader = Shader("assets/shaders/skybox.glsl");
        shader.bind();
        shader.compile();
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ib);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        //  the cubemap texture object
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // These are very important to prevent seams
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // This might help with seams on some systems
        //glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        // Cycles through all the textures and attaches them to the cubemap object
        for (u32 i = 0; i < 6; i++) {
            const char* temp  = "";
            switch (i) {
                // All the faces of the cubemap (HAVE TO BE IN THIS ORDER)
                // and we expect to be named like this
                case 0: temp = "/right"; break;
                case 1: temp = "/left"; break;
                case 2: temp = "/top"; break;
                case 3: temp = "/bottom"; break;
                case 4: temp = "/front"; break;
                case 5: temp = "/back"; break;
            }

            int width, height, channels;
            unsigned char* data = stbi_load(
                (dir + std::string(temp) + std::string(extension)).c_str(), 
                &width, &height, &channels, 0
            );

            if(! data ) {
                std::cout << "[Skybox] Failed to load texture: "  
                          << dir << temp << extension
                          << " \nReason:" << stbi_failure_reason();
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
    
            stbi_set_flip_vertically_on_load(false);// Because opengl lmao
            glTexImage2D (
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, // little hack with the + i, just look at the definition you'll see
                0,
                format,
                width,
                height,
                0,
                format,
                GL_UNSIGNED_BYTE,
                data
            );
            stbi_image_free(data);
        }
	}

    void Skybox::draw(const mat4& view, mat4 projection){
        // Cubemap has depth = 1.0 which will make it invisible in clip space
        // remember opengl z maz is 1.0 in clip space? so thats why
        glDepthFunc(GL_LEQUAL);
        shader.bind();
        mat4 striped_view = mat4(view);
        // we do the following, we simply don't allow translations :)
        striped_view(0,3) = 0.0; 
        striped_view(1,3) = 0.0;
        striped_view(2,3) = 0.0;
        
        striped_view(3,0) = 0.0; 
        striped_view(3,1) = 0.0;
        striped_view(3,2) = 0.0;

        striped_view(3,3) = 0.0;
        shader.uniform_mat4("view", striped_view.data(), true);
        shader.uniform_mat4("projection", projection.data(),true);
        shader.uniform_int("skybox", 0);

        // Simple depth is always 1.0 is best to avoid zbuffer comparations
        // because it'll always fail
        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Switch back to the normal depth function
        glDepthFunc(GL_LESS);
    }

};