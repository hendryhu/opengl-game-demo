#ifndef UI_OBJECT_H_
#define UI_OBJECT_H_

#include "config.h"

#include "game_object.h"
#include "health_bar.h"
#include "arrow.h"

namespace game {

	class UIObject : public GameObject {

	public:
		UIObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GameObject* player);
		~UIObject();

		void setUp(Geometry* health_bar_geom, GLuint arrow_texture, Shader* arrow_shader,
			GameObject** win_1, GameObject** win_2, GameObject** win_3, GameObject** win_4);

		void Update(double delta_time) override;

		void Render(glm::mat4 view_matrix, double current_time) override;

	private:
		GameObject* player_;

		HealthBar* health_bar;
		glm::vec3 health_bar_position = glm::vec3(-11.5, -8, -5);

		Arrow* arrow;
		glm::vec3 arrow_position = glm::vec3(0, 0, -5);

	}; // class HealthBar

} // namespace game

#endif