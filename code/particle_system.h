#ifndef PARTICLE_SYSTEM_H_
#define PARTICLE_SYSTEM_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class ParticleSystem : public GameObject {

    public:
        ParticleSystem(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GameObject* parent, int type, float cycle, float speed, float gravity);
        ParticleSystem(const ParticleSystem& other);

        // Getters
        inline int getType() { return type; };
        inline float getCycle() { return cycle; };
        inline float getSpeed() { return speed; };
        inline float getGravity() { return gravity; };

        // Setters
        inline void setParent(GameObject* parent) { parent_ = parent; };

        void Update(double delta_time) override;

        void Render(glm::mat4 view_matrix, double current_time);

    private:
        GameObject* parent_;

        // attributes
        float cycle;
        float speed;
        float gravity;

        int type;


        double current_time;


    }; // class ParticleSystem

} // namespace game

#endif // PARTICLE_SYSTEM_H_
