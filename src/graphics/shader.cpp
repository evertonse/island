#include "graphics/shader.hpp"
#include "shader.hpp"
namespace cyx {
    static void SHADER_ASSERT_LOCATION(int location,const char* filepath,const char* var) {
        if (location == -1) {
			std::cout << "[Shader]:"
                << filepath <<" BAD uniform location: " << location
                << " for uniform: " << var  << "\n";
            #if ! defined (SHADER_IGNORE_BAD_UNIFORM)
            exit(1);
            #endif
		}
    }

    static std::string read_file(const std::string& filename) {
        std::ifstream file(filename);
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return content;
    }

	Shader::Shader(const char* filepath) {
    this->_filepath = filepath;

    if (filepath == nullptr) {
        set_default(ShaderType::VERTEX);
        set_default(ShaderType::FRAGMENT);
        return;
    }

    FILE* file = fopen(filepath, "r");
    if (!file) {
        // Handle error - could set defaults or throw
        set_default(ShaderType::VERTEX);
        set_default(ShaderType::FRAGMENT);
        return;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read entire file into a string
    std::string source;
    source.resize(file_size);
    fread(&source[0], 1, file_size, file);
    fclose(file);

    // Find shader sections
    size_t vertex_start = source.find("#pragma vertex");
    size_t fragment_start = source.find("#pragma fragment");

    // Extract vertex shader
    if (vertex_start != std::string::npos) {
        size_t start = source.find('\n', vertex_start) + 1;
        size_t end = (fragment_start != std::string::npos) ? fragment_start : source.size();

        _vertex_source = source.substr(start, end - start);

        // Remove trailing whitespace/newlines
        while (!_vertex_source.empty() && isspace(_vertex_source.back())) {
            _vertex_source.pop_back();
        }
    } else {
        set_default(ShaderType::VERTEX);
    }

    // Extract fragment shader
    if (fragment_start != std::string::npos) {
        size_t start = source.find('\n', fragment_start) + 1;
        size_t end = source.size();

        _fragment_source = source.substr(start, end - start);

        // Remove trailing whitespace/newlines
        while (!_fragment_source.empty() && isspace(_fragment_source.back())) {
            _fragment_source.pop_back();
        }
    } else {
        set_default(ShaderType::FRAGMENT);
    }
}
#if 0
	Shader::Shader(const char* filepath) {
        this->_filepath = filepath;
		if (filepath == NULL) {
				set_default(ShaderType::VERTEX);
				set_default(ShaderType::FRAGMENT);
		}
		else {
				std::ifstream shaderfile(filepath);
				std::string line;
				ShaderType interpret_source_as = ShaderType::NONE;
				// Stream to add our vertex
				std::stringstream vertex, fragment;
				assert(shaderfile && "Coundt open shader file somehow");

				bool found_pragma_vertex = false, found_pragma_fragment = false;

				while (std::getline(shaderfile, line)) {

					if (line.find("#pragma") != std::string::npos) {
						if  (line.find("vertex") != std::string::npos) {
								// add to vertex string
								interpret_source_as = ShaderType::VERTEX;
								continue;
						}
						else if ((line.find("fragment") != std::string::npos)) {
								// add to fragment string
								interpret_source_as = ShaderType::FRAGMENT;
								continue;
						}
					}

					// Adding to the actual string
					switch (interpret_source_as)
					{
						case cyx::ShaderType::VERTEX: {
								vertex << line << "\n";
								break;
						}
						case cyx::ShaderType::FRAGMENT: {

								fragment << line << "\n";
								break;
						}
						case cyx::ShaderType::NONE: {
								// Nothing should be added
								break;
						}
						default: {

								assert(0 && "unreachable");
						}
					}
				}
				_vertex_source   = vertex.str();
				_fragment_source = fragment.str();        }

	}
#endif

	Shader::Shader(const char* vertex_path, const char* frag_path)
    :_vertex_source(read_file(vertex_path)),_fragment_source(read_file(frag_path)) { }

   Shader Shader::from_src(const std::string &vertex_src, const std::string &frag_src)
	{
        Shader self;
	    self._vertex_source  = vertex_src;
		self._fragment_source = frag_src;
		if (vertex_src == "") {
				self.set_default(ShaderType::VERTEX);
                assert(0 && "[Shader] Vertex Source in NULL \n");
		}
		if (frag_src == "") {
				self.set_default(ShaderType::FRAGMENT);
				std::cout << "[Shader] Fragment Source in NULL \n";
		}
        return self;
    }

    Shader::~Shader()
    {
		glDeleteProgram(_programID);
	}

	auto Shader::compile() -> void {
		if (create_program(this->_vertex_source.c_str(), this->_fragment_source.c_str())) {
				std::cout << "[Shader] PROGRAM SUCESSFULLY CREATED\n";
                this->_is_compiled = true;
		}
	}

    auto Shader::is_compiled() -> bool {
        return this->_is_compiled;
    }


	auto Shader::bind() const -> void {
		glUseProgram(_programID);
	}

	auto Shader::unbind() const -> void {
		glUseProgram(0);
	}

	auto Shader::src(ShaderType type) -> const char* {
        if (type == ShaderType::FRAGMENT)
            return _fragment_source.c_str();
        else if(type == ShaderType::VERTEX)
            return _vertex_source.c_str();
        else
            return nullptr;
	}

	auto Shader::compile_shader(u32 type, const char* src) -> u32 {
		u32 shader;
		switch (type)
		{
				case GL_VERTEX_SHADER: {
					shader = glCreateShader(GL_VERTEX_SHADER);
					break;
				}

				case GL_FRAGMENT_SHADER: {
					shader = glCreateShader(GL_FRAGMENT_SHADER);
					break;
				}

				default: {
					std::cout << "others shaders are not supported" "\n";
					return false;
				}
		}
        glLogCall(
            glShaderSource(shader, 1, &src, NULL)
        );

        glLogCall(
            glCompileShader(shader);
        );
		// check for error
		i32 sucess;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &sucess);
		if (sucess != GL_TRUE) {
				char info[512];
				glGetShaderInfoLog(shader, 512, NULL, info);
				std::cout << "[Shader::compile_shader ERROR]: " << info << "\n";
				assert(0 && "look at console");
				glDeleteShader(shader);
				return false;
		}

		return shader;
	}

	auto Shader::set_default(ShaderType type) -> void {
		if (type == ShaderType::VERTEX) {
				_vertex_source = R"(
					#version 330  core
					in  vec4 position;
					out  vec4 pos;
					void main() {
						// builtin variable for positions
						gl_Position = vec4(position.x, position.y, position.z, position.w);
						pos = vec4(position.x, position.y, position.z, position.w);
					}
				)";
		}
		if (type == ShaderType::FRAGMENT) {
				_fragment_source = R"(
					#version 330  core
					// fragment shader must have 1 'out', thats the color
					out vec4 color;
					in vec4 pos;

					void main() {
						//color = vec4(1.0, 0.0, 0.0, 1.0);
						color = vec4(pos.z+pos.x, pos.z+pos.y, pos.z+pos.x, 1.0);
					}
				)";

		}
	}
	auto Shader::create_program(const char* vertex_shader_src, const char* frag_shader_src) -> bool {
		_programID = glCreateProgram();

		u32 vertex_shader    = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
		u32 frag_shader      = compile_shader(GL_FRAGMENT_SHADER, frag_shader_src);

		glAttachShader(_programID, vertex_shader);
		glAttachShader(_programID, frag_shader);


		glLinkProgram(_programID);

		valid(*this);
		// check for error
		i32 sucess;
		glGetProgramiv(_programID, GL_LINK_STATUS, &sucess);
		if (sucess != GL_TRUE) {
				glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &sucess);
				char info[512];
				glGetProgramInfoLog(_programID, 512, NULL, info);
				std::cout << "[ERROR]: couldnt link programID: " << _programID << info << "\n";
				assert(0 && "look at console");
				return false;
		}
		//glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &sucess);
		//if (sucess != GL_TRUE) {
		//    char info[512];
		//    glGetProgramInfoLog(_programID, 512, NULL, info);
		//    std::cout << "[ERROR]: GL_INFO_LOG_LENGT link programID: " << _programID << info << "\n";
		//    assert(0 && "look at console");
		//    return false;
		//}
		glDeleteShader(vertex_shader);
		glDeleteShader(frag_shader);
		return true;
	}

	auto Shader::valid(GLuint id) -> bool {
		glValidateProgram(id);
		// check for error
		i32 sucess;
		glGetProgramiv(id, GL_VALIDATE_STATUS, &sucess);
		if (sucess != GL_TRUE) {
				glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &sucess);
				char info[512];
				glGetProgramInfoLog(_programID, 512, NULL, info);
				std::cout << "[ERROR]: couldnt link programID: " << _programID << info << "\n";
				assert(0 && "Look at console !! I guess you're already looking, look up then");
				return false;
		}
		return true;
	}

	auto Shader::uniform_vec4(const char* var, f32* values) -> void {
		GLint location = glGetUniformLocation(_programID, var);
		glUniform4f(location, values[0], values[1], values[2], values[4]);
        SHADER_ASSERT_LOCATION(location,_filepath.c_str(),var);
	}

	auto Shader::uniform_mat4(const char *var, f32* values, bool transpose) -> void {
		GLint location = glGetUniformLocation(_programID, var);
		glUniformMatrix4fv(location,1, transpose?GL_TRUE:GL_FALSE, values);
        SHADER_ASSERT_LOCATION(location,_filepath.c_str(),var);

	}

	auto Shader::uniform_int(const char *var, i32 value) -> void {
		GLint location = glGetUniformLocation(_programID, var);
		glUniform1i(location, value);
        SHADER_ASSERT_LOCATION(location,_filepath.c_str(),var);
	}

	auto Shader::uniform_vec3(const char* var, f32* values) -> void {
        bind();
		GLint location = glGetUniformLocation(_programID, var);
		glUniform3f(location, values[0], values[1], values[2]);
        SHADER_ASSERT_LOCATION(location,_filepath.c_str(),var);
	}

	auto Shader::uniform_vec2(const char* var, f32* values) -> void {
		GLint location = glGetUniformLocation(_programID, var);
		glUniform2f(location, values[0], values[1]);
        SHADER_ASSERT_LOCATION(location,_filepath.c_str(),var);
	}

	auto Shader::uniform_float(const char* var, f32 value) -> void {
		i32 location = glGetUniformLocation(_programID, var);
		glUniform1f(location, value);
        SHADER_ASSERT_LOCATION(location,_filepath.c_str(),var);
	}



}
