#include "rendering.hpp"

namespace cyx {
	// *------------------------------------------------------------------------------
	// | START Renderer
	// *------------------------------------------------------------------------------
	Renderer::Renderer() {
		set_clear_color(255, 255, 255, 255);
		enable_blending();


		i32 n_attr;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n_attr);
		std::cout << "[Renderer]" << "Maximum nº of vertex attributes supported: " << n_attr << std::endl;
	}

	auto Renderer::enable_blending() -> void {
		glLogCall(
			glEnable(GL_BLEND)
		);
		std::cout << "[Renderer]" << "sucessfully enable blending\n";

		glLogCall(
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA)
		);
		std::cout << "[Renderer]" << "sucessfully set blending options\n";
	}

	Renderer::~Renderer() {

	}

	void Renderer::clear() {
		glLogCall(
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		);
	}

    void Renderer::set_clear_color(f32 r, f32 g, f32 b, f32 a)
    {
        assert(r <= 1.0 && r >= 0);
        assert(g <= 1.0 && g >= 0);
        assert(b <= 1.0 && b >= 0);
        assert(a <= 1.0 && a >= 0);
		glLogCall(
			glClearColor(r,g,b,a)
		);
    }

    auto Renderer::set_viewport(i32 x, i32 y, u32 width, u32 height) -> void {
		glViewport(x, y, width, height);
	}

	auto Renderer::draw(const VertexArray& va, const VertexIndex& vi, const Shader& shader) -> void {

		std::cout << "[Renderer]" << "sucessfully set blending options\n";
		std::cout << "[Renderer]" << "GOT HERE 2\n";
		va.bind();
		shader.bind();
		vi.bind();
		std::cout << "[Renderer]" << "GOT HERE 3\n";
		std::cout << "vi.count()" << vi.count();
		glLogCall(
			glDrawElements(GL_TRIANGLES, vi.count(), GL_UNSIGNED_INT, NULL);
		);
		std::cout << "[Renderer]" << "GOT HERE 4\n";
		// <End> draw call
		va.unbind();
		shader.unbind();
		vi.unbind();
	}

	auto Renderer::draw(const VertexArray& va, const VertexIndex& vi, const Shader& shader, const Texture& tex) -> void {

		std::cout << "[Renderer]" << "sucessfully set blending options\n";
		std::cout << "[Renderer]" << "GOT HERE 2\n";
		va.bind();
		shader.bind();
		vi.bind();
		tex.bind();
		std::cout << "[Renderer]" << "GOT HERE 3\n";
		std::cout << "vi.count()" << vi.count();
		glLogCall(
			glDrawElements(GL_TRIANGLES, vi.count(), GL_UNSIGNED_INT, NULL);
		);
		std::cout << "[Renderer]" << "GOT HERE 4\n";

		va.unbind();
		shader.unbind();
		vi.unbind();
		tex.unbind();
	}


	// x------------------------------------------------------------------------------
	// | END Renderer
	// x------------------------------------------------------------------------------
		
}