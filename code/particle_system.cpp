#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

#include "particle_system.h"


namespace game {

    ParticleSystem::ParticleSystem(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GameObject* parent, int type, float cycle, float speed, float gravity)
        : GameObject(position, geom, shader, texture) {

        this->current_time = 0.0;
        this->type = type;
        this->parent_ = parent;
        this->cycle = cycle;
        this->speed = speed;
        this->gravity = gravity;
    }


    ParticleSystem::ParticleSystem(const ParticleSystem& other)
        : GameObject(other.GetPosition(), other.getGeometry(), other.getShader(), other.getTexture()) {
        parent_ = other.parent_;
        cycle = other.cycle;
        speed = other.speed;
        gravity = other.gravity;
        type = other.type;
        current_time = 0.0;
    }


    void ParticleSystem::Update(double delta_time) {
        current_time += delta_time;

        // Call the parent's update method to move the object in standard way, if desired
        GameObject::Update(delta_time);
    }


    void ParticleSystem::Render(glm::mat4 view_matrix, double temp) {
        // Set up the shader
        shader_->Enable();

        // Set up the view matrix
        shader_->SetUniformMat4("view_matrix", view_matrix);

        // Setup the scaling matrix for the shader
        glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale_.x, scale_.y, 1.0));

        // Setup the rotation matrix for the shader
        glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), angle_, glm::vec3(0.0, 0.0, 1.0));

        // Set up the translation matrix for the shader
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);

        // Set up the parent transformation matrix
        glm::mat4 parent_rotation_matrix = glm::rotate(glm::mat4(1.0f), parent_->GetRotation(), glm::vec3(0.0, 0.0, 1.0));
        glm::mat4 parent_translation_matrix = glm::translate(glm::mat4(1.0f), parent_->GetPosition());
        glm::mat4 parent_transformation_matrix = parent_translation_matrix * parent_rotation_matrix;

        // Setup the transformation matrix for the shader
        glm::mat4 transformation_matrix = parent_transformation_matrix * translation_matrix * rotation_matrix * scaling_matrix;

        // Set the transformation matrix in the shader
        shader_->SetUniformMat4("transformation_matrix", transformation_matrix);

        // Set the time in the shader
        shader_->SetUniform1f("time", current_time);

        // Set the attributes in the shader
        shader_->SetUniform1f("cycle", cycle);
        shader_->SetUniform1f("speed", speed);
        shader_->SetUniform1f("gravity", gravity);

        // Set the particle type in the shader
        shader_->SetUniform1i("particle_type", type);

        // Set up the geometry
        geometry_->SetGeometry(shader_->GetShaderProgram());

        // Bind the particle texture
        glBindTexture(GL_TEXTURE_2D, texture_);

        // Draw the entity
        glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
    }

} // namespace game
