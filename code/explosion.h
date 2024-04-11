// Modified ------------------------------------------------------------------------//

#ifndef EXPLOSION_H_
#define EXPLOSION_H_

# include "game_object.h"
#include "particle_system.h"
#include "timer.h"

namespace game {
	class Explosion : public GameObject {

	public:
		Explosion(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, int type, float cycle, float speed, float gravity, float scale);
		~Explosion();

		// Getter
		const double GetTimer(void);
		inline float GetDespawnTime() const { return despawn_time; };

		// Update function for updating timer
		void Update(double delta_time) override;

		bool timesUp();

		// Render function
		void Render(glm::mat4 view_matrix, double current_time) override;

		// Helpers
        inline bool isExplosion() override { return true; };

	protected:
		Timer timer;
		float despawn_time;
		ParticleSystem* particles;
	};
}

#endif

// End Modified ------------------------------------------------------------------------//