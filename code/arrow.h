#ifndef ARROW_H_
#define ARROW_H_

#include <limits>

#include "game_object.h"

namespace game {

	class Arrow : public GameObject {
	
	public:
		Arrow(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GameObject* player,
			GameObject** win_1, GameObject** win_2, GameObject** win_3, GameObject** win_4);
		~Arrow();

		void Render(glm::mat4 view_matrix, double current_time) override;

		void Update(double delta_time) override;

		// Getters
		inline glm::vec3 getModelPosition() { return model_position; };

	private:
		GameObject* player;

		glm::vec3 model_position;

		std::vector<GameObject**> win_collectibles;
	};

}

#endif
