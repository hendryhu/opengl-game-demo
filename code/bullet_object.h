#ifndef BULLET_OBJECT_H
#define BULLET_OBJECT_H

#define MAX_HOMING_DISTANCE 30.0f

#include "config.h"
#include "game_object.h"
#include "timer.h"
#include "particle_system.h"

namespace game
{
    class BulletObject : public GameObject
    {
    public:
        BulletObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture,
            float damage, const glm::vec3& direction, float speed, float expiration_time, float hitbox_radius = 0.0f, BulletType type = BulletType::NORMAL);
        BulletObject(const BulletObject& other);
        ~BulletObject() override;

        // Getters
        glm::vec3 GetDirection() const { return direction_; }
        inline float GetSpeed() const { return speed_; };
        ObjectType GetShooterType() const { return shooter_type_; }
        float GetDamage() const { return damage_; }
        bool IsAlive() const { return is_alive_; }
        BulletType GetBulletType() const { return type_; }

        // setters
        void SetScale(float scale) {
            GameObject::SetScale(scale);
            hitbox_radius = scale / 10.f;
        }
        void SetTrail(ParticleSystem* trail, float scale) {
            trail_ = new ParticleSystem(*trail);
            trail_->setParent(this);
            trail_->SetScale(scale);
        }
        void SetEnemyObjects(vector<GameObject*>* enemy_objects) { enemy_objects_ = enemy_objects; }

        // utility
        void Shoot(const glm::vec3& position, const glm::vec3& direction, GameObject* shooter);
        void Update(double delta_time) override;
        bool Pierce(GameObject* object);
        void Render(glm::mat4 view_matrix, double current_time) override;

        // helpers
        bool isBullet() override { return true; };

    private:
        // bullet specific variables
        BulletType type_;
        float damage_;
        glm::vec3 direction_;
        float speed_;
        GameObject* shooter_ = nullptr;
        ObjectType shooter_type_;

        // expiration variables
        float expiration_time_;
        Timer expiration_timer_;
        bool is_alive_ = true;

        // homing variables
        vector <GameObject*> hit_objects_;
        vector <GameObject*>* enemy_objects_ = nullptr;
        glm::vec3 target_position_;
        bool target_found_ = false;

        // particle system for the bullet trail
        ParticleSystem* trail_ = nullptr;
    };
}


#endif //BULLET_OBJECT_H
