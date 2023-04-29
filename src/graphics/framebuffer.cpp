#include "framebuffer.hpp"

void cyx::ShadowMap::init(const vec3& light_position) {
   shader = Shader("assets/hadowmap.glsl"); 
    // Framebuffer for Shadow Map
	glGenFramebuffers(1, &fbo);

	// Texture for Shadow Map FBO
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
	// Prevents darkness outside the frustrum
	float clamp_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clamp_color);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
	// Needed since we don't touch the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Matrices needed for the light's perspective
	mat4 projection = mat4::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 100.0f);
	mat4 view = mat4::lookat(light_position * 20.0f, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 light_projection = projection % view;

	shader.bind();
    shader.uniform_mat4("light_projection", light_projection.data());

	auto ok = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (ok != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "Framebuffer error : " << ok << '\n';
    }
}