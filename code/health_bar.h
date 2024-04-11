#ifndef HEALTH_BAR_H_
#define HEALTH_BAR_H_

#include "config.h"

#include "game_object.h"

namespace game {

	class HealthBar : public GameObject {

	public:
		HealthBar(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GameObject* player);
		~HealthBar();

		void Render(glm::mat4 view_matrix, double current_time) override;

		// Getters
		inline glm::vec3 getModelPosition() { return model_position; };

	private:
		GameObject* player;

		glm::vec3 model_position;

	}; // class HealthBar

} // namespace game

#endif