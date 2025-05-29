#include "graphics/shadowmap.hpp"
#include "shadowmap.hpp"

void cyx::ShadowMap::init() {
   shader = Shader("assets/shaders/shadow.glsl"); 
   shader.bind();
   shader.compile();
    // Framebuffer for Shadow Map
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// Texture for Shadow Map FBO
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
	// Needed since we don't touch the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);


	auto ok = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (ok != GL_FRAMEBUFFER_COMPLETE){
		std::cout << "Framebuffer error : " << ok << '\n';
    }
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer another error error : " << ok << '\n';
    }
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

cyx::mat4 cyx::ShadowMap::matrix() { 

    mat4 fix_tex_coord{
        {0.5, 0.0, 0.0, 0.5},
        {0.0, 0.5, 0.0, 0.5},
        {0.0, 0.0, 0.5, 0.5},
        {0.0, 0.0, 0.0, 1.0},
    };

    mat4 projection = mat4::ortho(-10,10,-10,10,-10,20);
    vec3 light_dir = vec3(0.5f, 2.f, 2.f);
    mat4 view = mat4::lookat(light_dir, {0,0,0}, {0,1,0});
    mat4 model = mat4::identity();
    return fix_tex_coord % projection % view % model;
}

void cyx::ShadowMap::begin() { 
    // Depth testing needed for Shadow Map
    glEnable(GL_DEPTH_TEST);
    // Preparations for the Shadow Map
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
    glClear(GL_DEPTH_BUFFER_BIT);

    mat4 projection = mat4::ortho(-10,10,-10,10, -10.0, 10.0);
    vec3 light_pos = vec3(1.0f, 2.f, 1.f);
    vec3 light_dir = vec3(1.0f, 2.f, 2.f);
    mat4 view = mat4::lookat(light_pos, light_pos + light_dir, {0,1,0});
    mat4 model = mat4::identity();
	shader.bind();
    shader.uniform_mat4("projection",  projection.data(),true);
    shader.uniform_mat4("view",  view.data(),true);
    shader.uniform_mat4("model",  model.data(),true);
}



void cyx::ShadowMap::end(veci2 viewport) {
    // Switch back to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Switch back to the default viewport
    glViewport(0, 0, viewport.x, viewport.y);
    // Clean the back buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void cyx::ShadowMap::bind_texture() {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D ,texture);
}
