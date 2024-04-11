#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "game_object.h"

namespace game {

GameObject::GameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture) 
{

    // Initialize all attributes
    position_ = position;
    scale_ = glm::vec2(1.0, 1.0);
    angle_ = 0.0;
    geometry_ = geom;
    shader_ = shader;
    texture_ = texture;
}


    GameObject::~GameObject()
{
    // free all child objects
    for (auto child : child_objects_)
    {
        delete child;
    }
}


glm::vec3 GameObject::GetBearing(void) const {

    glm::vec3 dir(cos(angle_), sin(angle_), 0.0);
    return dir;
}


glm::vec3 GameObject::GetRight(void) const {

    float pi_over_two = glm::pi<float>() / 2.0f;
    glm::vec3 dir(cos(angle_ - pi_over_two), sin(angle_ - pi_over_two), 0.0);
    return dir;
}


void GameObject::SetRotation(float angle){ 

    // Set rotation angle of the game object
    // Make sure angle is in the range [0, 2*pi]
    float two_pi = 2.0f*glm::pi<float>();
    angle = fmod(angle, two_pi);
    if (angle < 0.0){
        angle += two_pi;
    }
    angle_ = angle;
}


void GameObject::Update(double delta_time) {
    // move object with knockback
    SetPosition(GetPosition() + knockback_velocity * static_cast<float>(delta_time));


    // knockback velocity decelerate
    float kb_velocity_length = glm::length(knockback_velocity);
    if (kb_velocity_length > 0.0f) {
        glm::vec3 kb_direction = glm::normalize(knockback_velocity);
        knockback_velocity -= kb_direction * min(knockback_deceleration * static_cast<float>(delta_time), kb_velocity_length);
    }

    // Update all child objects
    for (auto child : child_objects_) {
        child->Update(delta_time);
    }
}


void GameObject::Render(glm::mat4 view_matrix, double current_time){

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

    // Setup the transformation matrix for the shader
    glm::mat4 transformation_matrix = translation_matrix * rotation_matrix * scaling_matrix;

    // Set the transformation matrix in the shader
    shader_->SetUniformMat4("transformation_matrix", transformation_matrix);

    // Set up the geometry
    geometry_->SetGeometry(shader_->GetShaderProgram());

    // Bind the entity's texture
    glBindTexture(GL_TEXTURE_2D, texture_);

    // Draw the entity
    glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);

    // Render all child objects
    for (auto child : child_objects_) {
        child->Render(view_matrix, current_time);
    }
}

    // Add child objects to the game object
    void GameObject::AddChildObject(GameObject* object)
    {
        child_objects_.push_back(object);
    }


    void GameObject::knockback(GameObject* knockback_object, float multiplier) {
        float total_weight = getWeight() + knockback_object->getWeight();
        glm::vec3 unnormalized_direction = GetPosition() - knockback_object->GetPosition();
        glm::vec3 knockback_direction;
        if (glm::length(unnormalized_direction) != 0.0f) {
            knockback_direction = glm::normalize(GetPosition() - knockback_object->GetPosition());
        }
        else {
            knockback_direction = glm::vec3(0.0f, 0.0f, 0.0f);
        }
        knockback_velocity += knockback_direction * knockback_force * (knockback_object->getWeight() / total_weight) * multiplier;
    }

} // namespace game
