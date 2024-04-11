#ifndef PARTICLES_H_
#define PARTICLES_H_

#include "geometry.h"

#define NUM_PARTICLES 600

namespace game {

    // A set of particles that can be rendered
    class Particles : public Geometry {

    public:
        Particles(void);

        // Create the geometry (called once)
        // span: angle of sector in respect to a full circle, 0 - 1
        // shift: angle of sector from 0 degrees in respect to a full circle, 0 - 1
        // range: the range the particles will go, >= 0.0f
        // phrase_mod: how centralized the particles are, 1 - 100
        void CreateGeometry(float span, float shift, float range, int phrase_mod);

        // Use the geometry
        void SetGeometry(GLuint shader_program);

    }; // class Particles
} // namespace game

#endif // PARTICLES_H_
