#ifndef PLAYER_GAME_OBJECT_H_
#define PLAYER_GAME_OBJECT_H_

#include <vector>
#include <glm/gtc/constants.hpp>

#include "config.h"

#include "game_object.h"
#include "turret_object.h"

namespace game
{
    // Inherits from GameObject
    class PlayerGameObject : public GameObject
    {
    public:
        PlayerGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);
        ~PlayerGameObject() override = default;

        // Setters
        inline void SetMovementInput(glm::vec2 movement_input) { this->movement_input = movement_input; };
        void takeDamage(float damage) override;
        void SetDefaultTurret(TurretObject* turret) { default_turret_ = turret; };

        // Getters
        inline glm::vec3 GetVelocity() { return current_velocity_; };
        inline bool inIFrame() { return (!iframe_timer_.timesUp()); };
        inline TurretObject* GetFirstTurret() { return first_turret_; };
        vector<TurretObject*>* GetTurrets()
        {
            auto turrets = new vector<TurretObject*>();
            for (auto& child : child_objects_)
            {
                auto* turret = dynamic_cast<TurretObject*>(child);
                if (turret != nullptr)
                {
                    turrets->push_back(turret);
                }
            }
            return turrets;
        }

        // Update function for moving the player object around
        void Update(double delta_time) override;
        void Render(glm::mat4 view_matrix, double current_time) override;

        // Turret-related functions
        void TurnTurrets(const glm::vec2 cursor_position, double delta_time);
        void ShootTurrets(std::vector<GameObject*>& bullets, WeaponShootKey key);
        void SetTurret(TurretObject* turret);

        // Helpers
        bool isPlayer() override { return true; };

    private:
        // private variables that handles the player's movement
        float max_speed_ = 10.0f;
        glm::vec3 current_velocity_;

        float acceleration_ = 7.0f;
        float front_friction = 5.0f;
        float side_friction = 15.0f;


        float turning_speed_ = 0;
        float rotational_acceleration = 1.2f;
        float rotational_friction = 1.0f;
        float max_turning_speed_ = 0.5f;

        float iframe = 0.5f;    // invincibility time (in seconds)
        Timer iframe_timer_;

        // input
        glm::vec2 movement_input = glm::vec2(0.0f, 0.0f);

        // default turret
        TurretObject* default_turret_ = nullptr;
        TurretObject* first_turret_ = nullptr;
    }; // class PlayerGameObject
} // namespace game

#endif // PLAYER_GAME_OBJECT_H_
