#include "collectible_game_object.h"

namespace game
{
    CollectibleGameObject::CollectibleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, CollectibleType type)
        : GameObject(position, geom, shader, texture), type_(type)
    {
        hitbox_radius = 0.5f;
    }

    // Set the movement of the collectible object to be circular
    void CollectibleGameObject::SetMovement(float radius, float time, const glm::vec2& center)
    {
        circular_movement = true;
        radius_ = radius;
        time_ = time;
        center_ = center;
    }

    // Update function (only different if the object is moving in a circular motion)
    void CollectibleGameObject::Update(double delta_time)
    {
        if (circular_movement)
        {
            // parametric circular motion
            phase_ += delta_time;
            float x = center_.x + radius_ * glm::cos(phase_ / time_ * 2 * glm::pi<float>());
            float y = center_.y + radius_ * glm::sin(phase_ / time_ * 2 * glm::pi<float>());
            glm::vec2 new_position = glm::vec2(x, y);
            SetPosition(glm::vec3(new_position, position_.z));

            // calculate new bearing
            float bearing = glm::atan(new_position.y - center_.y, new_position.x - center_.x);
            SetRotation(bearing);
        }
        GameObject::Update(delta_time);
    }
} // namespace game
