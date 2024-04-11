#ifndef CODE_COLLECTIBLE_GAME_OBJECT_H
#define CODE_COLLECTIBLE_GAME_OBJECT_H

#include "game_object.h"
#include "config.h"

namespace game
{
    class CollectibleGameObject : public game::GameObject
    {
    public:
        CollectibleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, CollectibleType type);

        void Update(double delta_time) override;

        // getters
        inline CollectibleType GetType() const { return type_; }

        // setters
        void SetScale(float scale) {
            GameObject::SetScale(scale);
            hitbox_radius = scale/2.0f;
        }
        void SetMovement(float radius, float time, const glm::vec2& center);

    private:
        CollectibleType type_;

        // parametric circular motion variables
        bool circular_movement = false;
        float radius_ = 0.0f;
        float time_ = 0.0f;
        float phase_ = 0.0f;
        glm::vec2 center_ = glm::vec2(0.0f, 0.0f);
    };
}


#endif //CODE_COLLECTIBLE_GAME_OBJECT_H
