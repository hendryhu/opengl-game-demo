#include "arrow.h"

namespace game {

	Arrow::Arrow(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GameObject* player, GameObject** win_1, GameObject** win_2, GameObject** win_3, GameObject** win_4)
		: GameObject(position, geom, shader, texture) {
		model_position = position;
		this->player = player;
		win_collectibles.push_back(win_1);
		win_collectibles.push_back(win_2);
		win_collectibles.push_back(win_3);
		win_collectibles.push_back(win_4);
	}

	Arrow::~Arrow() {};


	void Arrow::Render(glm::mat4 view_matrix, double current_time) {
		// Set up the shader
		shader_->SetUniform1i("object_type", 1001);
		GameObject::Render(view_matrix, current_time);
	}


	void Arrow::Update(double delta_time) {
		GameObject* closest_collectible = NULL;
		float min_distance = std::numeric_limits<float>::max();

		for (int i = 0; i < win_collectibles.size(); i++) {
			GameObject* current = *win_collectibles[i];
			if (current != NULL) {
				float current_distance = glm::length(current->GetPosition() - player->GetPosition());
				if (current_distance < min_distance) {
					min_distance = current_distance;
					closest_collectible = current;
				}
			}
		}
		if (closest_collectible != NULL) {
			glm::vec3 direction = closest_collectible->GetPosition() - player->GetPosition();
			SetRotation(atan2(direction.y, direction.x));
		}
	}
}

