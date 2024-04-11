#include "player_game_object.h"

namespace game
{
    /*
        PlayerGameObject inherits from GameObject
        It overrides GameObject's update method, so that you can check for input to change the velocity of the player
    */

    PlayerGameObject::PlayerGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture)
        : GameObject(position, geom, shader, texture)
    {
        object_type = ObjectType::Player;
        setHealth(500);
        setHitboxRadius(this->GetScale() / 2);
        melee_damage = 5.0f;
        weight = 50.0f;
    }


    void PlayerGameObject::takeDamage(float damage) {
        if (!inIFrame()) {
            // if there are more than 1 turret, delete the last one instead of taking damage
            if (child_objects_.size() > 1) {
                TurretObject* turret = nullptr;
                int index = -1;
                // find the last turret that isn't a cannon
                for (int i = child_objects_.size() - 1; i >= 1; i--) {
                    TurretObject* curr_turret = dynamic_cast<TurretObject*>(child_objects_[i]);
                    if (curr_turret != nullptr && curr_turret->GetType() != TurretType::CANNON) {
                        turret = curr_turret;
                        index = i;
                        break;
                    }
                }

                // if we found a turret, damage it
                if (turret != nullptr) {
                    turret->takeDamage(damage);
                    if (turret->getHealth() <= 0) {
                        Helper::removeByIndex(&child_objects_, index);
                    }
                    iframe_timer_.setTime(iframe);
                    return;
                }

            }
            GameObject::takeDamage(damage);
            iframe_timer_.setTime(iframe);
        }
    }


    // Update function for moving the player object around
    void PlayerGameObject::Update(double delta_time)
    {
        // decelerate
        // front
        float max_forward_velocity = movement_input.x * max_speed_;
        float forward_velocity = glm::dot(current_velocity_, GetBearing());
        float velocity_difference = abs(max_forward_velocity - forward_velocity);
        if (forward_velocity > 0.0f) {
            if (max_forward_velocity < forward_velocity){
                current_velocity_ -= GetBearing() * std::min(front_friction * static_cast<float>(delta_time), velocity_difference);
            }
        }
        else if (forward_velocity < 0.0f) {
            if (max_forward_velocity > forward_velocity) {
                current_velocity_ += GetBearing() * std::min(front_friction * static_cast<float>(delta_time), velocity_difference);
            }
        }

        // side
        float max_rightside_velocity = 0.0f;
        float rightside_velocity = glm::dot(current_velocity_, GetRight());
        velocity_difference = abs(max_rightside_velocity - rightside_velocity);
        if (rightside_velocity > 0.0f) {
            if (max_rightside_velocity < rightside_velocity) {
                current_velocity_ -= GetRight() * std::min(side_friction * static_cast<float>(delta_time), velocity_difference);
            }
        }
        else if (rightside_velocity < 0.0f) {
            if (max_rightside_velocity > rightside_velocity) {
                current_velocity_ += GetRight() * std::min(side_friction * static_cast<float>(delta_time), velocity_difference);
            }
        }


        // accelerate
        // front speed less than max_speed
        forward_velocity = glm::dot(current_velocity_, GetBearing());
        velocity_difference = abs(max_speed_ - forward_velocity);
        if (abs(forward_velocity) < max_speed_) {
            current_velocity_ += GetBearing() * movement_input.x * min(acceleration_ * static_cast<float>(delta_time), velocity_difference);
        }

        // update the position of the object based on the current velocity
        SetPosition(GetPosition() + current_velocity_ * static_cast<float>(delta_time));

        // clamp the turning speed to the maximum turning speed
        if (turning_speed_ > max_turning_speed_)
        {
            turning_speed_ = max_turning_speed_;
        }

        if (turning_speed_ < -max_turning_speed_)
		{
			turning_speed_ = -max_turning_speed_;
		}


        // turning
        if (movement_input.y == 0.0f) {
            // decrease turning speed
            if (turning_speed_ < 0) {
                turning_speed_ += rotational_friction * static_cast<float>(delta_time);
            }
            else if (turning_speed_ > 0) {
                turning_speed_ -= rotational_friction * static_cast<float>(delta_time);
            }
        }
        else if (movement_input.y > 0.0f) {
            if (turning_speed_ < max_turning_speed_) {
                turning_speed_ += min(rotational_acceleration * static_cast<float>(delta_time), max_turning_speed_ - turning_speed_);
            }
        }
        else {
            if (turning_speed_ > -max_turning_speed_) {
                turning_speed_ -= min(rotational_acceleration * static_cast<float>(delta_time), max_turning_speed_ - turning_speed_);
            }
        }

        // update the rotation of the object based on the current turning speed
        SetRotation(GetRotation() - glm::pi<float>() * turning_speed_ * static_cast<float>(delta_time));

        // update iframe timer
        iframe_timer_.Update(delta_time);

        // move object with knockback
        SetPosition(GetPosition() + knockback_velocity * static_cast<float>(delta_time));


        // knockback velocity decelerate
        float kb_velocity_length = glm::length(knockback_velocity);
        if (kb_velocity_length > 0.0f) {
            glm::vec3 kb_direction = glm::normalize(knockback_velocity);
            knockback_velocity -= kb_direction * min(knockback_deceleration * static_cast<float>(delta_time), kb_velocity_length);
        }

        int num_turrets = 0;

        // Update all child objects
        for (auto child : child_objects_) {
            child->Update(delta_time);
            if (dynamic_cast<TurretObject*>(child) != nullptr) {
                num_turrets++;
            }
        }

        // reset all expired turrets to the default turret
        for (int i = 0; i < child_objects_.size(); i++) {
            auto* turret = dynamic_cast<TurretObject*>(child_objects_[i]);
            if (turret->IsExpired()) {
                if (turret == first_turret_) {
                    SetTurret(default_turret_);
                } else {
                    delete turret;
                    child_objects_.erase(child_objects_.begin() + i);
                    i--;
                    num_turrets--;
                }
            }
        }

        // if there are no turrets, add a default turret
        if (num_turrets <= 0) {
            auto* turret = new TurretObject(*default_turret_);
            AddChildObject(turret);
            first_turret_ = turret;
        }
    }

    // override parent's render method
    void PlayerGameObject::Render(glm::mat4 view_matrix, double current_time)
    {
        // call the parent's render method
        GameObject::Render(view_matrix, current_time);

        // Render all child objects
        for (auto child : child_objects_)
        {
            child->Render(view_matrix, current_time);
        }
    }

    void PlayerGameObject::TurnTurrets(const glm::vec2 cursor_position, double delta_time)
    {
        for (auto child : child_objects_) {
            if (dynamic_cast<TurretObject*>(child) != nullptr) {
                auto* turret = dynamic_cast<TurretObject*>(child);
                turret->Control(cursor_position, delta_time);
            }
        }
    }


    void PlayerGameObject::ShootTurrets(std::vector<GameObject*> &bullets, WeaponShootKey key)
    {
        if (key == WeaponShootKey::PRIMARY) {
            for (auto child : child_objects_) {
                if (dynamic_cast<TurretObject*>(child) != nullptr) {
                    auto* turret = dynamic_cast<TurretObject*>(child);
                    if (turret->GetType() == TurretType::CANNON) {
                        continue;
                    }
                    auto* new_bullet = turret->Fire();
                    if (new_bullet != nullptr) {
                        bullets.push_back(new_bullet);
                    }
                }
            }
        }

        if (key == WeaponShootKey::SECONDARY) {
            for (auto child : child_objects_) {
                if (dynamic_cast<TurretObject*>(child) != nullptr) {
                    auto* turret = dynamic_cast<TurretObject*>(child);
                    if (turret->GetType() == TurretType::CANNON) {
                        auto* new_bullet = turret->Fire();
                        if (new_bullet != nullptr) {
                            bullets.push_back(new_bullet);
                        }

                        // if the player shot a cannonball, propel the player forward
                        if (new_bullet != nullptr) {
                            current_velocity_ += GetBearing() * 10.0f;
                        }
                    }
                }
            }
        }
    }

    // replaces the first turret with the given turret, also deleting the old turret
    void PlayerGameObject::SetTurret(TurretObject* turret) {
        if (first_turret_ != nullptr) {
            delete first_turret_;
            child_objects_.erase(child_objects_.begin());
        }

        auto* turret_copy = new TurretObject(*turret);
        child_objects_.insert(child_objects_.begin(), turret_copy);
        first_turret_ = turret_copy;
    };
} // namespace game
