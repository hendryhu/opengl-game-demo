#ifndef SAW_OBJECT_H_
#define SAW_OBJECT_H_

#include "game_object.h"

namespace game {

	class SawObject : public GameObject {
		
	public:
		SawObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);

		void Update(double delta_time) override;

		// getters
		inline float getDamage() { return damage; };

		// Helpers
		bool isSaw() override { return true; };

	private:
		float damage = SAW_DAMAGE;
		
		float turn_speed = 2 * glm::pi<float>();
	};

}

#endif
