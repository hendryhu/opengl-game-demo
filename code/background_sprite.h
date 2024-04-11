#ifndef BACKGROUND_SPRITE_H_
#define BACKGROUND_SPRITE_H_

#include "geometry.h"

namespace game {

    // A sprite (i.e., a square composed of two triangles)
    class BackgroundSprite : public Geometry {

        public:
            // Constructor and destructor
            BackgroundSprite(void);

            // Create the geometry (called once)
            void CreateGeometry(void);

            // Use the geometry
            void SetGeometry(GLuint shader_program);

    }; // class Sprite
} // namespace game

#endif // BACKGROUND_SPRITE_H_
