#ifndef TURRET_OBJECT_H
#define TURRET_OBJECT_H

#include "config.h"
#include "bullet_object.h"
#include "game_object.h"

namespace game
{
    class TurretObject : public GameObject
    {
    public:
        // constructor and destructor for the TurretObject
        TurretObject(GameObject* parent, TurretType type,
                     const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture,
                     const glm::vec3& parent_offset = glm::vec3(0.0f, 0.0f, -1.0f),
                     float hitbox = 1.0f);
        ~TurretObject() override = default;

        // deep copy constructor
        TurretObject(const TurretObject& other);

        void Update(double delta_time) override;
        void Render(glm::mat4 view_matrix, double current_time) override;

        // turret specific functions
        void Control(const glm::vec2 cursor_position, double delta_time);

        // setters
        void SetBullet(BulletObject* bullet) { this->bullet = bullet; }
        inline void SetFireCooldown(float cooldown) { fire_cooldown = cooldown; }
        BulletObject* Fire();
        void SetAngleOffset(float offset) { angle_offset = offset; }
        void SetParentRotationOffset(float offset);
        void SetHitbox(float hitbox) { hitbox_radius = hitbox; }

        // getters
        bool IsExpired() { return expired; }
        inline TurretType GetType() { return type; }

        // Helpers
        bool isTurret() override { return true; };

    private:
        // turret control variables
        glm::mat4 view_matrix_ = glm::mat4(1.0f);  // view matrix for rendering the turret
        float rotation_speed = 1.0f;  // speed at which the turret rotates
        glm::vec3 parent_offset = glm::vec3(0.0f, 0.0f, -1.0f);  // relative to the parent object's origin
        float parent_rotation_offset = glm::pi<float>() / 2.0f;  // offset to the parent's rotation
        float angle_offset = 0.0f;  // offset to the angle of the turret

        // turret specific variables
        TurretType type;
        GameObject* parent;

        // bullet variables
        float fire_cooldown = 0.5f;  // time between shots
        Timer* fire_cooldown_timer;  // timer to control the fire rate of the turret
        BulletObject* bullet = nullptr;  // bullet object to be fired (every time a bullet is fired, a copy of this object is created)

        // expiration variables
        float expiration_time = 0.0f; // time until the turret expires
        Timer* expiration_timer; // timer to control the expiration of the turret
        bool expired = false; // flag to indicate if the turret has expired
    };
} // game

#endif //TURRET_OBJECT_H
