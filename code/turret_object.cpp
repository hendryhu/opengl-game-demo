#include "turret_object.h"
#include <glm/gtc/matrix_transform.hpp>

namespace game {
    TurretObject::TurretObject(GameObject* parent, TurretType type,
                               const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture,
                               const glm::vec3& parent_offset, float hitbox)
            : GameObject(position, geom, shader, texture),
            type(type), parent(parent), parent_offset(parent_offset)
    {
        object_type = ObjectType::Turret;
        // set the position and rotation of the turret object to match the parent object
        SetPosition(parent->GetPosition());
        SetRotation(parent->GetRotation());
        parent_rotation_offset = parent->GetRotation();

        // in this case, the hitbox_radius is the length of the turret object
        hitbox_radius = hitbox;

        // create a timer for the fire cooldown
        fire_cooldown_timer = new Timer();

        expiration_timer = new Timer();
        expiration_timer->setTime(expiration_time);

        if (type == TurretType::MACHINE_GUN) {
            expiration_time = 10.0f;
        }
        if (type == TurretType::HOMING) {
            expiration_time = 20.0f;
        }
    }

    // deep copy constructor, for initializing a template turret object
    TurretObject::TurretObject(const TurretObject& other)
        : GameObject(other), type(other.type), parent(other.parent), parent_offset(other.parent_offset),
          parent_rotation_offset(other.parent_rotation_offset), fire_cooldown(other.fire_cooldown),
          fire_cooldown_timer(new Timer(*other.fire_cooldown_timer)), expiration_timer(new Timer(*other.expiration_timer)),
          expiration_time(other.expiration_time)
    {
        // shallow copy the bullet object
        bullet = other.bullet;
        expiration_timer->setTime(expiration_time);
    }

    void TurretObject::Update(double delta_time)
    {
        // update the angle of the turret object to match the parent object
        // if the parent has changed rotation, update the turret's rotation
        if (parent->GetRotation() != parent_rotation_offset)
        {
            angle_ += parent->GetRotation() - parent_rotation_offset;
            parent_rotation_offset = parent->GetRotation();
        }
        SetRotation(angle_);

        // the cannon sticks to the parent object rotation
        if (type == TurretType::CANNON)
        {
            SetRotation(parent->GetRotation() + glm::pi<float>());
        }

        // update timer
        fire_cooldown_timer->Update(delta_time);
        if ((type == TurretType::MACHINE_GUN || type == TurretType::HOMING) && expiration_timer->timesUp())
        {
            expired = true;
        }
        expiration_timer->Update(delta_time);

        SetPosition(parent->GetPosition() + parent_offset);
        GameObject::Update(delta_time);
    }

    // hierarchically render the turret object
    void TurretObject::Render(glm::mat4 view_matrix, double current_time) {
        view_matrix_ = view_matrix;

        // Set up the shader
        shader_->Enable();

        // Set up the view matrix
        shader_->SetUniformMat4("view_matrix", view_matrix);

        // Setup the scaling matrix for the shader
        glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale_.x, scale_.y, 1.0));

        // Get the parent's position and rotation
        glm::vec3 parent_position = parent->GetPosition();
        float parent_rotation = parent->GetRotation();

        // Translate the turret to the parent's position
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), parent_position + parent_offset);

        // Rotate the turret based on the parent's rotation
        // glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), parent_rotation + parent_rotation_offset, glm::vec3(0.0, 0.0, 1.0));

        // Apply the turret's own rotation
        glm::mat4 turret_rotation_matrix = glm::rotate(glm::mat4(1.0f), angle_, glm::vec3(0.0, 0.0, 1.0));

        // Combine the transformations
        glm::mat4 transformation_matrix = translation_matrix * turret_rotation_matrix * scaling_matrix;
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

    void TurretObject::Control(const glm::vec2 cursor_position, double delta_time) {

        // you can't control cannons
        if (type == TurretType::CANNON) {
            return;
        }

        // transform the cursor position to the world space using the view matrix
        glm::vec4 cursor_position_view = glm::vec4(cursor_position.x, cursor_position.y, 0.0f, 1.0f);
        glm::vec4 cursor_position_world = glm::inverse(view_matrix_) * cursor_position_view;
        glm::vec2 cursor_position_new = glm::vec2(cursor_position_world.x, cursor_position_world.y);

        // get the angle between the cursor and the turret
        glm::vec2 turret_position = glm::vec2(GetPosition().x, GetPosition().y);
        glm::vec2 direction = cursor_position_new - turret_position;

        float target_angle = glm::atan(direction.y, direction.x) + angle_offset;
        if (target_angle < 0) {
            target_angle += 2 * glm::pi<float>();
        }

        // calculate if we should rotate clockwise or counterclockwise
        float angle_diff = target_angle - angle_;
        if (angle_diff > glm::pi<float>()) {
            angle_diff -= 2 * glm::pi<float>();
        } else if (angle_diff < -glm::pi<float>()) {
            angle_diff += 2 * glm::pi<float>();
        }

        // rotate the turret
        if (angle_diff > 0.05) {
            angle_ += rotation_speed * static_cast<float>(delta_time);
        } else if (angle_diff < 0.05) {
            angle_ -= rotation_speed * static_cast<float>(delta_time);
        }
        else {  // if the angle is close enough, snap to the target angle
            angle_ = target_angle;
        }
    }

    // called by the player object to fire the turret
    BulletObject* TurretObject::Fire()
    {
        // if the fire cooldown timer is still running, return nullptr
        if (!fire_cooldown_timer->timesUp())
            return nullptr;

        // reset the timer
        fire_cooldown_timer->setTime(fire_cooldown);

        // deep copy the bullet object
        auto* new_bullet = new BulletObject(*bullet);

        // calculate bullet position, should be at the tip of the turret
        glm::vec3 bullet_position = GetPosition() + glm::vec3(glm::cos(angle_), glm::sin(angle_), 0.0f) * hitbox_radius;
        bullet_position.z = 0.0f;
        new_bullet->Shoot(bullet_position, GetBearing(), parent);
        // new_bullet->SetScale(10.0);
        new_bullet->SetRotation(angle_ - glm::pi<float>() / 2.0f);

        return new_bullet;
    }

    // set the angle offset of the turret
    void TurretObject::SetParentRotationOffset(float offset) {
        parent_rotation_offset += offset;
        float angle = parent_rotation_offset + angle_offset;
        while (angle < 2.0f * glm::pi<float>()) {
            angle += 2.0f * glm::pi<float>();
        }
        while (angle > 2.0f * glm::pi<float>()) {
            angle -= 2.0f * glm::pi<float>();
        }
        SetRotation(angle);
    }

} // game