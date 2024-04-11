#include "bullet_object.h"

namespace game
{
    BulletObject::BulletObject(const glm::vec3 &position, game::Geometry *geom, game::Shader *shader, GLuint texture,
                               float damage, const glm::vec3 &direction, float speed, float expiration_time, float hitbox_radius, BulletType type):
    GameObject(position, geom, shader, texture), damage_(damage), direction_(direction), speed_(speed), type_(type), expiration_time_(expiration_time)
    {
        object_type = ObjectType::Bullet;
        // find the angle of the bullet
        float angle = glm::atan(direction_.y, direction_.x);
        SetRotation(angle - glm::pi<float>() / 2.0f);

        // set the hitbox radius (in this case, it is the length of the bullet)
        this->hitbox_radius = hitbox_radius;

        trail_ = nullptr;
    }

    // deep copy constructor
    BulletObject::BulletObject(const BulletObject &other) : GameObject(other)
    {
        object_type = other.object_type;
        damage_ = other.damage_;
        direction_ = other.direction_;
        speed_ = other.speed_;
        type_ = other.type_;
        is_alive_ = other.is_alive_;
        shooter_ = other.shooter_;
        shooter_type_ = other.shooter_type_;
        expiration_time_ = other.expiration_time_;
        expiration_timer_ = other.expiration_timer_;
        if (other.trail_ != nullptr) {
            trail_ = new ParticleSystem(*other.trail_);
            trail_->setParent(this);
            trail_->SetScale(other.trail_->GetScale());
        } else {
            trail_ = nullptr;
        }
        enemy_objects_ = other.enemy_objects_;
        target_position_ = glm::vec3 (0.0f, 0.0f, 0.0f);
        target_found_ = false;
    }

    BulletObject::~BulletObject()
    {
        delete trail_;
    }

    void BulletObject::Shoot(const glm::vec3 &position, const glm::vec3 &direction, GameObject *shooter)
    {
        SetPosition(position);
        direction_ = direction;
        shooter_ = shooter;
        shooter_type_ = shooter->getObjectType();

        // start the expiration timer
        expiration_timer_.setTime(expiration_time_);
        is_alive_ = true;
    }

    void BulletObject::Update(double delta_time)
    {
        // update the expiration timer
        expiration_timer_.Update(delta_time);

        // Update direction if the bullet is homing
        if (enemy_objects_ != nullptr) {
            // find the closest enemy
            if (!target_found_) {
                float closest_distance = 1000000.0f;
                for (auto& enemy : *enemy_objects_) {
                    float distance = glm::length(enemy->GetPosition() - (position_  + GetBearing() * 0.5f));
                    if (distance < closest_distance) {
                        closest_distance = distance;
                        target_position_ = enemy->GetPosition();
                        target_found_ = true;
                    }
                }
            }
            // if an enemy is found, slowly change direction to face the enemy
            if (target_found_) {
                if (glm::length(target_position_ - position_) > 0.3f) {
                    glm::vec3 direction = glm::normalize(target_position_ - position_);
                    glm::vec3 diff = direction - direction_;
                    direction_ += diff * (float)delta_time;
                    SetRotation(glm::atan(direction_.y, direction_.x) - glm::pi<float>() / 2.0f);
                } else {
                    target_found_ = false;
                }
            }

        }

        // Move the bullet in the direction it was shot
        position_ += direction_ * speed_ * (float)delta_time;
        GameObject::Update(delta_time);

        // update trail if it exists
        if (trail_ != nullptr)
        {
            trail_->Update(delta_time);
        }

        // if the bullet has expired, remove it from the game
        if (expiration_timer_.timesUp())
        {
            is_alive_ = false;  // main game loop will remove the bullet from the game
        }
    }

    void BulletObject::Render(glm::mat4 view_matrix, double current_time)
    {

        // render trail if it exists
        if (trail_ != nullptr)
        {
            trail_->Render(view_matrix, current_time);
        }

        GameObject::Render(view_matrix, current_time);
    }

    // check if the bullet has already hit the object
    bool BulletObject::Pierce(GameObject *object)
    {
        for (auto &hit_object : hit_objects_)
        {
            if (hit_object == object)
            {
                return false;
            }
        }

        hit_objects_.push_back(object);
        return true;
    }
}