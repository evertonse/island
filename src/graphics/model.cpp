#include "graphics/model.hpp"

void cyx::Model::load(const char *mesh_path, const char *texture_path)
{
    mesh.load(mesh_path);
    texture.load(texture_path);
}

void cyx::Model::draw() {
    texture.bind();
    mesh.draw();
}

void cyx::Model::from(Model *self, TripleBufferMesh mesh, SimpleTexture texture) {
    self->mesh = mesh;
    self->texture = texture;
}

void cyx::Model::from(Model *self, const char *mesh_path, const char *texture_path) {
    assert(self != nullptr);
    self->load(mesh_path,texture_path);
}
