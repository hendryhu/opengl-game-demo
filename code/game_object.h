#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <glm/glm.hpp>
#define GLEW_STATIC
#include <vector>
#include <GL/glew.h>

#include "config.h"
#include "helper.h"
#include "timer.h"

#include "shader.h"
#include "geometry.h"

namespace game {

    /*
        GameObject is responsible for handling the rendering and updating of one object in the game world
        The update and render methods are virtual, so you can inherit them from GameObject and override the update or render functionality (see PlayerGameObject for reference)
    */
    class GameObject {

        public:
            // Constructor
            GameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture);

            // Destructor
            virtual ~GameObject();

            // Update the GameObject's state. Can be overriden in children
            virtual void Update(double delta_time);

            // Renders the GameObject 
            virtual void Render(glm::mat4 view_matrix, double current_time);

            // Getters
            inline glm::vec3 GetPosition(void) const { return position_; }
            inline glm::vec2 GetScaleVec(void) const { return scale_; }
            inline float GetScale(void) const { return scale_.x; }
            inline float GetRotation(void) const { return angle_; }
            inline Geometry* getGeometry() const { return geometry_; };
            inline Shader* getShader() const { return shader_; };
            inline GLuint getTexture() const { return texture_; };
            inline float getHitbox() { return hitbox_radius; };
            inline float getWeight() { return weight; };
            inline float getHealth() { return health; };
            inline float getMaxHealth() { return max_health; };
            inline float getMeleeDamage() { return melee_damage; };

            // Get bearing direction (direction in which the game object
            // is facing)
            glm::vec3 GetBearing(void) const;

            // Get vector pointing to the right side of the game object
            glm::vec3 GetRight(void) const;

            // Setters
            inline void SetPosition(const glm::vec3& position) { position_ = position; }
            inline void SetScale(float scale) { scale_ = glm::vec2(scale, scale); }
            inline void SetScale(const glm::vec2& scale) { scale_ = scale; }
            void SetRotation(float angle);
            inline void setHitboxRadius(float radius) { hitbox_radius = radius; }
            inline void setWeight(float weight) { this->weight = weight; };
            inline void setHealth(float health) { this->health = health; this->max_health = health; };
            virtual inline void takeDamage(float damage) { health -= damage; };
            inline void heal(float heal) { health += heal; };

            // Add child objects to the game object
            void AddChildObject(GameObject* object);

            // Knockback
            void knockback(GameObject* knockback_object, float multiplier);

            // Helpers
            virtual bool isPlayer() { return false; };
            virtual bool isTurret() { return false; };
            virtual bool isEnemy() { return false; };
            virtual bool isBullet() { return false; };
            virtual bool isExplosion() { return false; };
            virtual bool isUI() { return false; };
            virtual bool isArm() { return false; };
            virtual bool isSaw() { return false; };
            ObjectType getObjectType() { return object_type; };

            bool isDead() const { return health <= 0.0f; };


        protected:
            // Object's Transform Variables
            glm::vec3 position_;
            glm::vec2 scale_;
            float angle_;

            // Geometry
            Geometry *geometry_;
 
            // Shader
            Shader *shader_;

            // Object's texture reference
            GLuint texture_;

            // child objects
            std::vector<GameObject*> child_objects_;

            // Object type
            ObjectType object_type = ObjectType::Enemy;

            // properties
            float hitbox_radius = 0.1f;
            float max_health = 50;
            float health = 50;
            float melee_damage = 10.0f;

            // for knockback
            float knockback_force = 5.0f;   // max knockback force for all objects
            float weight = 1.0f;
            float knockback_deceleration = 10.0f;
            glm::vec3 knockback_velocity = glm::vec3(0.0f, 0.0f, 0.0f);

    }; // class GameObject

} // namespace game

#endif // GAME_OBJECT_H_
